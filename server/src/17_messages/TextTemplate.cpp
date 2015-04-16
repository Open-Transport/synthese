
/** TextTemplate class implementation.
	@file TextTemplate.cpp

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

#include "TextTemplate.h"
#include "Registry.h"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const string Registry<messages::TextTemplate>::KEY("TextTemplate");
	}

	namespace messages
	{
		TextTemplate::TextTemplate(
			RegistryKeyType key
		):	Registrable(key)
		{
		}

		void TextTemplate::setAlarmLevel( AlarmLevel level )
		{
			_level = level;
		}

		void TextTemplate::setLongMessage( const std::string& message )
		{
			_long_message = message;
		}

		void TextTemplate::setShortMessage( const std::string& message )
		{
			_short_message = message;
		}

		void TextTemplate::setName( const std::string& name )
		{
			_name = name;
		}

		synthese::messages::AlarmLevel TextTemplate::getAlarmLevel() const
		{
			return _level;
		}

		const std::string& TextTemplate::getLongMessage() const
		{
			return _long_message;
		}

		const std::string& TextTemplate::getShortMessage() const
		{
			return _short_message;
		}

		std::string TextTemplate::getName() const
		{
			return _name;
		}



		bool TextTemplate::getIsFolder() const
		{
			return _isFolder;
		}



		void TextTemplate::setIsFolder( bool value )
		{
			_isFolder = value;
		}
	}
}
