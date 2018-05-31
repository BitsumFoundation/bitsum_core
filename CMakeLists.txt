# Copyright (c) 2012-2018, The CryptoNote developers, The Bytecoin developers.
# Licensed under the GNU Lesser General Public License. See LICENSE for details.

cmake_minimum_required(VERSION 3.5)
project(bitsum_core)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)
# message(STATUS "Bitsumd profile: According to cmake, sizeof(void *) == " ${CMAKE_SIZEOF_VOID_P})
option(USE_INSTRUMENTATION "For testing - builds with address sanitizer instrument" OFF)
option(WITH_THREAD_SANITIZER "For testing - builds with thread sanitizer instrument, USE_INSTRUMENTATION must be also set" OFF)
option(USE_SSL "Builds with support of https between wallet-rpc and bitsumd" OFF)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    if(WIN32)
	    option(USE_SQLITE "Builds with SQLite instead of LMDB. 4x slower, but works on 32-bit and mobile platforms" ON)
	else()
		option(USE_SQLITE "Builds with SQLite instead of LMDB. 4x slower, but works on 32-bit and mobile platforms" OFF)
    endif()
    set(OPENSSL_ROOT ../openssl)
else()
    option(USE_SQLITE "Builds with sqlite instead of LMDB. 4x slower, but works on 32-bit and mobile platforms" ON)
    set(OPENSSL_ROOT ../openssl32)
endif()
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    add_definitions(-D_SCL_SECURE_NO_WARNINGS=1 -D_CRT_SECURE_NO_WARNINGS=1 -D_WIN32_WINNT=0x0501)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -maes -g -O2 -Wall -Wextra -Werror=return-type -Wno-unused-parameter -Wno-unknown-pragmas")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -maes -g -O2 -Wall -Wextra -Werror=return-type -Wno-unused-parameter -Wno-unknown-pragmas")
    message(STATUS "Instrumentation usage: " ${USE_INSTRUMENTATION})
    message(STATUS "Thread sanitizer usage: " ${WITH_THREAD_SANITIZER})
    if(USE_INSTRUMENTATION)
        if(WITH_THREAD_SANITIZER)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address,undefined")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-omit-frame-pointer -fsanitize=address,undefined")
        endif()
    endif()
endif()
if(USE_SQLITE)
    # Requires dl on Linux, we add it unconditionally for simplicity.
    message(STATUS "Database selected: SQLite 3. Make sure it is put into ../sqlite/")
    include_directories(../sqlite)
    set(SRC_DB ../sqlite/sqlite3.c src/platform/DBsqlite3.cpp src/platform/DBsqlite3.hpp)
    add_definitions(-Dplatform_USE_SQLITE=1)
else()
    message(STATUS "Database selected: LMDB. Make sure it is put into ../lmdb/")
    include_directories(../lmdb/libraries/liblmdb)
    set(SRC_DB ../lmdb/libraries/liblmdb/mdb.c ../lmdb/libraries/liblmdb/midl.c src/platform/DBlmdb.cpp src/platform/DBlmdb.hpp)
endif()
if(USE_SSL)
    message(STATUS "SSL usage: ON. Make sure openssl headers are in " ${OPENSSL_ROOT} "/include and static libs are in " ${OPENSSL_ROOT} "/")
    include_directories(${OPENSSL_ROOT}/include)
    link_directories(${OPENSSL_ROOT}) # Must be placed before add_executable, add_library.
    set(LINK_OPENSSL ssl crypto)
    add_definitions(-Dplatform_USE_SSL=1)
else()
    message(STATUS "SSL usage: OFF")
endif()
file(GLOB SRC_BITSUM src/bitsum/*.cpp src/bitsum/*.hpp)
file(GLOB SRC_CRYPTO
    src/crypto/*.cpp src/crypto/*.hpp src/crypto/*.h
    src/crypto/blake256.c
    src/crypto/chacha8.c
    src/crypto/crypto-ops-data.c
    src/crypto/crypto-ops.c
    src/crypto/crypto-util.c
    src/crypto/groestl.c
    src/crypto/hash-extra-blake.c
    src/crypto/hash-extra-groestl.c
    src/crypto/hash-extra-jh.c
    src/crypto/hash-extra-skein.c
    src/crypto/hash.c
    src/crypto/jh.c
    src/crypto/keccak.c
    src/crypto/oaes_lib.c
    src/crypto/random.c
    src/crypto/skein.c
    src/crypto/slow-hash_x86.c
    src/crypto/tree-hash.c
)
file(GLOB SRC_COMMON src/common/*.cpp src/common/*.hpp)
file(GLOB SRC_SERIALIZATION src/Serialization/*.cpp src/Serialization/*.hpp)
file(GLOB SRC_SERIA src/seria/*.cpp src/seria/*.hpp)
file(GLOB SRC_LOGGING src/logging/*.cpp src/logging/*.hpp)
file(GLOB SRC_P2P src/p2p/*.cpp src/p2p/*.hpp)
file(GLOB SRC_CORE src/Core/*.cpp src/Core/*.hpp src/Core/*.h)
file(GLOB SRC_HTTP src/http/*.cpp src/http/*.hpp)
file(GLOB SRC_PLATFORM
    src/platform/ExclusiveLock.cpp src/platform/ExclusiveLock.hpp
    src/platform/Files.cpp src/platform/Files.hpp
    src/platform/Network.cpp src/platform/Network.hpp
    src/platform/PathTools.cpp src/platform/PathTools.hpp
    src/platform/PreventSleep.cpp src/platform/PreventSleep.hpp
    src/platform/Windows.hpp src/platform/DB.hpp
)
if(WIN32)
    set_property(SOURCE ${SRC_BITSUM} PROPERTY COMPILE_FLAGS -Ot)
    set_property(SOURCE ${SRC_CRYPTO} PROPERTY COMPILE_FLAGS -Ot)
    set_property(SOURCE ${SRC_DB} PROPERTY COMPILE_FLAGS "-Ot -w")
    set_property(SOURCE ${SRC_COMMON} PROPERTY COMPILE_FLAGS -Ot)
    set_property(SOURCE ${SRC_SERIALIZATION} PROPERTY COMPILE_FLAGS -Ot)
    set_property(SOURCE ${SRC_SERIA} PROPERTY COMPILE_FLAGS -Ot)
else()
    set_property(SOURCE ${SRC_BITSUM} PROPERTY COMPILE_FLAGS -O3)
    set_property(SOURCE ${SRC_CRYPTO} PROPERTY COMPILE_FLAGS -O3)
    set_property(SOURCE ${SRC_DB} PROPERTY COMPILE_FLAGS "-O3 -w")
    set_property(SOURCE ${SRC_COMMON} PROPERTY COMPILE_FLAGS -O3)
    set_property(SOURCE ${SRC_SERIALIZATION} PROPERTY COMPILE_FLAGS -O3)
    set_property(SOURCE ${SRC_SERIA} PROPERTY COMPILE_FLAGS -O3)
endif()
include_directories(src)
set(SOURCE_FILES
    ${SRC_DB}
    ${SRC_COMMON}
    ${SRC_HTTP}
    ${SRC_CORE}
    ${SRC_SERIALIZATION}
    ${SRC_SERIA}
    ${SRC_LOGGING}
    ${SRC_PLATFORM}
    ${SRC_P2P}
    src/CryptoNote.hpp
    src/CryptoNoteConfig.hpp
    src/rpc_api.hpp
    src/version.hpp
)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/x64")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/libs/x64")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/libs/x64")
else()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/x86")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/libs/x86")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/libs/x86")
endif()
if(WIN32)
    include_directories(SYSTEM ../boost)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        link_directories(SYSTEM ../boost/stage/lib) # must be before add_executable, add_library
    else()
        link_directories(SYSTEM ../boost/stage32/lib) # must be before add_executable, add_library
    endif()
endif()
add_library(bitsum SHARED ${SRC_BITSUM})
add_library(bitsum-crypto ${SRC_CRYPTO})
add_library(bitsum-core ${SOURCE_FILES})
target_link_libraries(bitsum-core bitsum-crypto)
if(WIN32)
    add_executable(wallet-rpc src/main_wallet_rpc.cpp src/bitsum.rc) # .rc works only if referenced directly in add_executable
    add_executable(bitsumd src/main_bitsumd.cpp src/bitsum.rc) # .rc works only if referenced directly in add_executable
else()
    add_executable(wallet-rpc src/main_wallet_rpc.cpp)
    add_executable(bitsumd src/main_bitsumd.cpp)
endif()
#add_executable(tests src/main_tests.cpp tests/io.hpp tests/crypto/test_crypto.cpp tests/hash/test_hash.cpp tests/json/test_json.cpp)
set(Boost_USE_STATIC_LIBS ON)
add_definitions(-DBOOST_BIND_NO_PLACEHOLDERS=1 -DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE=1) # boost::_1 conflicts with std::_1
target_link_libraries(bitsum bitsum-crypto bitsum-core)
target_link_libraries(wallet-rpc bitsum-crypto bitsum-core)
target_link_libraries(bitsumd bitsum-crypto bitsum-core)
#target_link_libraries(tests bitsum-crypto bitsum-core)
if(WIN32)
else()
    set(BOOST_ROOT ../boost)
    set(Boost_USE_MULTITHREADED OFF) # all boost libraries are multithreaded since some version
    find_package(Boost 1.65 REQUIRED COMPONENTS system)
    include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
    set(Boost_LIBRARIES "${Boost_LIBRARIES}")
    if(APPLE)
        set(BOOST_ROOT ../boost)
        find_package(Boost 1.62 REQUIRED COMPONENTS system)
        message(STATUS "Boost_INCLUDE_DIRS: " ${Boost_INCLUDE_DIRS})
        include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
        #set(Boost_LIBRARIES "${Boost_LIBRARIES}")
    else()
        message(STATUS "Boost_INCLUDE_DIRS: ../boost")
        include_directories(SYSTEM ../boost)
        set(Boost_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/../boost/stage/lib/libboost_system.a")
    endif()
    message(STATUS "Boost_LIBRARIES: " ${Boost_LIBRARIES})
    if(APPLE)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11")
        target_link_libraries(wallet-rpc "-framework Foundation" "-framework IOKit")
        target_link_libraries(bitsumd "-framework Foundation" "-framework IOKit")
    endif()
    target_link_libraries(bitsum ${Boost_LIBRARIES} ${LINK_OPENSSL} dl pthread)
	target_link_libraries(wallet-rpc ${Boost_LIBRARIES} ${LINK_OPENSSL} dl pthread)
    target_link_libraries(bitsumd ${Boost_LIBRARIES} ${LINK_OPENSSL} dl pthread)
    #target_link_libraries(tests ${Boost_LIBRARIES} ${LINK_OPENSSL} dl pthread)
endif()
