
/** Scenario class implementation.
	@file Scenario.cpp

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

#include "Scenario.h"

namespace synthese
{
	using namespace util;

	namespace messages
	{


		Scenario::Scenario()
			: Registrable<uid, Scenario>()
		{

		}

		void Scenario::setName( const std::string& name )
		{
			_name = name;
		}

		std::vector<Alarm*>& Scenario::getAlarms()
		{
			return _alarms;
		}

		const std::string& Scenario::getName() const
		{
			return _name;
		}

		void Scenario::setPeriodStart( const synthese::time::DateTime& periodStart )
		{
			_periodStart = periodStart;
		}

		void Scenario::setPeriodEnd( const synthese::time::DateTime& periodEnd )
		{
			_periodEnd = periodEnd;
		}

		void Scenario::setIsATemplate( bool isATemplate )
		{
			_isATemplate = isATemplate;
		}

		bool Scenario::getIsATemplate() const
		{
			return _isATemplate;
		}

		const time::DateTime& Scenario::getPeriodStart() const
		{
			return _periodStart;
		}

		const time::DateTime& Scenario::getPeriodEnd() const
		{
			return _periodEnd;
		}
	}
}
