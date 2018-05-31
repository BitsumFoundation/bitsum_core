#pragma once

namespace bitsum {

#if defined(_MSC_VER)
	#define EXPORT extern "C" __declspec(dllexport)
#elif defined(__GNUC__)
	#define EXPORT extern "C"
#else
	#pragma warning Unknown dynamic link import/export semantics.
#endif

#if defined(__GNUC__)
#define ERROR_SUCCESS 0;
#endif
#define ERROR_UNKNOWN 1;
#define ERROR_NODE_NOT_INITIALIZED 2;
#define ERROR_WALLET_NOT_INITIALIZED 3;

	struct PrivateKeyPair {
		unsigned char spend_key[32];
		unsigned char view_key[32];
	};
}
