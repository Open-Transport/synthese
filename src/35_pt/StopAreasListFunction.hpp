////////////////////////////////////////////////////////////////////////////////
/// StopAreasListFunction class header.
///	@file StopAreasListFunction.hpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_StopAreasListFunction_H__
#define SYNTHESE_StopAreasListFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include <boost/shared_ptr.hpp>
#include <geos/geom/Envelope.h>

namespace synthese
{
	class CoordinatesSystem;

	namespace geography
	{
		class City;
	}

	namespace cms
	{
		class Webpage;
	}

	namespace pt
	{
		class CommercialLine;
		class StopArea;

		////////////////////////////////////////////////////////////////////
		/// 35.15 Function : Stop Area list public Function class.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Stop_areas_list
		//////////////////////////////////////////////////////////////////////////
		/// @author Xavier Raffin, Hugues Romain
		///	@date 2008
		///	@ingroup m35Functions refFunctions
		class StopAreasListFunction:
			public util::FactorableTemplate<server::Function,StopAreasListFunction>
		{
		public:
			static const std::string PARAMETER_BBOX;
			static const std::string PARAMETER_SRID;
			static const std::string PARAMETER_OUTPUT_LINES;
			static const std::string PARAMETER_STOP_PAGE_ID;
			static const std::string PARAMETER_LINE_PAGE_ID;
			static const std::string PARAMETER_TERMINUS_ID;
			static const std::string PARAMETER_OUTPUT_STOPS;
			static const std::string PARAMETER_OUTPUT_LINES_IN_STOPS;
			static const std::string PARAMETER_GROUP_BY_CITIES;
			static const std::string PARAMETER_STOPS_DIRECTIONS;

		private:
			static const std::string TAG_CITY;
			static const std::string TAG_STOP;
			static const std::string TAG_DIRECTION;
			static const std::string DATA_LINE;
			static const std::string DATA_LINES;
			static const std::string DATA_STOP_RANK;
			static const std::string DATA_STOP_AREA;
			static const std::string DATA_STOP_AREAS;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const pt::CommercialLine> _commercialLine;
				boost::shared_ptr<const geography::City> _city;
				boost::shared_ptr<const StopArea> _stopAreaFilter;
				boost::optional<geos::geom::Envelope> _bbox;
				const CoordinatesSystem* _coordinatesSystem;
				bool _outputLines;
				boost::shared_ptr<const cms::Webpage> _stopPage;
				boost::shared_ptr<const cms::Webpage> _linePage;
				std::string _outputFormat;
				boost::optional<util::RegistryKeyType> _terminusId;
				bool _outputStops;
				bool _outputLinesInStops;
				bool _groupByCities;
				size_t _stopsDirections;
			//@}

			//! @name Setters
			//@{
				void setCity(boost::shared_ptr<const geography::City> value){ _city = value; }
				void setStopPage(boost::shared_ptr<const cms::Webpage> value){ _stopPage = value; }
				void setLinePage(boost::shared_ptr<const cms::Webpage> value){ _linePage = value; }
				void setOutputFormat(const std::string& value){ _outputFormat = value; }
			//@}


			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			util::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			StopAreasListFunction();

			/** Action to run, defined by each subclass.
			*/
			util::ParametersMap run(std::ostream& stream, const server::Request& request) const;

			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_StopAreasListFunction_H__
