#include "common/CommandLine.hpp"
#include "common/ConsoleTools.hpp"
#include "Core/Node.hpp"
#include "Core/WalletNode.hpp"
#include "Core/Config.hpp"
#include "platform/ExclusiveLock.hpp"
#include "logging/LoggerManager.hpp"
#include "platform/Network.hpp"
#include "version.hpp"
#include "common/Base58.hpp"
#include "seria/BinaryInputStream.hpp"
#include "seria/BinaryOutputStream.hpp"
#include "Explorer.hpp"

using namespace bytecoin;
using namespace common;


//boost::asio::io_service node_io;
//std::unique_ptr<platform::ExclusiveLock> node_lock;
//std::unique_ptr<BlockChainState> blockchain_state;


//std::unique_ptr<explorer::Explorer> explorer;

//struct BlockHeader_ {
//	//explorer::Hash hashes[];
//	//std::vector<explorer::Hash> has;
//	std::vector<uint8_t> has{};
//};
//extern "C" _declspec(dllexport) void __Test(BlockPreview &bp)
//{
//	try
//	{
//		uint8_t buf[1];
//		crypto::Hash h = cn_fast_hash(buf, 1);
//		std::cout << common::pod_to_hex(h);
//
//		bp.height = 100;
//		std::copy(std::begin(h.data), std::end(h.data), std::begin(bp.hash));
//		bp.size = 101;
//		bp.timestamp = 102;
//		bp.tx_count = 103;
//
//	}
//	catch (const std::exception& ex)
//	{
//		std::cout << ex.what();
//	}
//
//}
//
//extern "C" _declspec(dllexport) void __Test2(BlockHeader_ &bp)
//{
//	try
//	{
//		uint8_t buf[1];
//		crypto::Hash h = cn_fast_hash(buf, 1);
//		std::cout << common::pod_to_hex(h);
//
//		//std::vector<explorer::Hash> has;
//		for (uint8_t i = 0; i < 5; i++)
//		{
//			//crypto::Hash h = cn_fast_hash(buf, 1);
//			//std::cout << common::pod_to_hex(h);
//
//			//explorer::Hash th = explorer::Hash();
//			//std::copy(std::begin(h.data), std::end(h.data), std::begin(th.bytes));
//			
//			bp.has.push_back(uint8_t());
//		}
//
//		//bp.height = 100;
//		//std::copy(std::begin(h.data), std::end(h.data), std::begin(bp.hash));
//		//bp.size = 101;
//		//bp.timestamp = 102;
//		//bp.tx_count = 103;
//
//}
//	catch (const std::exception& ex)
//	{
//		std::cout << ex.what();
//	}
//
//}





















