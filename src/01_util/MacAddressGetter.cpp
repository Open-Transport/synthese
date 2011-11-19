
/** MacAddressGetter class implementation.
	@file MacAddressGetter.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "MacAddressGetter.h"

#include <sstream>
#include <iomanip>

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <tchar.h>
#include <Windows.h>
#include <Iphlpapi.h>
#include <Assert.h>
#pragma comment(lib, "iphlpapi.lib")

#endif

using namespace std;
using namespace boost;

namespace synthese
{
	namespace util
	{
		// Fetches the MAC address and prints it
		string MacAddressGetter::GetMACaddress(
			optional<size_t> interfaceId
		){
#ifdef WIN32
			IP_ADAPTER_INFO AdapterInfo[16];			// Allocate information for up to 16 NICs
			DWORD dwBufLen = sizeof(AdapterInfo);		// Save the memory size of buffer

			DWORD dwStatus = GetAdaptersInfo(			// Call GetAdapterInfo
				AdapterInfo,							// [out] buffer to receive data
				&dwBufLen);								// [in] size of receive data buffer
			assert(dwStatus == ERROR_SUCCESS);			// Verify return value is valid, no buffer overflow

			size_t i(0);
			PIP_ADAPTER_INFO pAdapterInfo;
			for(pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
				pAdapterInfo &&
				(	!interfaceId && pAdapterInfo->AddressLength != 6 ||
					interfaceId && i < *interfaceId
				);						// Terminate if last adapter
				pAdapterInfo = pAdapterInfo->Next		// Progress through linked list
			);


			if(pAdapterInfo)
			{
				stringstream s;
				for(int j(0); j<6; ++j)
				{
					if(j>0) s << "-";
					if(pAdapterInfo->Address[j] < 16) s << "0";
					s << hex << static_cast<int>(pAdapterInfo->Address[j]);
				}
				return s.str();
			}

#endif
			return string();
		}
	}
}
