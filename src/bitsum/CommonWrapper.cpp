#include "Common.hpp"
#include "crypto/crypto.hpp"
#include "CryptoNote.hpp"
#include "common/Base58.hpp"
#include "seria/BinaryInputStream.hpp"

using namespace bytecoin;

namespace bitsum {
	
	EXPORT bool CheckLib()
	{
		return true;
	}

	EXPORT bool CheckAddress(char address[])
	{
		BinaryArray data;
		uint64_t prefix = 154;
		AccountPublicAddress adr;

		try
		{
			if (!common::base58::decode_addr(address, prefix, data))
				return false;
			seria::from_binary(adr, data);
			return key_isvalid(adr.spend_public_key) && key_isvalid(adr.view_public_key);
		}
		catch (const std::exception)
		{
			return false;
		}
	}
}