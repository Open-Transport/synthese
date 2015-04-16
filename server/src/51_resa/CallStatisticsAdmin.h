

//////////////////////////////////////////////////////////////////////////
/// CallStatisticsAdmin class header.
///	@file CallStatisticsAdmin.h
///	@author Hugues
///	@date 2009
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

#ifndef SYNTHESE_CallStatisticsAdmin_H__
#define SYNTHESE_CallStatisticsAdmin_H__

#include "ResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"
#include "CallStatisticsTableSync.h"

#include <vector>
#include <utility>
#include <boost/optional.hpp>

namespace synthese
{
	namespace resa
	{
		//////////////////////////////////////////////////////////////////////////
		/// CallStatisticsAdmin Admin compound class.
		///	@ingroup m51Admin refAdmin
		///	@author Hugues
		///	@date 2009
		class CallStatisticsAdmin:
			public admin::AdminInterfaceElementTemplate<CallStatisticsAdmin>
		{
		public:
			/// @name Parameter identifiers
			//@{
				static const std::string PARAM_SEARCH_START_DATE;
				static const std::string PARAM_SEARCH_END_DATE;
				static const std::string PARAM_SEARCH_STEP;
			//@}

		private:

			/// @name Search parameters
			//@{
				boost::gregorian::date_period _searchPeriod;
				CallStatisticsTableSync::Step _searchStep;
			//@}

				typedef std::vector<std::pair<boost::optional<CallStatisticsTableSync::Step>, std::string> > _StepsVector;
				static _StepsVector _GetStepsVector();

		protected:
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author Hugues
			///	@date 2009
			CallStatisticsAdmin();



			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Hugues
			///	@date 2009
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Hugues
			///	@date 2009
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param request The current request
			///	@author Hugues
			///	@date 2009
			void display(
				std::ostream& stream,
				const server::Request& request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Authorization control.
			/// Returns if the page can be displayed. In most cases, the needed right
			/// level is READ.
			///	@param request The current request
			///	@return bool True if the displayed page can be displayed
			///	@author Hugues
			///	@date 2009
			bool isAuthorized(
				const security::User& profile
			) const;
		};
	}
}

#endif // SYNTHESE_CallStatisticsAdmin_H__
