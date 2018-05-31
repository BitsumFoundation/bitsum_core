#include "Common.hpp"
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

using namespace bytecoin;
using namespace common;

namespace bitsum {

	boost::asio::io_service node_io;
	std::unique_ptr<platform::ExclusiveLock> node_lock;
	std::unique_ptr<BlockChainState> blockchain_state;

	EXPORT int StartNode(bool coutRedirect = true)
	{
		try {
			if (coutRedirect) {
				std::cout.rdbuf(nullptr);
			}
			else {
				common::console::UnicodeConsoleSetup console_setup;
			}

			auto idea_start = std::chrono::high_resolution_clock::now();

			common::CommandLine cmd(1, 0);
			bytecoin::Config config(cmd);
			bytecoin::Currency currency(config.is_testnet);
			const std::string coinFolder = config.get_data_folder();

			// platform::ExclusiveLock coin_lock(coinFolder, "bitsumd.lock");
			node_lock = std::make_unique<platform::ExclusiveLock>(coinFolder, "bitsumd.lock");

			logging::LoggerManager logManager;
			logManager.configure_default(config.get_data_folder("logs"), "bitsumd-");

			// BlockChainState block_chain(logManager, config, currency);
			// std::unique_ptr<BlockChainState> block_chain;// (logManager, config, currency);
			blockchain_state = std::make_unique<BlockChainState>(logManager, config, currency);

			// boost::asio::io_service io;
			platform::EventLoop run_loop(node_io);

			Node node(logManager, config, *(blockchain_state.get()));

			auto idea_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::high_resolution_clock::now() - idea_start);
			std::cout << "bitsumd started seconds=" << double(idea_ms.count()) / 1000 << std::endl;
			while (!node_io.stopped()) {
				if (node.on_idle())  // Using it to load blockchain
					node_io.poll();
				else
					node_io.run_one();
			}

			return 0;
		}
		catch (const std::exception &) {
			return 1;
		}
	}

	EXPORT int StopNode()
	{
		try {
			node_io.stop();
			node_lock->~ExclusiveLock();
			return 0;
		}
		catch (const std::exception) {
			return -1;
		}
	}
}