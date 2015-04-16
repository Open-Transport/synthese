
/** RoadJourneyPlannerAdmin class header.
	@file RoadJourneyPlannerAdmin.h
	@author Thomas Bonfort
	@date 2010

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

#ifndef SYNTHESE_RoadTripPlannerAdmin_H__
#define SYNTHESE_RoadTripPlannerAdmin_H__

#include "AdminInterfaceElementTemplate.h"
#include "GraphConstants.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace graph
	{
	}

	namespace road_journey_planner
	{
		/** Road journey planner screen for administration console.
			@ingroup m59Admin refAdmin
			@author Thomas Bonfort
			@date 2010
			@since 3.2.0

			Parameters :
				- PARAMTER_PLANNING_ORDER / po : 0 : departure first, 1 : arrival first
				- PARAMETER_DATE / da : if route planning order is departure first : min date of departure, else max date arrival
				- PARAMETER_DATE / ti : if route planning order is departure first : min hour of departure, else max date arrival
		*/
		class RoadJourneyPlannerAdmin:
			public admin::AdminInterfaceElementTemplate<RoadJourneyPlannerAdmin>
		{
			std::string	_startPlace;
			std::string	_endPlace;
			graph::UserClassCode _accessibility;

		public:
			static const std::string PARAMETER_START_PLACE;
			static const std::string PARAMETER_END_PLACE;
			static const std::string PARAMETER_ACCESSIBILITY;

			RoadJourneyPlannerAdmin();


			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author Thomas Bonfort
				@date 2010
				@since 3.2.0
			*/
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			/** Parameters map generator, used when building an url to the admin page.
				@return util::ParametersMap The generated parameters map
				@author Thomas Bonfort
				@date 2010
				@since 3.2.0
			*/
			virtual util::ParametersMap getParametersMap() const;



			/** Display of the content of the admin element.
				@param stream Stream to write on.
				@author Thomas Bonfort
				@date 2010
				@since 3.2.0
			*/
			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;



			/** Authorization control.
				@return bool True if the displayed page can be displayed
				@author Thomas Bonfort
				@date 2010
				@since 3.2.0
			*/
			bool isAuthorized(
				const security::User& user
			) const;

			/** Gets sub page of the designed parent page, which are from the current class.
				@param parentLink Link to the parent page
				@param currentPage Currently displayed page
				@return PageLinks each subpage of the parent page designed in parentLink
				@author Thomas Bonfort
				@date 2010
				@since 3.2.0
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const server::ModuleClass& module,
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// @author Thomas Bonfort
			/// @date 2010
			/// @since 3.2.0
			virtual bool isPageVisibleInTree(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;
		};
	}
}

#endif // SYNTHESE_RoadTripPlannerAdmin_H__
