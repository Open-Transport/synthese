
//////////////////////////////////////////////////////////////////////////////////////////
/// StopPointsListFunction class header.
/// @file StopPointsListFunction.hpp
/// @author Hugues Romain
/// @date 2010
///
/// This file belongs to the SYNTHESE project (public transportation specialized software)
/// Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
/// This program is free software; you can redistribute it and/or
/// modify it under the terms of the GNU General Public License
/// as published by the Free Software Foundation; either version 2
/// of the License, or (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program; if not, write to the Free Software
/// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_PTPhysicalStopsListFunction_H__
#define SYNTHESE_PTPhysicalStopsListFunction_H__

#include "CommercialLine.h"
#include "UtilTypes.h"
#include "FactorableTemplate.h"
#include "Function.h"

#include <boost/optional.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <geos/geom/Envelope.h>
#include <map>

namespace synthese
{
	class CoordinatesSystem;

	namespace cms
	{
		class Webpage;
	}

	namespace pt_website
	{
		class RollingStockFilter;
	}

	namespace pt
	{
		class StopArea;
		class StopPoint;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Function : Physical Stops Search.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Stops_list
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m35Functions refFunctions
		/// @author Hugues Romain, Xavier Raffin
		/// @date 2010
		/// @since 3.1.18
		class StopPointsListFunction:
			public util::FactorableTemplate<server::Function,StopPointsListFunction>
		{
		public:
			static const std::string PARAMETER_DATE;
			static const std::string PARAMETER_LINE_ID;
			static const std::string PARAMETER_PAGE_ID;
			static const std::string PARAMETER_DESTINATION_PAGE_ID;
			static const std::string PARAMETER_LINE_PAGE_ID;
			static const std::string PARAMETER_BBOX;
			static const std::string PARAMETER_SRID;
			static const std::string PARAMETER_ROLLING_STOCK_FILTER_ID;
			static const std::string PARAMETER_SORT_BY_LINE_NAME;
			static const std::string PARAMETER_OMIT_SAME_AREA_DESTINATIONS;
			static const std::string PARAMETER_SORT_BY_DISTANCE_TO_BBOX_CENTER;
			static const std::string PARAMETER_MAX_SOLUTIONS_NUMBER; 
			static const std::string PARAMETER_DATA_SOURCE_FILTER;

		protected:
			static const std::string TAG_PHYSICAL_STOP;
			static const std::string TAG_PHYSICAL_STOPS;
			static const std::string TAG_DESTINATION;
			static const std::string TAG_LINE;
			static const std::string DATA_STOP_AREA_PREFIX;

			static const std::string DATA_STOPAREA_NAME;
			static const std::string DATA_STOPAREA_CITY_NAME;
			static const std::string DATA_DESTINATIONS;
			static const std::string DATA_DESTINATIONS_NUMBER;
			static const std::string DATA_LINES;
			static const std::string DATA_DISTANCE_TO_BBOX_CENTER;

			//! \name Page parameters
			//@{
				boost::optional<boost::posix_time::ptime> _date;
				boost::optional<boost::shared_ptr<const pt::StopArea> > _stopArea;
				boost::optional<util::RegistryKeyType> _commercialLineID;
				boost::shared_ptr<const cms::Webpage> _page;
				boost::shared_ptr<const cms::Webpage> _destinationPage;
				boost::shared_ptr<const cms::Webpage> _linePage;
				boost::optional<geos::geom::Envelope> _bbox;
				const CoordinatesSystem* _coordinatesSystem;
				boost::shared_ptr<const pt_website::RollingStockFilter> _rollingStockFilter;
				bool _sortByLineName;
				bool _omitSameAreaDestinations;
				boost::optional<std::size_t> _maxSolutionsNumber;
				boost::shared_ptr<const impex::DataSource> _dataSourceFilter;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Stops_list#Request
			//////////////////////////////////////////////////////////////////////////
			/// @return Generated parameters map
			/// @author Xavier Raffin
			/// @date 2010
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Stops_list#Request
			//////////////////////////////////////////////////////////////////////////
			/// @param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);



		public:
			//! @name Setters
			//@{
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Stops_list#Response
			//////////////////////////////////////////////////////////////////////////
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Xavier Raffin
			/// @date 2010
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues Romain
			/// @date 2010
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2010
			virtual std::string getOutputMimeType() const;

			virtual server::FunctionAPI getAPI() const;

		private:
			bool _isSortByDistanceToBboxCenter;

			class SortableStopPoint
			{
			private:
				const StopPoint * _sp;
				std::string _opCode;
				int _distanceToBboxCenter;
				bool _isSortByDistanceToBboxCenter;
			public:
				SortableStopPoint(const StopPoint * sp, int distanceToBboxCenter, bool isSortByDistanceToBboxCenter);
				bool operator<(SortableStopPoint const &otherStopPoint) const;
				const StopPoint * getStopPoint() const;
				int getDistanceToBboxCenter() const;
				std::string getOpCode() const;
			};

			class SortableStopArea
			{
			private:
				util::RegistryKeyType _key;
				std::string _destinationName;
			public:
				SortableStopArea(util::RegistryKeyType key, std::string destinationName);
				bool operator<(SortableStopArea const &otherStopArea) const;
				std::string getDestinationName() const;
				util::RegistryKeyType getKey() const;
			};

			//////////////////////////////////////////////////////////////////////////
			/// StopPoints are stored in a map as key
			/// Value is another map containing StopArea Destination deserved from this StopPoint
			/// For each destination, there is a map containing all lines
			/// (several lines could reach the same destination from the same stopPoint)
			typedef std::set<const CommercialLine *, CommercialLine::PointerComparator> CommercialLineSetType;
			typedef std::map<const SortableStopArea, std::pair<const StopArea *, CommercialLineSetType > > StopAreaDestinationMapType;
			typedef std::map<const SortableStopPoint, StopAreaDestinationMapType > StopPointMapType;

			//////////////////////////////////////////////////////////////////////////
			/// Add the StopPoint to the stopPoint map
			/// @author Xavier Raffin
			/// @date 2011
			void addStop(
				StopPointMapType & stopPointMap,
				const StopPoint & sp,
				boost::posix_time::ptime & startDateTime,
				boost::posix_time::ptime & endDateTime
			) const;

			void _displayLines(
				std::ostream& stream,
				const std::vector<boost::shared_ptr<util::ParametersMap> >& lines,
				const std::string destinationName,
				const std::string destinationCityName,
				const server::Request& request
			) const;

			void _displayDestinations(
				std::ostream& stream,
				const std::vector<boost::shared_ptr<util::ParametersMap> >& destinations,
				const server::Request& request
			) const;

			int CalcDistanceToBboxCenter (const StopPoint & stopPoint) const;
		};
	}
}

#endif // SYNTHESE_PTPhysicalStopsListFunction_H__
