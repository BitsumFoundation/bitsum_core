#include "Explorer.hpp"

using namespace crypto;

namespace explorer {

	Explorer::Explorer(bytecoin::BlockChainState* blockChain)
	{
		_blockChain = blockChain;
	}

	crypto::Hash Explorer::GetBlockHash(uint32_t height)
	{
		crypto::Hash result;
		_blockChain->read_chain(height, &result);

		return result;
	}

	BlockPreview Explorer::GetBlockPreview(uint32_t height)
	{
		crypto::Hash bid = GetBlockHash(height);

		BlockPreview result;
		RawBlock rb;
		api::BlockHeader bh;

		_blockChain->read_block(bid, &rb);
		_blockChain->read_header(bid, &bh);

		result.height = height;
		std::copy(std::begin(bid.data), std::end(bid.data), std::begin(result.hash));
		result.timestamp = bh.timestamp;
		result.size = bh.block_size + ((uint32_t)rb.block.size() - (bh.block_size - bh.transactions_cumulative_size));
		result.tx_count = (uint32_t)rb.transactions.capacity() + 1;
		
		return result;
	}


	Explorer::~Explorer()
	{
	}
}


