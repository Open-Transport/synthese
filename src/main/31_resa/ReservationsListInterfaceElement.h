
/** ReservationsListInterfaceElement class header.
	@file ReservationsListInterfaceElement.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_ReservationsListInterfaceElement_H__
#define SYNTHESE_ReservationsListInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace html
	{
		class HTMLTable;
	}

	namespace resa
	{
		class Reservation;

		/** ReservationsListInterfaceElement Library Interface Element Class.
			@author Hugues Romain
			@date 2007
			@ingroup m31Library refLibrary

			@code reservations_list @endcode

			Parameters :
				- 0 : Line ID to search
				- 1 : Customer ID to search
				- 2 : Reservation name to search
				- 3 : date
				- 4 : display cancelled reservations
				- 5 : Title cell style
				- 6 : Table style

			Object : none

		*/
		class ReservationsListInterfaceElement
			: public util::FactorableTemplate<interfaces::LibraryInterfaceElement, ReservationsListInterfaceElement>
		{
			// List of parameters to store
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _lineId;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _userId;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _userName;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _date;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _displayCancelled;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _titleCellStyle;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _tableStyle;

			static void _DisplayReservation(std::ostream& stream, html::HTMLTable& t, const Reservation* reservation);

		public:
			/** Controls and store the internals parameters.
				@param vel Parameters list to read
			*/
			void storeParameters(interfaces::ValueElementList& vel);
			
			/* Displays the object.
				@param stream Stream to write on
				@param parameters Runtime parameters used in the internal parameters reading
				@param variables Runtime variables
				@param object Object to read at the display
				@param request Source request
			*/
			std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL
			) const;
				
			~ReservationsListInterfaceElement();
		};
	}
}

#endif // SYNTHESE_ReservationsListInterfaceElement_H__
