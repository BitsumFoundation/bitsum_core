// Copyright (c) 2017-2018, The Bitsum Foundation.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

#include "rpc_api.hpp"

using namespace crypto;

namespace bytecoin
{
	namespace api
	{
		namespace extensions
		{
			struct BlockPreview
			{
				crypto::Hash hash;
				uint32_t height;
				uint32_t timestamp;
				uint32_t size;
				uint32_t tx_count;
				uint64_t difficulty;
			};

			struct TransactionPreview
			{
				Hash hash;
				Amount fee;
				Amount amount_in;
				Amount amount_out;
				size_t size;
				size_t mixin;
				Hash payment_id;
			};

			struct Block
			{
				bytecoin::api::BlockHeader header;
				std::vector<TransactionPreview> transactions;
			};

			struct GetBlocks
			{
				static std::string method() { return "get_blocks_json"; }

				struct Request
				{
					Height height;
				};

				struct Response
				{
					std::vector<BlockPreview> blocks;
				};
			};

			struct GetBlock
			{
				static std::string method() { return "get_block_json"; }

				struct Request
				{
					crypto::Hash hash;
				};

				struct Response
				{
					Block block;
				};
			};

			struct GetTransaction
			{
				static std::string method() { return "get_transaction_json"; }

				struct Request
				{
					crypto::Hash hash;
				};

				struct Response
				{
					BlockPreview block;
					bytecoin::Transaction transaction;
					TransactionPreview transaction_details;
				};
			};

			struct GetMempool
			{
				static std::string method() { return "get_mempool_json"; }

				typedef EmptyStruct Request;

				struct Response
				{
					std::vector<TransactionPreview> transactions;
				};
			};
		}
	}
}

namespace seria
{
	void ser_members(bytecoin::api::extensions::BlockPreview &v, ISeria &s);
	void ser_members(bytecoin::api::extensions::Block &v, ISeria &s);
	void ser_members(bytecoin::api::extensions::GetBlocks::Request &v, ISeria &s);
	void ser_members(bytecoin::api::extensions::GetBlocks::Response &v, ISeria &s);
	void ser_members(bytecoin::api::extensions::GetBlock::Request &v, ISeria &s);
	void ser_members(bytecoin::api::extensions::GetBlock::Response &v, ISeria &s);
	void ser_members(bytecoin::api::extensions::GetTransaction::Request &v, ISeria &s);
	void ser_members(bytecoin::api::extensions::GetTransaction::Response &v, ISeria &s);
	void ser_members(bytecoin::api::extensions::GetMempool::Response &v, ISeria &s);
}
