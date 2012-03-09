
/** Record class implementation.
	@file Record.cpp

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

#include "Record.hpp"

#include <boost/algorithm/string.hpp>

using namespace boost::algorithm;
using namespace std;

namespace synthese
{
	Record::Record()
	{
	}



	string Record::Trim( const string& value )
	{
		return trim_copy_if(value, is_any_of(" \r\n\t"));
	}



	bool Record::isTrue( const std::string& parameterName ) const
	{
		if(!isDefined(parameterName))
		{
			return false;
		}

		string value(Trim(getValue(parameterName)));

		if(value.empty())
		{
			return false;
		}

		if(value == "0")
		{
			return false;
		}

		return true;

	}
}

