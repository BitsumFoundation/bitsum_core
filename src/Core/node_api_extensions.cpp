// Copyright (c) 2017-2018, The Bitsum Foundation.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#include "node_api_extensions.hpp"
#include "Node.hpp"
#include "CryptoNoteTools.hpp"
#include "TransactionExtra.hpp"

using namespace bytecoin;

namespace extensions
{
	crypto::Hash get_block_hash(BlockChain* blockchain, uint32_t height)
	{
		crypto::Hash result;
		blockchain->read_chain(height, &result);

		return result;
	}

	Amount get_output_amount(const Transaction& transaction)
	{
		Amount amount = 0;
		for (auto& output : transaction.outputs)
		{
			amount += output.amount;
		}

		return amount;
	}

	Amount get_input_amount(const Transaction& transaction)
	{
		Amount amount = 0;
		for (auto& input : transaction.inputs)
		{
			if (input.type() == typeid(KeyInput))
			{
				amount += boost::get<KeyInput>(input).amount;
			}
		}

		return amount;
	}

	uint32_t get_mixin(const Transaction& transaction)
	{
		uint32_t result = 0;

		for (const TransactionInput& txin : transaction.inputs)
		{
			if (txin.type() != typeid(KeyInput))
			{
				continue;
			}

			uint32_t current_mixin = boost::get<KeyInput>(txin).output_indexes.size();
			if (current_mixin > result)
			{
				result = current_mixin;
			}
		}

		return result;
	}
}

Hash get_payment_id(const BinaryArray &extra) 
{
	Hash result;

	get_payment_id_from_tx_extra(extra, result);

	return result;
}

bool Node::on_get_blocks_json(http::Client *, http::RequestData &&, json_rpc::Request &&,
	api::extensions::GetBlocks::Request &&request, api::extensions::GetBlocks::Response &response)
{
	m_log(logging::INFO) << "API_EX: get_blocks_json, height=" << request.height;

	if (request.height > m_block_chain.get_tip_height())
	{
		throw json_rpc::Error(json_rpc::INVALID_PARAMS, "Invalid block height");
	}

	uint32_t count = 30;
	uint32_t last;
	if (request.height <= count) 
	{ 
		last = 0;
		count = request.height;
	}
	else
	{
		last = request.height - count;
	}
	
	response.blocks.reserve(count);

	for (uint32_t i = request.height; i >= last; i--)
	{
		crypto::Hash bid = extensions::get_block_hash(&m_block_chain, i);

		api::extensions::BlockPreview bp = api::extensions::BlockPreview();
		RawBlock rb;
		api::BlockHeader bh; 

		m_block_chain.read_block(bid, &rb);
		m_block_chain.read_header(bid, &bh);

		bp.height = i;
		std::copy(std::begin(bid.data), std::end(bid.data), std::begin(bp.hash.data));
		bp.timestamp = bh.timestamp;
		bp.size = bh.block_size + ((uint32_t)rb.block.size() - (bh.block_size - bh.transactions_cumulative_size));
		bp.tx_count = (uint32_t)rb.transactions.capacity() + 1;
		bp.difficulty = bh.difficulty;

		response.blocks.push_back(bp);

		if (i == 0) break;
	}
	
	return true;
}

bool Node::on_get_block_json(http::Client *, http::RequestData &&, json_rpc::Request &&,
	api::extensions::GetBlock::Request &&request, api::extensions::GetBlock::Response &response)
{
	m_log(logging::INFO) << "API_EX: get_block_json, hash=" << common::pod_to_hex(request.hash);

	if (!m_block_chain.read_header(request.hash, &response.block.header))
	{
		throw json_rpc::Error(json_rpc::INVALID_PARAMS, "Internal node error: Can't get block by hash. (Hash: " + common::pod_to_hex(request.hash) + ").");
	}

	RawBlock raw_block;
	Block block;

	m_block_chain.read_block(request.hash, &raw_block);
	block.from_raw_block(raw_block);

	auto bh = &response.block.header;
	bh->block_size = bh->block_size + ((uint32_t)raw_block.block.size() - (bh->block_size - bh->transactions_cumulative_size));
	
	response.block.transactions.reserve(raw_block.transactions.size() + 1);

	api::extensions::TransactionPreview tp = api::extensions::TransactionPreview();
	tp.hash = get_transaction_hash(block.header.base_transaction);
	tp.fee = 0;
	tp.size = seria::to_binary(block.header.base_transaction).size();
	bh->transactions_cumulative_size += tp.size;
	tp.amount_out = extensions::get_output_amount(block.header.base_transaction);
	tp.mixin = 0; 
	response.block.transactions.push_back(tp);

	for (const Transaction t : block.transactions)
	{
		tp = api::extensions::TransactionPreview();
		tp.hash = get_transaction_hash(t);
		tp.amount_in = extensions::get_input_amount(t);
		tp.amount_out = extensions::get_output_amount(t);
		tp.fee = extensions::get_input_amount(t) - tp.amount_out;
		tp.size = seria::to_binary(t).size();
		tp.mixin = extensions::get_mixin(t);
		tp.payment_id = get_payment_id(t.extra);

		response.block.transactions.push_back(tp);
	}

	return true;
}

bool Node::on_get_transaction_json(http::Client *, http::RequestData &&, json_rpc::Request &&,
	api::extensions::GetTransaction::Request &&request, api::extensions::GetTransaction::Response &response)
{
	m_log(logging::INFO) << "API_EX: get_transaction_json, hash=" << common::pod_to_hex(request.hash);

	Height height = 0;
	Hash bid;
	size_t index_in_block = 0;
	if (!m_block_chain.read_transaction(request.hash, &response.transaction, &height, &bid, &index_in_block))
	{
		throw json_rpc::Error(json_rpc::INVALID_PARAMS, "Internal node error: Can't get transaction by hash. (Hash: " + common::pod_to_hex(request.hash) + ").");
	}

	RawBlock rb;
	api::BlockHeader bh;

	m_block_chain.read_block(bid, &rb);
	m_block_chain.read_header(bid, &bh);

	response.block.height = bh.height;
	std::copy(std::begin(bid.data), std::end(bid.data), std::begin(response.block.hash.data));
	response.block.timestamp = bh.timestamp;
	response.block.size = bh.block_size + ((uint32_t)rb.block.size() - (bh.block_size - bh.transactions_cumulative_size));
	response.block.tx_count = (uint32_t)rb.transactions.capacity() + 1;
	response.block.difficulty = bh.difficulty;

	response.transaction_details.hash = get_transaction_hash(response.transaction);
	response.transaction_details.amount_in = extensions::get_input_amount(response.transaction);
	response.transaction_details.amount_out = extensions::get_output_amount(response.transaction);
	if (response.transaction_details.amount_in < response.transaction_details.amount_out)
	{
		response.transaction_details.fee = 0;
	}
	else
	{
		response.transaction_details.fee = response.transaction_details.amount_in - response.transaction_details.amount_out;
	}
	
	response.transaction_details.size = seria::to_binary(response.transaction).size();
	response.transaction_details.mixin = extensions::get_mixin(response.transaction);
	response.transaction_details.payment_id = get_payment_id(response.transaction.extra);
	
	return true;
}

bool Node::on_get_mempool_json(http::Client *, http::RequestData &&, json_rpc::Request &&,
	api::extensions::GetMempool::Request &&, api::extensions::GetMempool::Response &response)
{
	m_log(logging::INFO) << "API_EX: get_mempool_json";

	auto v = m_block_chain.get_memory_state_transactions();

	response.transactions.reserve(v.size());

	for each (auto item in v)
	{
		api::extensions::TransactionPreview tp = api::extensions::TransactionPreview();
		tp.hash = get_transaction_hash(item.second.tx);
		tp.amount_in = extensions::get_input_amount(item.second.tx);
		tp.amount_out = extensions::get_output_amount(item.second.tx);
		tp.fee = extensions::get_input_amount(item.second.tx) - tp.amount_out;
		tp.size = seria::to_binary(item.second.tx).size();
		tp.mixin = extensions::get_mixin(item.second.tx);
		tp.payment_id = get_payment_id(item.second.tx.extra);
		response.transactions.push_back(tp);
	}
	
	return true;
}

namespace seria
{
	void ser_members(api::extensions::BlockPreview &v, ISeria &s)
	{
		seria_kv("difficulty", v.difficulty, s);
		seria_kv("hash", v.hash, s);
		seria_kv("height", v.height, s);
		seria_kv("size", v.size, s);
		seria_kv("timestamp", v.timestamp, s);
		seria_kv("tx_count", v.tx_count, s);
	}

	void ser_members(api::extensions::TransactionPreview &v, ISeria &s)
	{
		seria_kv("hash", v.hash, s);
		seria_kv("fee", v.fee, s);
		seria_kv("amount_in", v.amount_in, s);
		seria_kv("amount_out", v.amount_out, s);
		seria_kv("size", v.size, s);
		seria_kv("mixin", v.mixin, s);
		seria_kv("payment_id", v.payment_id, s);
	}

	void ser_members(api::extensions::Block &v, ISeria &s)
	{
		seria_kv("header", v.header, s);
		seria_kv("transactions", v.transactions, s);
	}

	void ser_members(api::extensions::GetBlocks::Request &v, ISeria &s)
	{
		seria_kv("height", v.height, s);
	}

	void ser_members(api::extensions::GetBlocks::Response &v, ISeria &s)
	{
		seria_kv("blocks", v.blocks, s);
	}

	void ser_members(api::extensions::GetBlock::Request &v, ISeria &s)
	{
		seria_kv("hash", v.hash, s);
	}

	void ser_members(api::extensions::GetBlock::Response &v, ISeria &s)
	{
		seria_kv("block", v.block, s);
	}

	void ser_members(api::extensions::GetTransaction::Request &v, ISeria &s)
	{
		seria_kv("hash", v.hash, s);
	}

	void ser_members(api::extensions::GetTransaction::Response &v, ISeria &s)
	{
		seria_kv("block", v.block, s);
		seria_kv("transaction", v.transaction, s);
		seria_kv("transaction_details", v.transaction_details, s);
	}

	void ser_members(api::extensions::GetMempool::Response &v, ISeria &s)
	{
		seria_kv("transactions", v.transactions, s);
	}
}