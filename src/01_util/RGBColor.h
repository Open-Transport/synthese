
/** RGBColor class header.
	@file RGBColor.h

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

#ifndef SYNTHESE_UTIL_RGBCOLOR_H
#define SYNTHESE_UTIL_RGBCOLOR_H

#include "Exception.h"

#include <string>

namespace synthese
{
	namespace util
	{

		/** RGB Color structure.
			@ingroup m01
		*/
		struct RGBColor
		{
			/** Exception to raise if a constructor fails to initialize.
				@ingroup m01
			*/
			class Exception : public synthese::Exception
			{
			public:
				Exception();
			};

			int r;
			int g;
			int b;

			RGBColor (int red, int green, int blue)
				: r (red), g (green), b (blue) {}

			RGBColor (const std::string& colorName);


			bool operator == (const synthese::util::RGBColor& op2 ) const;

			std::string toString() const;



			/** Outputs the color in XML format (#RRGGBB).
				@return std::string the color in XML Format (#RRGGBB)
				@author Hugues Romain
				@date 2009
			*/
			std::string toXMLColor() const;

			static RGBColor FromXMLColor(const std::string& value);
		};
	}
}

#endif
