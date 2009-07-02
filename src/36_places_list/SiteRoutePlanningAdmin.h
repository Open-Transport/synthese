
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

#include "GraphTypes.h"

#include "AdminInterfaceElementTemplate.h"
#include "DateTime.h"

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
			graph::UserClassCode			_accessibility;
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
			
			boost::shared_ptr<const Site> getSite() const;
			void setSite(boost::shared_ptr<const Site> value);

			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author Hugues Romain
				@date 2008
			*/
			void setFromParametersMap(
				const server::ParametersMap& map,
				bool doDisplayPreparationActions,
					bool objectWillBeCreatedLater
			);
			
			
			
			/** Parameters map generator, used when building an url to the admin page.
					@return server::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007					
				*/
			virtual server::ParametersMap getParametersMap() const;



			/** Display of the content of the admin element.
				@param stream Stream to write on.
				@author Hugues Romain
				@date 2008
			*/
			void display(std::ostream& stream, interfaces::VariablesMap& variables,
					const server::FunctionRequest<admin::AdminRequest>& _request) const;

			/** Authorization control.
				@return bool True if the displayed page can be displayed
				@author Hugues Romain
				@date 2008
			*/
			bool isAuthorized(
				const server::FunctionRequest<admin::AdminRequest>& _request
			) const;
		};
	}
}

#endif // SYNTHESE_RoutePlannerAdmin_H__
