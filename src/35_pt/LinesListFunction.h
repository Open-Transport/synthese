////////////////////////////////////////////////////////////////////////////////
/// LinesListFunction class header.
///	@file LinesListFunction.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_LinesListFunction_H__
#define SYNTHESE_LinesListFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	class CoordinatesSystem;

	namespace cms
	{
		class Webpage;
	}

	namespace pt
	{
		class CommercialLine;
		class TransportNetwork;

		////////////////////////////////////////////////////////////////////
		/// 35.15 Function : Lines list public Function class.
		/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Lines_list
		//////////////////////////////////////////////////////////////////////////
		/// @author Hugues Romain
		///	@date 2008 2011
		///	@ingroup m35Functions refFunctions
		class LinesListFunction:
			public util::FactorableTemplate<server::Function,LinesListFunction>
		{
		 public:
			static const std::string PARAMETER_OUTPUT_FORMAT;
			static const std::string PARAMETER_NETWORK_ID;
			static const std::string PARAMETER_PAGE_ID;
			static const std::string PARAMETER_SRID;
			static const std::string PARAMETER_OUTPUT_STOPS;
			static const std::string PARAMETER_OUTPUT_GEOMETRY;
			static const std::string PARAMETER_IGNORE_TIMETABLE_EXCLUDED_LINES;
			static const std::string PARAMETER_IGNORE_JOURNEY_PLANNER_EXCLUDED_LINES;
			static const std::string PARAMETER_IGNORE_DEPARTURES_BOARD_EXCLUDED_LINES;

			static const std::string FORMAT_XML;
			static const std::string FORMAT_JSON;
			static const std::string FORMAT_WKT;

			static const std::string DATA_LINE;
			static const std::string DATA_LINES;
			static const std::string DATA_STOP_AREAS;
			static const std::string DATA_STOP_AREA;
			static const std::string DATA_TRANSPORT_MODE;
			static const std::string DATA_GEOMETRY;
			static const std::string DATA_WKT;
			static const std::string DATA_EDGE;
			static const std::string DATA_POINT;
			static const std::string DATA_X;
			static const std::string DATA_Y;
			static const std::string DATA_RANK;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const pt::TransportNetwork> _network;
				boost::shared_ptr<const pt::CommercialLine> _line;
				boost::shared_ptr<const cms::Webpage> _page;
				const CoordinatesSystem* _coordinatesSystem;
				std::string _outputFormat;
				bool _outputStops;
				bool _ignoreTimetableExcludedLines;
				bool _ignoreJourneyPlannerExcludedLines;
				bool _ignoreDeparturesBoardExcludedLines;
				std::string _outputGeometry;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// https://extranet-rcsmobility.com/projects/synthese/wiki/Lines_list#Request
			//////////////////////////////////////////////////////////////////////////
			/// @return Generated parameters map
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// https://extranet-rcsmobility.com/projects/synthese/wiki/Lines_list#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to read
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			LinesListFunction();

			//////////////////////////////////////////////////////////////////////////
			/// Output.
			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Lines_list#Response
			//////////////////////////////////////////////////////////////////////////
			void run(std::ostream& stream, const server::Request& request) const;


			//! @name Setters
			//@{
				void setNetwork(boost::shared_ptr<const TransportNetwork> value){ _network = value; }
			//@}

			virtual bool isAuthorized(const server::Session* session) const;


			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_LinesListFunction_H__
