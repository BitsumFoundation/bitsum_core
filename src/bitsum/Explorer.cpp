#include "Explorer.hpp"

using namespace crypto;

namespace explorer 
{
	Explorer::Explorer(bytecoin::BlockChainState* blockChain)
	{
		_blockchain = blockChain;
	}

	crypto::Hash Explorer::GetBlockHash(uint32_t height)
	{
		crypto::Hash result;
		_blockchain->read_chain(height, &result);

		return result;
	}

	BlockPreview Explorer::GetBlockPreview(uint32_t height)
	{
		crypto::Hash bid = GetBlockHash(height);

		BlockPreview result;
		RawBlock rb;
		api::BlockHeader bh;

		_blockchain->read_block(bid, &rb);
		_blockchain->read_header(bid, &bh);

		result.height = height;
		std::copy(std::begin(bid.data), std::end(bid.data), std::begin(result.hash));
		result.timestamp = bh.timestamp;
		result.size = bh.block_size + ((uint32_t)rb.block.size() - (bh.block_size - bh.transactions_cumulative_size));
		result.tx_count = (uint32_t)rb.transactions.capacity() + 1;
		result.difficulty = bh.difficulty;
		
		return result;
	}

	BlockPreview2 Explorer::GetBlockPreview2(uint32_t height)
	{
		crypto::Hash bid = GetBlockHash(height);

		BlockPreview2 result;
		RawBlock rb;
		Block b;
		//api::BlockHeader bh;


		_blockchain->read_block(bid, &rb);
		b.from_raw_block(rb);
		result.length = b.header.transaction_hashes.capacity();
		//result.hashes

		//result.hash = b.header.transaction_hashes[0];
		
		//std::vector<Hash> hh(b.header.transaction_hashes);

		//for each (crypto::Hash h in b.header.transaction_hashes)
		//{
		//	
		//}
		//_blockchain->read_header(bid, &bh);
		
	    

		//result.height = height;
		//std::copy(std::begin(bid.data), std::end(bid.data), std::begin(result.hash));
		//result.timestamp = bh.timestamp;
		//result.size = bh.block_size + ((uint32_t)rb.block.size() - (bh.block_size - bh.transactions_cumulative_size));
		//result.tx_count = (uint32_t)rb.transactions.capacity() + 1;
		//result.difficulty = bh.difficulty;

		return result;
	}

	crypto::Hash Explorer::GetBlockPreview3(uint32_t height)
	{
		crypto::Hash bid = GetBlockHash(height);

		BlockPreview2 result;
		RawBlock rb;
		Block b;

		_blockchain->read_block(bid, &rb);
		b.from_raw_block(rb);
		return b.header.transaction_hashes[0];
	}

	std::vector<crypto::Hash> Explorer::GetBlockPreview4(uint32_t height)
	{
		crypto::Hash bid = GetBlockHash(height);

		BlockPreview2 result;
		RawBlock rb;
		Block b;

		_blockchain->read_block(bid, &rb);
		b.from_raw_block(rb);
		return b.header.transaction_hashes;
	}

	Explorer::~Explorer()
	{
	}
}


