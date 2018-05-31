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

	boost::asio::io_service wallet_io;
	std::unique_ptr<platform::ExclusiveLock> wallet_lock;
	std::unique_ptr<Wallet> wallet;
	std::unique_ptr<WalletState> wallet_state;

	EXPORT int StartWallet(char path[], char password[], bool coutRedirect = true)
	{
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
		// std::unique_ptr<platform::ExclusiveLock> walletcache_lock;
		//std::unique_ptr<Wallet> wallet;

		try {
			wallet = std::make_unique<Wallet>(path, password, false);
			wallet_lock = std::make_unique<platform::ExclusiveLock>(
				config.get_data_folder("wallet_cache"), wallet->get_cache_name() + ".lock");
		}
		catch (const std::ios_base::failure &ex) {
			std::cout << ex.what() << std::endl;
			return api::WALLET_FILE_READ_ERROR;
		}
		catch (const platform::ExclusiveLock::FailedToLock &ex) {
			std::cout << ex.what() << std::endl;
			return api::WALLET_WITH_THE_SAME_VIEWKEY_IN_USE;
		}
		catch (const Wallet::Exception &ex) {
			std::cout << ex.what() << std::endl;
			return ex.return_code;
		}

		logging::LoggerManager logManagerWalletNode;
		logManagerWalletNode.configure_default(config.get_data_folder("logs"), "wallet-");

		//WalletState wallet_state(*wallet, logManagerWalletNode, config, currency);
		wallet_state = std::make_unique<WalletState>(*wallet, logManagerWalletNode, config, currency);

		// boost::asio::io_service _io;
		wallet_io.reset();
		platform::EventLoop run_loop(wallet_io);

		std::unique_ptr<Node> node;
		std::unique_ptr<WalletNode> wallet_node;
		try {
			wallet_node = std::make_unique<WalletNode>(nullptr, logManagerWalletNode, config, *(wallet_state.get()));
		}
		catch (const boost::system::system_error &ex) {
			std::cout << ex.what() << std::endl;
			return api::WALLETD_BIND_PORT_IN_USE;
		}

		auto idea_ms =
			std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - idea_start);
		std::cout << "wallet-rpc started seconds=" << double(idea_ms.count()) / 1000 << std::endl;

		while (!wallet_io.stopped()) {
			if (node && node->on_idle())  // We load blockchain there
				wallet_io.poll();
			else
				wallet_io.run_one();
		}

		//std::cout << "WEX";
		return 0;
	}

	EXPORT int StopWallet()
	{
		try {
			wallet_io.stop();
			
			delete wallet_state.release();
			delete wallet.release();
			delete wallet_lock.release();

			return ERROR_SUCCESS; 
		}
		catch (const std::exception &ex) {
			std::cout << ex.what() << std::endl;
			return ERROR_UNKNOWN;
		}
	}

	EXPORT bool CheckWalletPassword(char path[], char password[])
	{
		std::unique_ptr<Wallet> wallet;
		try {
			wallet = std::make_unique<Wallet>(path, password, false);
			return true;
		}
		catch (const std::exception &) {
			return false;
		}
	}

	EXPORT int CreateWallet(char path[], char password[])
	{
		std::unique_ptr<Wallet> wallet;

		try {
			wallet = std::make_unique<Wallet>(path, password, true);

			return 0;
		}
		catch (const std::ios_base::failure &ex) {
			std::cout << ex.what() << std::endl;
			return api::WALLET_FILE_READ_ERROR;
		}
		catch (const platform::ExclusiveLock::FailedToLock &ex) {
			std::cout << ex.what() << std::endl;
			return api::WALLET_WITH_THE_SAME_VIEWKEY_IN_USE;
		}
		catch (const Wallet::Exception &ex) {
			std::cout << ex.what() << std::endl;
			return ex.return_code;
		}
	}

	EXPORT int ImportWallet(char path[], char password[], char keys[])
	{
		std::unique_ptr<Wallet> wallet;

		try {
			wallet = std::make_unique<Wallet>(path, password, true, keys);
			// auto addr = wallet->get_first_address();
			// std::cout << Currency::get_account_address_as_str(154, addr);
			return 0;
		}
		catch (const std::ios_base::failure &ex) {
			std::cout << ex.what() << std::endl;
			return api::WALLET_FILE_READ_ERROR;
		}
		catch (const platform::ExclusiveLock::FailedToLock &ex) {
			std::cout << ex.what() << std::endl;
			return api::WALLET_WITH_THE_SAME_VIEWKEY_IN_USE;
		}
		catch (const Wallet::Exception &ex) {
			std::cout << ex.what() << std::endl;
			return ex.return_code;
		}
	}

	EXPORT int ChangeWalletPassword(char newPassword[])
	{
		// std::unique_ptr<Wallet> wallet;

		try {
			// wallet = std::make_unique<Wallet>(path, oldPassword, false);
			wallet_state->get_wallet().set_password(newPassword);
			return 0;
		}
		catch (const std::ios_base::failure &ex) {
			std::cout << ex.what() << std::endl;
			return api::WALLET_FILE_READ_ERROR;
		}
		catch (const platform::ExclusiveLock::FailedToLock &ex) {
			std::cout << ex.what() << std::endl;
			return api::WALLET_WITH_THE_SAME_VIEWKEY_IN_USE;
		}
		catch (const Wallet::Exception &ex) {
			std::cout << ex.what() << std::endl;
			return ex.return_code;
		}
	}

	EXPORT int ChangeContainerPassword(char path[], char oldPassword[], char newPassword[])
	{
		std::unique_ptr<Wallet> wallet;

		try {
			wallet = std::make_unique<Wallet>(path, oldPassword, false);
			wallet->set_password(newPassword);
			return 0;
		}
		catch (const std::ios_base::failure &ex) {
			std::cout << ex.what() << std::endl;
			return api::WALLET_FILE_READ_ERROR;
		}
		catch (const platform::ExclusiveLock::FailedToLock &ex) {
			std::cout << ex.what() << std::endl;
			return api::WALLET_WITH_THE_SAME_VIEWKEY_IN_USE;
		}
		catch (const Wallet::Exception &ex) {
			std::cout << ex.what() << std::endl;
			return ex.return_code;
		}
	}

	std::string get_first_address()
	{
		AccountPublicAddress fa = wallet->get_first_address();
		BinaryArray ba = seria::to_binary(fa);
		return common::base58::encode_addr(154, ba);
	}

	EXPORT int GetFirstAddress(char * &address)
	{
		if (wallet == nullptr)
		{
			return ERROR_WALLET_NOT_INITIALIZED;
		}

		try {
			std::string sa = get_first_address();
			
			address = new char[sa.length() + 1];
			strcpy(address, sa.c_str());

			return ERROR_SUCCESS;
		}
		catch (const std::exception &ex) {
			std::cout << ex.what() << std::endl;
			return ERROR_UNKNOWN;
		}
	}

	EXPORT int GetBalance(char address[], bool total, api::Balance &balance)
	{
		try {
			if (wallet_state == nullptr)
			{
				return ERROR_WALLET_NOT_INITIALIZED;
			}

			std::string sa;
			std::cout << "1";
			if (!total)
			{
				std::cout << ">1";
				if (address != nullptr)
				{
					std::cout << "2";
					sa = std::string(address);
				}
				else
				{
					std::cout << "3";
					sa = get_first_address();
				}
			}
			std::cout << "4";
			std::cout << sa << "<ADD";
			balance = wallet_state->get_balance(sa, -6);

			return ERROR_SUCCESS;
		}
		catch (const std::exception &ex) {
			std::cout << ex.what() << std::endl;
			return ERROR_UNKNOWN;
		}
	}

	EXPORT int GetKeys(char address[], PrivateKeyPair &keys) {
		if (wallet == nullptr)
		{
			return ERROR_WALLET_NOT_INITIALIZED;
		}

		try {
			keys = PrivateKeyPair();

			SecretKey vk = wallet->get_view_secret_key();
			std::copy(std::begin(vk.data), std::end(vk.data), std::begin(keys.view_key));

			BinaryArray data;
			uint64_t prefix = 154;
			AccountPublicAddress adr;
			AccountKeys ak;

			if (!common::base58::decode_addr(address, prefix, data)) return false; // parsing error
			seria::from_binary(adr, data);
			wallet->spend_keys_for_address(adr, ak);
			SecretKey sk = ak.spend_secret_key;
			std::copy(std::begin(sk.data), std::end(sk.data), std::begin(keys.spend_key));

			return ERROR_SUCCESS;
		}
		catch (const std::exception &ex) {
			std::cout << ex.what() << std::endl;
			return ERROR_UNKNOWN;
		}
	}

}