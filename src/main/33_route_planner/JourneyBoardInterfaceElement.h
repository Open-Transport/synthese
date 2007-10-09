
/** JourneyBoardInterfaceElement class header.
	@file JourneyBoardInterfaceElement.h

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


#ifndef SYNTHESE_JourneyBoardInterfaceElement_H__
#define SYNTHESE_JourneyBoardInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
    namespace server 
    {
		class Request;
    }

	namespace routeplanner
	{
		/** Journey board Library Interface Element Class.
			@ingroup m53Library refLibrary

			Parameters :
			 - 0 : Handicapped filter
			 - 1 : Bike filter
		*/
		class JourneyBoardInterfaceElement : public util::FactorableTemplate<interfaces::LibraryInterfaceElement,JourneyBoardInterfaceElement>
		{
		private:
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _handicappedFilter;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _bikeFilter;
			
		public:
			std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL
				) const;
			
			/** Parser.
				@param text Text to parse : standard list of parameters
					-# Handicapped filter
					-# Bike filter
			*/
			void storeParameters(interfaces::ValueElementList& vel);
		};

	}
}

#endif // SYNTHESE_JourneyBoardInterfaceElement_H__
