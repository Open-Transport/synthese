
/** DisplayType class implementation.
	@file DisplayType.cpp

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

#include "DisplayType.h"

#include "MessageType.hpp"
#include "ParametersMap.h"
#include "Webpage.h"

using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace util
	{
		template<> const std::string Registry<departure_boards::DisplayType>::KEY("DisplayType");
	}


	namespace departure_boards
	{
		const string DisplayType::ATTR_ID = "id";
		const string DisplayType::ATTR_NAME = "name";
		const string DisplayType::ATTR_MESSAGE_TYPE_ID = "message_type_id";
		const string DisplayType::ATTR_ROWS_NUMBER = "rows_number";
		const string DisplayType::ATTR_MONITORING_PAGE_ID = "monitoring_page_id";
		const string DisplayType::ATTR_MESSAGE_IS_DISPLAYED_PAGE_ID = "message_is_displayed_page_id";



		const Interface* DisplayType::getDisplayInterface() const
		{
			return _displayInterface;
		}



		DisplayType::DisplayType(util::RegistryKeyType id)
		:	Registrable(id),
			_displayInterface(NULL),
			_audioInterface(NULL),
			_monitoringInterface(NULL),
			_rowNumber(1),
			_timeBetweenChecks(minutes(1)),
			_displayMainPage(NULL),
			_displayRowPage(NULL),
			_displayDestinationPage(NULL),
			_displayTransferDestinationPage(NULL),
			_monitoringParserPage(NULL),
			_messageIsDisplayedPage(NULL),
			_messageType(NULL)
		{}



		void DisplayType::setDisplayInterface(const Interface* interf )
		{
			_displayInterface = interf;
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
			const time_duration& value
		){
			_timeBetweenChecks = value;
		}



		const time_duration& DisplayType::getTimeBetweenChecks(
		) const {
			return _timeBetweenChecks;
		}



		void DisplayType::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const	{
			pm.insert(ATTR_ID, getKey());
			pm.insert(ATTR_NAME, getName());
			pm.insert(ATTR_MESSAGE_TYPE_ID, _messageType ? _messageType->getKey() : RegistryKeyType(0));
			pm.insert(ATTR_ROWS_NUMBER, _rowNumber);
			pm.insert(
				ATTR_MONITORING_PAGE_ID,
				_monitoringParserPage ? _monitoringParserPage->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				ATTR_MESSAGE_IS_DISPLAYED_PAGE_ID,
				_messageIsDisplayedPage ? _messageIsDisplayedPage->getKey() : RegistryKeyType(0)
			);
		}
}	}
