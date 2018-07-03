// Copyright (c) 2017-2018, The Bitsum Foundation.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

#include "rpc_api.hpp"

using namespace crypto;

namespace bytecoin
{
	namespace api
	{
		namespace extensions
		{
			struct GetKeys
			{
				static std::string method() { return "get_keys"; }

				struct Request
				{
					std::string address;
				};

				struct Response
				{
					BinaryArray keys;
				};
			};
		}
	}
}

namespace seria
{
	void ser_members(bytecoin::api::extensions::GetKeys::Request &v, ISeria &s);
	void ser_members(bytecoin::api::extensions::GetKeys::Response &v, ISeria &s);
}