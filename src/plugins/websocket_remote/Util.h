//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007-2016 musikcube team
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may
//      be used to endorse or promote products derived from this software
//      without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <boost/bimap.hpp>

#include <core/sdk/constants.h>
#include <core/sdk/IPreferences.h>

#ifdef WIN32
#include <Windows.h>
#endif

#ifdef __APPLE__
extern __thread char threadLocalBuffer[4096];
#else
extern thread_local char threadLocalBuffer[4096];
#endif

template <typename L, typename R>
boost::bimap<L, R>
static makeBimap(std::initializer_list<typename boost::bimap<L, R>::value_type> list) {
    /* http://stackoverflow.com/a/31841462 */
    return boost::bimap<L, R>(list.begin(), list.end());
}

static std::string GetPreferenceString(
    musik::core::sdk::IPreferences* prefs,
    const std::string& key,
    const std::string& defaultValue)
{
    prefs->GetString(key.c_str(), threadLocalBuffer, sizeof(threadLocalBuffer), defaultValue.c_str());
    return std::string(threadLocalBuffer);
}

template <typename MetadataT>
static std::string GetMetadataString(MetadataT* metadata, const std::string& key) {
    metadata->GetValue(key.c_str(), threadLocalBuffer, sizeof(threadLocalBuffer));
    return std::string(threadLocalBuffer);
}

#ifdef WIN32
static inline std::wstring utf8to16(const char* utf8) {
    int size = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, 0, 0);
    wchar_t* buffer = new wchar_t[size];
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, buffer, size);
    std::wstring utf16fn(buffer);
    delete[] buffer;
    return utf16fn;
}
#endif

std::string urlEncode(const std::string& s);
std::string urlDecode(const std::string& str);
