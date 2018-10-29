// Copyright (c) 2012-2018, The CryptoNote developers, The Bytecoin developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

// defines are for Windows resource compiler
#define bytecoin_VERSION_WINDOWS_COMMA 1, 18, 10, 29
#define bytecoin_VERSION_STRING "1.18.10.29"

#ifndef RC_INVOKED  // Windows resource compiler

namespace bytecoin {
inline const char *app_version() { return bytecoin_VERSION_STRING; }
}

#endif
