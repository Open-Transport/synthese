
/** TransportSiteAdmin class header.
	@file TransportSiteAdmin.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_TransportSiteAdmin_H__
#define SYNTHESE_TransportSiteAdmin_H__

#include "AdminInterfaceElementTemplate.h"
#include "GraphTypes.h"
#include "ResultHTMLTable.h"
#include "WebPageTableSync.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "PTJourneyPlannerService.hpp"

namespace synthese
{
	namespace html
	{
		class ActionResultHTMLTable;
	}

	namespace cms
	{
		class WebPageAdmin;
		class WebPageDisplayFunction;
		class WebPageRemoveAction;
		class WebPageAddAction;
	}

	namespace pt_website
	{
		class PTServiceConfig;
		class RollingStockFilter;


		/** Transport web site administration page class.
			@ingroup m56Admin refAdmin
			@author Hugues Romain
			@date 2008
		*/
		class TransportSiteAdmin:
			public admin::AdminInterfaceElementTemplate<TransportSiteAdmin>
		{
		public:
			static const std::string PARAMETER_SEARCH_RANK;
			static const std::string PARAMETER_JOURNEY_PLANNING_ALGORITHM;

			static const std::string TAB_PROPERTIES;
			static const std::string TAB_PERIMETER;
			static const std::string TAB_ROUTE_PLANNING;

		private:
			boost::shared_ptr<PTServiceConfig>	_config;
			pt_journey_planner::PTJourneyPlannerService	_journeyPlanner;
			bool							_pt_journey_planning;

		public:
			TransportSiteAdmin();

			boost::shared_ptr<PTServiceConfig> getSite() const { return _config; }
			void setSite(boost::shared_ptr<PTServiceConfig> value){ _config = value; }

			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author Hugues Romain
				@date 2008
			*/
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			/** Parameters map generator, used when building an url to the admin page.
					@return util::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007
				*/
			virtual util::ParametersMap getParametersMap() const;



			/** Display of the content of the admin element.
				@param stream Stream to write on.
				@author Hugues Romain
				@date 2008
			*/
			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;



			/** Authorization control.
				@return bool True if the displayed page can be displayed
				@author Hugues Romain
				@date 2008
			*/
			bool isAuthorized(
				const security::User& user
			) const;

			/** Gets sub page of the designed parent page, which are from the current class.
				@param parentLink Link to the parent page
				@param currentPage Currently displayed page
				@return PageLinks each subpage of the parent page designed in parentLink
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const server::ModuleClass& module,
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;



			virtual AdminInterfaceElement::PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;


			/** Title generator.
				@return The title of the page
				@author Hugues Romain
				@date 2008
			*/
			virtual std::string getTitle() const;



			//////////////////////////////////////////////////////////////////////////
			/// Builds the tabs of the page.
			/// @param profile The profile of the current user
			/// @author Hugues Romain
			/// @date 2010
			virtual void _buildTabs(
				const security::Profile& profile
			) const;



			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;
		};
}	}

#endif // SYNTHESE_TransportSiteAdmin_H__
