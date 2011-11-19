
//////////////////////////////////////////////////////////////////////////////////////////
/// CSVResaStatisticsFunction class header.
///	@file CSVResaStatisticsFunction.h
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

#ifndef SYNTHESE_CSVResaStatisticsFunction_H__
#define SYNTHESE_CSVResaStatisticsFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"
#include "ResaStatisticsTableSync.h"

#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace pt
	{
		class CommercialLine;
	}

	namespace resa
	{
		/** CSVResaStatisticsFunction Function class.
			@author Hugues Romain
			@date 2009
			@ingroup m31Functions refFunctions
		*/
		class CSVResaStatisticsFunction:
			public util::FactorableTemplate<server::Function,CSVResaStatisticsFunction>
		{
		public:
			/// @todo request parameter names declaration
			static const std::string PARAM_LINE_ID;
			static const std::string PARAM_SEARCH_START_DATE;
			static const std::string PARAM_SEARCH_END_DATE;
			static const std::string PARAM_ROW_STEP;
			static const std::string PARAM_COL_STEP;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const pt::CommercialLine>	_line;
				boost::gregorian::date_period _searchPeriod;
				ResaStatisticsTableSync::Step _searchRowStep;
				ResaStatisticsTableSync::Step _searchColStep;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues
			/// @date 2009
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues
			/// @date 2009
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


		public:
			void setLine(boost::shared_ptr<const pt::CommercialLine> value);
			void setPeriod(boost::gregorian::date_period value);
			void setRowStep(ResaStatisticsTableSync::Step value);
			void setColStep(ResaStatisticsTableSync::Step value);


			CSVResaStatisticsFunction();


			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @author Hugues
			/// @date 2009
			virtual void run(std::ostream& stream, const server::Request& request) const;



			virtual std::string getFileName() const;


			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @return true if the function can be run
			/// @author Hugues
			/// @date 2009
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues
			/// @date 2009
			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_CSVResaStatisticsFunction_H__
