
/** IConv class header.
	@file IConv.hpp

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

#ifndef SYNTHESE_util_IConv_hpp__
#define SYNTHESE_util_IConv_hpp__

#include "Exception.h"

namespace synthese
{
	namespace util
	{
		/** IConv class.
			@ingroup m01
		*/
		class IConv
		{
		private:
			const std::string _from;
			const std::string _to;
			void* _iconv;

		public:
			class BadCharsetException:
				public synthese::Exception
			{
			public:
				BadCharsetException(const IConv& obj);
			};

			class ImpossibleConversion:
				public synthese::Exception
			{
			public:
				ImpossibleConversion(
					const char* text,
					const IConv& obj
				);
			};

			IConv(
				const std::string& from,
				const std::string& to
			);
			~IConv();

			const std::string& getFrom() const { return _from; }
			const std::string& getTo() const { return _to; }

			//////////////////////////////////////////////////////////////////////////
			/// Character set conversion.
			/// @param text text to convert
			/// @param from character set of the text to convert
			/// @param to character set of the result
			/// @return the converted text
			/// @author Hugues Romain
			/// @since 3.2.0
			/// @date 2010
			std::string convert(
				const std::string& text
			) const;
		};
	}
}

#endif // SYNTHESE_util_IConv_hpp__
