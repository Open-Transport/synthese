
/** DisplayScreenHasAlarmValueInterfaceElement class header.
	@file DisplayScreenHasAlarmValueInterfaceElement.h

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

#ifndef SYNTHESE_DisplayScreenHasAlarmValueInterfaceElement_H__
#define SYNTHESE_DisplayScreenHasAlarmValueInterfaceElement_H__

#include "LibraryInterfaceElement.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace interfaces
	{
		class ValueElementList;
	}
	namespace departurestable
	{
		/** Value Interface Element : Test of presence of an alarm on the display screen.
			@ingroup m54Values refLibrary

			Output : The level of the present alarm, ALARM_LEVEL_NO_ALARM if no alarm.
		*/
		class DisplayScreenHasAlarmValueInterfaceElement
			: public util::FactorableTemplate<interfaces::LibraryInterfaceElement, DisplayScreenHasAlarmValueInterfaceElement>
		{
		public:
			std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL) const;

			/** Parser.
			@param text Optional parameter
			*/
			void storeParameters(interfaces::ValueElementList& vel);
		};
	}
}

#endif // SYNTHESE_DisplayScreenHasAlarmValueInterfaceElement_H__
