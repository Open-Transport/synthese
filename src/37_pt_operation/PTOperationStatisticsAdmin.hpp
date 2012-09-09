

//////////////////////////////////////////////////////////////////////////
/// PTOperationStatisticsAdmin class header.
///	@file PTOperationStatisticsAdmin.hpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_PTOperationStatisticsAdmin_H__
#define SYNTHESE_PTOperationStatisticsAdmin_H__

#include "ResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"
#include "PTOperationStatisticsTableSync.hpp"

namespace synthese
{
	namespace pt_operation
	{
		//////////////////////////////////////////////////////////////////////////
		/// PTOperationStatisticsAdmin Admin compound class.
		///	@ingroup m37Admin refAdmin
		///	@author Hugues Romain
		///	@date 2011
		class PTOperationStatisticsAdmin:
			public admin::AdminInterfaceElementTemplate<PTOperationStatisticsAdmin>
		{
		public:
			/// @name Parameter identifiers
			//@{
				static const std::string PARAM_SEARCH_START_DATE;
				static const std::string PARAM_SEARCH_END_DATE;
				static const std::string PARAM_ROW_STEP;
				static const std::string PARAM_COL_STEP;
				static const std::string PARAM_WHAT;
			//@}

		private:
			/// @name Search parameters
			//@{
				boost::gregorian::date_period _searchPeriod;
				PTOperationStatisticsTableSync::Step _searchRowStep;
				PTOperationStatisticsTableSync::Step _searchColStep;
				PTOperationStatisticsTableSync::What _searchWhat;
			//@}

		protected:

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author Hugues Romain
			///	@date 2011
			PTOperationStatisticsAdmin();

			static std::string GetColumnName(PTOperationStatisticsTableSync::Step step);
			typedef std::vector<std::pair<boost::optional<PTOperationStatisticsTableSync::Step>, std::string> > _StepsVector;
			static _StepsVector _GetStepsVector();

			static std::string GetWhatName(PTOperationStatisticsTableSync::What what);
			typedef std::vector<std::pair<boost::optional<PTOperationStatisticsTableSync::What>, std::string> > _WhatVector;
			static _WhatVector _GetWhatVector();


			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Hugues Romain
			///	@date 2011
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Hugues Romain
			///	@date 2011
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param request The current request
			///	@author Hugues Romain
			///	@date 2011
			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Authorization control.
			/// Returns if the page can be displayed. In most cases, the needed right
			/// level is READ.
			///	@param request The current request
			///	@return bool True if the displayed page can be displayed
			///	@author Hugues Romain
			///	@date 2011
			bool isAuthorized(
				const security::User& user
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Builds links to the pages of the current class to put directly under
			/// a module admin page in the pages tree.
			///	@param module The module
			///	@param currentPage Currently displayed page
			/// @param request Current request
			///	@return PageLinks each page to put under the module page in the page
			///	@author Hugues Romain
			///	@date 2011
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const server::ModuleClass& module,
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;
		};
}	}

#endif // SYNTHESE_PTOperationStatisticsAdmin_H__
