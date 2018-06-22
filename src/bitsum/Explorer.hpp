#pragma once
#include <stdint.h>
#include "Core/BlockChainState.hpp"
#include "CryptoNote.hpp"
#include "crypto/types.hpp"
#include "rpc_api.hpp"

namespace explorer
{
	using namespace crypto;
	using namespace bytecoin;

	//typedef struct Hash
	//{
	//	uint8_t bytes[32];
	//};

	struct BlockPreview
	{
		uint8_t hash[32];
		uint32_t height;
		uint32_t timestamp;
		uint32_t size;
		uint32_t tx_count;
		uint64_t difficulty;
	};

	struct BlockPreview2
	{
		uint32_t length;
		//crypto::Hash * hash;
		std::vector<crypto::Hash> hash;
	};

	class Explorer
	{
		bytecoin::BlockChainState *_blockchain;

	public:
		Explorer(bytecoin::BlockChainState *blockChain);
		crypto::Hash GetBlockHash(uint32_t height);
		BlockPreview GetBlockPreview(uint32_t height);
		BlockPreview2 GetBlockPreview2(uint32_t height);
		crypto::Hash GetBlockPreview3(uint32_t height);
		std::vector<crypto::Hash> GetBlockPreview4(uint32_t height);
		~Explorer();
	};
}
