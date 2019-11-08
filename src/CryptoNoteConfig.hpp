// Copyright (c) 2012-2018, The CryptoNote developers, The Bytecoin developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

// All values below should only be used in code through Currency and Config classes, never directly.
// This approach allows unlimited customization through config file/command line parameters
// Never include this header into other headers
namespace bytecoin {
namespace parameters {

const uint32_t HARDFORK_V1_HEIGHT = 106195;
const uint32_t HARDFORK_V2_HEIGHT = 162000;
const uint32_t HARDFORK_V3_HEIGHT = 260000;
const uint32_t HARDFORK_V4_HEIGHT = 4294967294;

const uint32_t DIFFICULTY_TARGET = 120;  // seconds
const uint32_t CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT_V2 = DIFFICULTY_TARGET * 3;
const uint32_t BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW_V2 = 11;
const uint64_t DIFFICULTY_WINDOW_V2 = 60;
const uint64_t DIFFICULTY_BLOCKS_COUNT_V2 = DIFFICULTY_WINDOW_V2 + 1;

const uint32_t CRYPTONOTE_MAX_BLOCK_NUMBER             = 500000000;
const uint32_t CRYPTONOTE_MAX_BLOCK_BLOB_SIZE          = 500000000;
const uint32_t CRYPTONOTE_MAX_TX_SIZE                  = 1000000000;
const uint64_t CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX = 154; // addresses start with "Sm"
const uint32_t CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW    = 10;
const uint32_t CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT      = 60 * 60 * 2;

const uint32_t BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW = 60;

// MONEY_SUPPLY - total number coins to be generated
const uint64_t MONEY_SUPPLY          = std::numeric_limits<uint64_t>::max();
const unsigned EMISSION_SPEED_FACTOR = 18;
static_assert(EMISSION_SPEED_FACTOR <= 8 * sizeof(uint64_t), "Bad EMISSION_SPEED_FACTOR");

const size_t CRYPTONOTE_REWARD_BLOCKS_WINDOW = 100;
const size_t CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE =
    100000;  // size of block (bytes) after which reward for block calculated using block size
const size_t CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V2 = 20000;
const size_t CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1 = 10000;
// const size_t CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_CURRENT = CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE;
const size_t CRYPTONOTE_COINBASE_BLOB_RESERVED_SIZE = 600;
const size_t CRYPTONOTE_DISPLAY_DECIMAL_POINT       = 10;
const uint64_t MINIMUM_FEE                          = 1000000;  // pow(10, 6)
const uint64_t DEFAULT_DUST_THRESHOLD               = 1000000;  // pow(10, 6)

constexpr uint32_t EXPECTED_NUMBER_OF_BLOCKS_PER_DAY(uint32_t difficulty_target) {
	return 24 * 60 * 60 / difficulty_target;
}
constexpr uint32_t DIFFICULTY_WINDOW(uint32_t difficulty_target) {
	return EXPECTED_NUMBER_OF_BLOCKS_PER_DAY(difficulty_target);
}  // blocks
const size_t DIFFICULTY_CUT = 60;  // timestamps to cut after sorting
const size_t DIFFICULTY_LAG = 15;  // !!!
static_assert(
    2 * DIFFICULTY_CUT <= DIFFICULTY_WINDOW(DIFFICULTY_TARGET) - 2, "Bad DIFFICULTY_WINDOW or DIFFICULTY_CUT");

const size_t MAX_BLOCK_SIZE_INITIAL                  = 20 * 1024;
const uint64_t MAX_BLOCK_SIZE_GROWTH_SPEED_NUMERATOR = 100 * 1024;
constexpr uint64_t MAX_BLOCK_SIZE_GROWTH_SPEED_DENOMINATOR(uint32_t difficulty_target) {
	return 365 * 24 * 60 * 60 / difficulty_target;
}

// After next hardfork remove settings below
const uint32_t CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS = 1;
constexpr uint32_t CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_SECONDS(uint32_t difficulty_target) {
	return difficulty_target * CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS;
}

const uint32_t CRYPTONOTE_MEMPOOL_TX_LIVETIME = 60 * 60 * 24;  // seconds, one day
// const uint32_t CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME = 60 * 60 * 24 * 7; //seconds, one week
// const uint32_t CRYPTONOTE_NUMBER_OF_PERIODS_TO_FORGET_TX_DELETED_FROM_POOL = 7;  //
// CRYPTONOTE_NUMBER_OF_PERIODS_TO_FORGET_TX_DELETED_FROM_POOL * CRYPTONOTE_MEMPOOL_TX_LIVETIME = time to forget tx

// const size_t FUSION_TX_MAX_SIZE = CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_CURRENT * 30 / 100;
// const size_t FUSION_TX_MIN_INPUT_COUNT = 12;
// const size_t FUSION_TX_MIN_IN_OUT_COUNT_RATIO = 4;

const uint32_t UPGRADE_HEIGHT_V2 = 1;
const uint32_t UPGRADE_HEIGHT_V3 = 2;
const uint32_t UPGRADE_HEIGHT_V4 = HARDFORK_V2_HEIGHT;

const char CRYPTONOTE_BLOCKS_FILENAME[]       = "blocks.bin";
const char CRYPTONOTE_BLOCKINDEXES_FILENAME[] = "blockindexes.bin";
}  // parameters

const char CRYPTONOTE_NAME[] = "bitsum";

const uint8_t CURRENT_TRANSACTION_VERSION = 1;

const size_t BLOCKS_IDS_SYNCHRONIZING_DEFAULT_COUNT = 10000;  // by default, blocks ids count in synchronizing
const size_t BLOCKS_SYNCHRONIZING_DEFAULT_COUNT     = 100;    // by default, blocks count in blocks downloading
const size_t COMMAND_RPC_GET_BLOCKS_FAST_MAX_COUNT  = 1000;

const int P2P_DEFAULT_PORT        = 28080;
const int RPC_DEFAULT_PORT        = 28081;
const int WALLET_RPC_DEFAULT_PORT = 28082;

const size_t P2P_LOCAL_WHITE_PEERLIST_LIMIT = 1000;
const size_t P2P_LOCAL_GRAY_PEERLIST_LIMIT  = 5000;

const size_t P2P_CONNECTION_MAX_WRITE_BUFFER_SIZE        = 32 * 1024 * 1024;  // 32 Mb
const uint32_t P2P_DEFAULT_CONNECTIONS_COUNT             = 8;
const uint32_t P2P_DEFAULT_WHITELIST_CONNECTIONS_PERCENT = 70;
const uint32_t P2P_DEFAULT_HANDSHAKE_INTERVAL            = 60;        // seconds
const uint32_t P2P_DEFAULT_PACKET_MAX_SIZE               = 50000000;  // 50000000 bytes maximum packet size
const uint32_t P2P_DEFAULT_PEERS_IN_HANDSHAKE            = 250;
const uint32_t P2P_DEFAULT_CONNECTION_TIMEOUT            = 5000;           // 5 seconds
const uint32_t P2P_DEFAULT_PING_CONNECTION_TIMEOUT       = 2000;           // 2 seconds
const uint32_t P2P_DEFAULT_INVOKE_TIMEOUT                = 60 * 2 * 1000;  // 2 minutes
const uint32_t P2P_DEFAULT_HANDSHAKE_INVOKE_TIMEOUT      = 5000;           // 5 seconds
const char     P2P_STAT_TRUSTED_PUB_KEY[] = "7faf1b1a49aea1f5bc50f5df7b9087b622dc9b6e5080bae33828f71a4c7b807d";

const char *const SEED_NODES[] = {
	"89.40.4.205:28080",
	"109.235.66.242:28080",
};

struct CheckpointData {
	uint32_t index;
	const char *block_id;
};

constexpr const CheckpointData CHECKPOINTS[] = {
	{ 500000, "8b7248897387db20060f94bcef6d4fa9b6a9fc78c6f7df3b6e1a7540be2d453e" },
};
}  // CryptoNote
