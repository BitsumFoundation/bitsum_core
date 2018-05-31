#pragma once
#include <stdint.h>
#include "crypto/types.hpp"
#include "CryptoNote.hpp"
#include "Core/BlockChainState.hpp"
#include "rpc_api.hpp"

namespace explorer {

	using namespace crypto;
	using namespace bytecoin;

	typedef struct Hash { uint8_t bytes[32]; } Hash;

	struct BlockPreview {
		uint8_t hash[32];
		uint32_t height;
		uint32_t timestamp;
		uint32_t size;
		uint32_t tx_count;
	};

	class Explorer
	{
		bytecoin::BlockChainState* _blockChain;
	public:
		Explorer(bytecoin::BlockChainState* blockChain);
		crypto::Hash GetBlockHash(uint32_t height);
		BlockPreview GetBlockPreview(uint32_t height);
		~Explorer();
	};

}

