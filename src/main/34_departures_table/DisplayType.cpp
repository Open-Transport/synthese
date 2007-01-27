
/** DisplayType class implementation.
	@file DisplayType.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "DisplayType.h"

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace departurestable
	{


		int DisplayType::getRowNumber() const
		{
			return _rowNumber;
		}

		const interfaces::Interface* DisplayType::getInterface() const
		{
			return _interf;
		}

		const std::string& DisplayType::getName() const
		{
			return _name;
		}

		DisplayType::DisplayType( const uid& id)
			: Registrable<uid, DisplayType>(id)
		{
			
		}

		void DisplayType::setRowNumber( int number )
		{
			_rowNumber = number;
		}

		void DisplayType::setInterface(const Interface* interf )
		{
			_interf = interf;
		}

		void DisplayType::setName( const std::string& name )
		{
			_name = name;
		}
	}
}