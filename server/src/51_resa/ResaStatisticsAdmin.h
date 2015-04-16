

//////////////////////////////////////////////////////////////////////////
/// ResaStatisticsAdmin class header.
///	@file ResaStatisticsAdmin.h
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

#ifndef SYNTHESE_ResaStatisticsAdmin_H__
#define SYNTHESE_ResaStatisticsAdmin_H__

#include "ResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"
#include "ResaStatisticsTableSync.h"

#include <vector>
#include <utility>
#include <boost/optional.hpp>

namespace synthese
{
	namespace pt
	{
		class CommercialLine;
	}

	namespace resa
	{
		//////////////////////////////////////////////////////////////////////////
		/// 31.14 Admin : Reservation and calls statistics viewer.
		///	@ingroup m51Admin refAdmin
		///	@author Hugues Romain
		///	@date 2009
		class ResaStatisticsAdmin:
			public admin::AdminInterfaceElementTemplate<ResaStatisticsAdmin>
		{
		public:
			/// @name Parameter identifiers
			//@{
				static const std::string PARAM_LINE_ID;
				static const std::string PARAM_SEARCH_START_DATE;
				static const std::string PARAM_SEARCH_END_DATE;
				static const std::string PARAM_ROW_STEP;
				static const std::string PARAM_COL_STEP;
			//@}

		private:
			/// @name Search parameters
			//@{
				boost::shared_ptr<const pt::CommercialLine>	_line;
				boost::gregorian::date_period _searchPeriod;
				ResaStatisticsTableSync::Step _searchRowStep;
				ResaStatisticsTableSync::Step _searchColStep;
			//@}


		protected:

		public:
			static std::string GetColumnName(ResaStatisticsTableSync::Step step);
			typedef std::vector<std::pair<boost::optional<ResaStatisticsTableSync::Step>, std::string> > _StepsVector;
			static _StepsVector _GetStepsVector();

			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author Hugues
			///	@date 2009
			ResaStatisticsAdmin();


			void setCommercialLine(boost::shared_ptr<pt::CommercialLine> value);
			void setCommercialLineC(boost::shared_ptr<const pt::CommercialLine> value);
			boost::shared_ptr<const pt::CommercialLine> getCommercialLine() const;


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
			///	@param profile Profile of the current user
			///	@return bool True if the displayed page can be displayed
			///	@author Hugues
			///	@date 2009
			bool isAuthorized(
				const security::User& user
			) const;


			/** Title generator.
				@return The title of the page
				@author Hugues Romain
				@date 2009
			*/
			virtual std::string getTitle() const;

			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;
		};
	}
}

#endif // SYNTHESE_ResaStatisticsAdmin_H__
