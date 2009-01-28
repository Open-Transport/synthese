
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
#include "Registry.h"

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace util
	{
		template<> const std::string Registry<departurestable::DisplayType>::KEY("DisplayType");
	}


	namespace departurestable
	{


		int DisplayType::getRowNumber() const
		{
			return _rowNumber;
		}

		const Interface* DisplayType::getDisplayInterface() const
		{
			return _displayInterface;
		}

		const std::string& DisplayType::getName() const
		{
			return _name;
		}

		DisplayType::DisplayType(util::RegistryKeyType id)
		:	Registrable(id),
			_displayInterface(NULL),
			_audioInterface(NULL),
			_monitoringInterface(NULL),
			_timeBetweenChecks(1),
			_rowNumber(1),
			_maxStopsNumber(UNKNOWN_VALUE)
		{
		}



		void DisplayType::setRowNumber( int number )
		{
			_rowNumber = number;
		}

		void DisplayType::setDisplayInterface(const Interface* interf )
		{
			_displayInterface = interf;
		}

		void DisplayType::setName( const std::string& name )
		{
			_name = name;
		}

		void DisplayType::setMaxStopsNumber( int number )
		{
			_maxStopsNumber = number;
		}

		int DisplayType::getMaxStopsNumber() const
		{
			return _maxStopsNumber;
		}



		void DisplayType::setMonitoringInterface(
			const interfaces::Interface* value
			) {
			_monitoringInterface = value;
		}



		void DisplayType::setAudioInterface(
			const interfaces::Interface* value
			) {
			_audioInterface = value;
		}



		const interfaces::Interface* DisplayType::getMonitoringInterface(

			) const {
			return _monitoringInterface;
		}



		const interfaces::Interface* DisplayType::getAudioInterface(

			) const {
			return _audioInterface;
		}



		void DisplayType::setTimeBetweenChecks(
			int value
		) {
			_timeBetweenChecks = value;
		}



		int DisplayType::getTimeBetweenChecks(
		) const {
			return _timeBetweenChecks;
		}
	}
}
