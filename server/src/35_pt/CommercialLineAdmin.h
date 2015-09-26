
/** CommercialLineAdmin class header.
	@file CommercialLineAdmin.h
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

#ifndef SYNTHESE_CommercialLineAdmin_H__
#define SYNTHESE_CommercialLineAdmin_H__

#include "ResultHTMLTable.h"

#include "AdminInterfaceElementTemplate.h"

#include "35_pt/AdvancedSelectTableSync.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace pt
	{
		class CommercialLine;
	}

	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// 35.14 : Commercial line edition.
		///	@ingroup m35Admin refAdmin
		///	@author Hugues Romain
		///	@date 2008
		///
		/// Parameters :
		///	<ul>
		///		<li>cc : if true launches the control of the respect of the calendar template and displays the run days</li>
		/// </ul>
		class CommercialLineAdmin:
			public admin::AdminInterfaceElementTemplate<CommercialLineAdmin>
		{
		public:
			static const std::string TAB_ROUTES_FORWARD;
			static const std::string TAB_ROUTES_BACKWARD;
			static const std::string TAB_FREE_DRT;
			static const std::string TAB_DATES;
			static const std::string TAB_NON_CONCURRENCY;
			static const std::string TAB_PROPERTIES;

			static const std::string PARAMETER_SEARCH_NAME;
			static const std::string PARAMETER_DATES_START;
			static const std::string PARAMETER_DATES_END;
			static const std::string PARAMETER_CALENDAR_CONTROL;

		protected:
			boost::shared_ptr<const pt::CommercialLine>	_cline;
			bool								_controlCalendar;
			std::string _searchName;
			boost::optional<boost::gregorian::date>	_startDate;
			boost::optional<boost::gregorian::date>	_endDate;

			html::ResultHTMLTable::RequestParameters	_requestParameters;

			void _displayRoutes(
				std::ostream& stream,
				const server::Request& request,
				bool wayBack
			) const;

		public:
			CommercialLineAdmin();

			virtual PageLinks _getCurrentTreeBranch() const;

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



			/** Sub pages getter.
				@return PageLinks Ordered vector of sub pages links
				@author Hugues Romain
				@date 2008

				The default implementation handles the auto registration of administrative components by getSuperiorVirtual() method.
				This method can be overloaded to create customized sub tree.
			*/
			virtual PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;

			/** Title generator.
				@return The title of the page
				@author Hugues Romain
				@date 2008
			*/
			virtual std::string getTitle() const;

			virtual void _buildTabs(
				const security::Profile& profile
			) const;

			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;

			//! @name Getters
			//@{
				boost::shared_ptr<const pt::CommercialLine> getCommercialLine() const { return _cline; }
			//@}

			//! @name Setters
			//@{
				void setCommercialLine(boost::shared_ptr<const pt::CommercialLine> value) { _cline = value; }
				void setControlCalendar(bool value) { _controlCalendar = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_CommercialLineAdmin_H__
