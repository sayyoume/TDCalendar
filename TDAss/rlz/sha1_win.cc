// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sha1.h"

#include <windows.h>
#include <wincrypt.h>
#include "scoped_capi_types.h"

// This file is not being compiled at the moment (see bug 47218). If we keep
// sha1 inside base, we cannot depend on src/crypto.
// #include "crypto/scoped_capi_types.h"

namespace base {

	template <class string_type>
	inline typename string_type::value_type* WriteInto(string_type* str,
		size_t length_with_null) {

		str->reserve(length_with_null);
		str->resize(length_with_null - 1);
		return &((*str)[0]);
	}

std::string SHA1HashString(const std::string& str) {
  ScopedHCRYPTPROV provider;
  if (!CryptAcquireContext(provider.receive(), NULL, NULL, PROV_RSA_FULL,
                           CRYPT_VERIFYCONTEXT)) 
  {
    return std::string(kSHA1Length, '\0');
  }

  {
    ScopedHCRYPTHASH hash;
    if (!CryptCreateHash(provider, CALG_SHA1, 0, 0, hash.receive())) 
	{
      return std::string(kSHA1Length, '\0');
    }

    if (!CryptHashData(hash, reinterpret_cast<CONST BYTE*>(str.data()),
                       static_cast<DWORD>(str.length()), 0)) 
	{
      return std::string(kSHA1Length, '\0');
    }

    DWORD hash_len = 0;
    DWORD buffer_size = sizeof hash_len;
    if (!CryptGetHashParam(hash, HP_HASHSIZE,
                           reinterpret_cast<unsigned char*>(&hash_len),
                           &buffer_size, 0)) 
	{
      return std::string(kSHA1Length, '\0');
    }

	std::string result;
	if (!CryptGetHashParam(hash, HP_HASHVAL,
		// We need the + 1 here not because the call will write a trailing \0,
		// but so that result.length() is correctly set to |hash_len|.
		reinterpret_cast<BYTE*>(WriteInto(&result, hash_len + 1)), &hash_len,0)) 
	{
		//DPLOG(ERROR) << "CryptGetHashParam(HP_HASHVAL) failed";
		return std::string(kSHA1Length, '\0');
	}

    if (hash_len != kSHA1Length) 
	{
      return std::string(kSHA1Length, '\0');
    }

    return result;
  }
}

}  // namespace base
