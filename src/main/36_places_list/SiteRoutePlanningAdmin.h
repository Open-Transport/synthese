
/** SiteRoutePlannerAdmin class header.
	@file SiteRoutePlannerAdmin.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_SiteRoutePlannerAdmin_H__
#define SYNTHESE_SiteRoutePlannerAdmin_H__

#include "36_places_list/Types.h"

#include "32_admin/AdminInterfaceElementTemplate.h"

#include "04_time/DateTime.h"

namespace synthese
{
	namespace transportwebsite
	{
		class Site;

		/** Site route planning admin component.
			@ingroup m36Admin refAdmin
			@author Hugues Romain
			@date 2008
		*/
		class SiteRoutePlanningAdmin : public admin::AdminInterfaceElementTemplate<SiteRoutePlanningAdmin>
		{
		private:
			boost::shared_ptr<const Site>	_site;
			std::string						_startCity;
			std::string						_startPlace;
			std::string						_endCity;
			std::string						_endPlace;
			time::DateTime					_dateTime;
			int								_resultsNumber;
			AccessibilityParameter			_accessibility;
			bool							_log;

		public:
			static const std::string PARAMETER_START_CITY;
			static const std::string PARAMETER_START_PLACE;
			static const std::string PARAMETER_END_CITY;
			static const std::string PARAMETER_END_PLACE;
			static const std::string PARAMETER_DATE_TIME;
			static const std::string PARAMETER_RESULTS_NUMBER;
			static const std::string PARAMETER_ACCESSIBILITY;
			static const std::string PARAMETER_LOG;

			SiteRoutePlanningAdmin();

			/** Initialization of the parameters from a parameters map.
			@param map The parameters map to use for the initialization.
			@throw AdminParametersException if a parameter has incorrect value.
			@author Hugues Romain
			@date 2008
			*/
			void setFromParametersMap(const server::ParametersMap& map);

			/** Display of the content of the admin element.
			@param stream Stream to write on.
			@param request The current request
			@author Hugues Romain
			@date 2008
			*/
			void display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request=NULL) const;

			/** Authorization control.
			@param request The current request
			@return bool True if the displayed page can be displayed
			@author Hugues Romain
			@date 2008
			*/
			bool isAuthorized(const server::FunctionRequest<admin::AdminRequest>* request) const;

			/** Gets sub page of the designed parent page, which are from the current class.
			@param parentLink Link to the parent page
			@param currentPage Currently displayed page
			@return PageLinks each subpage of the parent page designed in parentLink
			@author Hugues Romain
			@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfParent(
				const PageLink& parentLink
				, const AdminInterfaceElement& currentPage
				, const server::FunctionRequest<admin::AdminRequest>* request
			) const;

			/** Sub pages getter.
			@param currentPage Currently displayed page
			@param request User request
			@return PageLinks No subpages
			@author Hugues Romain
			@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPages(
				const PageLink& parentLink
				, const AdminInterfaceElement& currentPage
				, const server::FunctionRequest<admin::AdminRequest>* request
			) const;

			/** Parameter name getter.
				@return The name of the parameter of the page
				@author Hugues Romain
				@date 2008
			*/
			virtual std::string getParameterName() const;
			
			/** Parameter value getter.
				@return The value of the parameter of the page
				@author Hugues Romain
				@date 2008
			*/
			virtual std::string getParameterValue() const;

		};
	}
}

#endif // SYNTHESE_RoutePlannerAdmin_H__
