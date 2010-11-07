////////////////////////////////////////////////////////////////////////////////
///	ImpExModule class implementation.
///	@file ImpExModule.cpp
///	@author Hugues Romain (RCS)
///	@date mer jun 24 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "ImpExModule.h"
#include "Exception.h"

#include <iconv.h>

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace impex;
	
	namespace util
	{
		template<> const string FactorableTemplate<ModuleClass,ImpExModule>::FACTORY_KEY("16_impex");
	}
	
	namespace server
	{
		template<> const string ModuleClassTemplate<ImpExModule>::NAME("Import Export");
		
		template<> void ModuleClassTemplate<ImpExModule>::PreInit()
		{
		}
		
		template<> void ModuleClassTemplate<ImpExModule>::Init()
		{
		}
		
		template<> void ModuleClassTemplate<ImpExModule>::End()
		{
		}
	}

	namespace impex
	{
		std::string ImpExModule::ConvertChar(
			const std::string& text,
			const std::string& from,
			const std::string& to
		){
#if defined(__MINGW32__) || defined(_WIN32)
			const char *pBuf;
#else /* not MINGW32 - WIN32 */
			char *pBuf;
#endif
			size_t len;
			size_t utf8len;
			iconv_t cvt = iconv_open (to.c_str(), from.c_str());
			if (cvt == (iconv_t) (-1))
			{
				throw Exception("Bad charset");
			}
			len = text.length();
			utf8len = 4*text.size();
			pBuf = text.c_str();
			char* utf8buf = (char*) malloc(sizeof(char) * utf8len);
			char* pUtfbuf(utf8buf);
			if (iconv (cvt, &pBuf, &len, &pUtfbuf, &utf8len) == (size_t) (-1))
			{
				delete utf8buf;
				iconv_close (cvt);
				throw Exception("iconv error");
			}
			*pUtfbuf = 0;
			string result(utf8buf);
			delete utf8buf;
			iconv_close (cvt);
			return result;
		}
	}
}
