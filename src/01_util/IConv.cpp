
/** IConv class implementation.
	@file IConv.cpp

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

#include "IConv.hpp"

#include <iconv.h>
#include <stdlib.h>

using namespace std;

namespace synthese
{
	namespace util
	{
		IConv::BadCharsetException::BadCharsetException( const IConv& obj ):
			synthese::Exception("No such charset : "+ obj.getFrom()+" or "+ obj.getTo())
		{}



		IConv::ImpossibleConversion::ImpossibleConversion(
			const char* text,
			const IConv& obj
		):	synthese::Exception("A character could not be converted from "+ obj.getFrom()+" to "+ obj.getTo() +" in "+ text)
		{}



		std::string IConv::convert(
			const std::string& text
		) const {
			if(text.empty())
			{
				return string();
			}

			char *pBuf;
			size_t len;
			size_t utf8len;
			len = text.length();
			utf8len = 4*text.size();
			pBuf = const_cast<char*>(text.c_str());
			char* utf8buf = (char*) malloc(sizeof(char) * utf8len);
			char* pUtfbuf(utf8buf);
			if (iconv(_iconv, &pBuf, &len, &pUtfbuf, &utf8len) == (size_t) (-1))
			{
				free(utf8buf);
				throw ImpossibleConversion(pBuf, *this);
			}
			*pUtfbuf = 0;
			string result(utf8buf);
			free(utf8buf);
			return result;
		}



		IConv::IConv( const std::string& from, const std::string& to ):
			_from(from),
			_to(to),
			_iconv(iconv_open (to.c_str(), from.c_str()))
		{
			if(_iconv == (iconv_t) (-1))
			{
				throw BadCharsetException(*this);
			}
		}



		IConv::~IConv()
		{
			iconv_close(_iconv);
		}
}	}
