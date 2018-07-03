// Copyright (c) 2017-2018, The Bitsum Foundation.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#include "wallet_api_extensions.hpp"
#include "WalletNode.hpp"

using namespace bytecoin;

bool WalletNode::on_get_keys(http::Client *, http::RequestData &&, json_rpc::Request &&,
	api::extensions::GetKeys::Request &&request, api::extensions::GetKeys::Response &response)
{
	m_log(logging::INFO) << "API_EX: get_keys, address=" << request.address;

	AccountPublicAddress addr;
	if (!m_wallet_state.get_currency().parse_account_address_string(request.address, &addr))
	{
		throw json_rpc::Error(json_rpc::INVALID_PARAMS, "Invalid address");
	}

	AccountKeys keys;
	if (!m_wallet_state.get_wallet().spend_keys_for_address(addr, keys))
	{
		throw json_rpc::Error(json_rpc::INVALID_PARAMS, "Address not found");
	}

	response.keys = BinaryArray(64);
	std::copy(std::begin(keys.spend_secret_key.data), std::end(keys.spend_secret_key.data), std::begin(response.keys));
	std::copy(std::begin(keys.view_secret_key.data), std::end(keys.view_secret_key.data), std::begin(response.keys) + 32);

	return true;
}

namespace seria
{
	void ser_members(api::extensions::GetKeys::Request &v, ISeria &s)
	{
		seria_kv("address", v.address, s);
	}

	void ser_members(api::extensions::GetKeys::Response &v, ISeria &s)
	{
		seria_kv("keys", v.keys, s);
	}
}