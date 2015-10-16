
/** JourneyPattern class header.
	@file JourneyPattern.hpp

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

#ifndef SYNTHESE_ENV_JOURNEY_PATTERN_H
#define SYNTHESE_ENV_JOURNEY_PATTERN_H

#include <vector>
#include <string>

#include <boost/logic/tribool.hpp>

#include "Object.hpp"

#include "Calendar.h"
#include "CommercialLine.h"
#include "DataSourceLinksField.hpp"
#include "Edge.h"
#include "ImportableTemplate.hpp"
#include "Path.h"
#include "Registry.h"

namespace synthese
{
	namespace calendar
	{
		class Calendar;
	}

	namespace geography
	{
		class City;
	}

	namespace graph
	{
		class Service;
	}

	namespace vehicle
	{
		class RollingStock;
	}

	namespace pt
	{
		class TransportNetwork;
		class StopPoint;
		class Destination;
		class LineStop;
		class LineAlarmBroadcast;
		class CommercialLine;

		FIELD_POINTER(JourneyPatternCommercialLine, CommercialLine)
		FIELD_STRING(TimetableName)
		FIELD_STRING(Direction)
		FIELD_POINTER(LineDestination, Destination)
		FIELD_BOOL(IsWalkingLine)
		FIELD_POINTER(LineRollingStock, vehicle::RollingStock)
		FIELD_BOOL(WayBack)
		FIELD_DATASOURCE_LINKS(LineDataSource)
		FIELD_BOOL(Main)
		FIELD_DOUBLE(PlannedLength)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(JourneyPatternCommercialLine),
			FIELD(TimetableName),					//!< Name for timetable
			FIELD(Direction),						//!< Direction (shown on vehicles)
			FIELD(LineDestination),
			FIELD(IsWalkingLine),
			FIELD(LineRollingStock),
			FIELD(BikeComplianceId),
			FIELD(HandicappedComplianceId),
			FIELD(PedestrianComplianceId),
			FIELD(WayBack),							//!< true if back route, false else (forward route or unknown)
			FIELD(LineDataSource),
			FIELD(Main),
			FIELD(PlannedLength)					//!< For DRT
		> JourneyPatternSchema;

		struct cmpLineStop
		{
		    bool operator() (const LineStop* s1, const LineStop* s2) const;
		};

		/** Journey pattern.
			TRIDENT JourneyPattern = Mission
			 inherits from Route
			@ingroup m35

			Une ligne technique est un regroupement de services, constitué dans un but d'amélioration des performances de la recherche d'itinéraires. En effet, plusieurs services groupés dans une ligne ne comptent que pour un dans leur participation au temps de calcul, ce qui rend tout son intérêt au regroupement des services en lignes.
			Le regroupement en lignes techniques s'effectue cependant, par convention, uniquement à  l'intérieur des \ref defLigneCommerciale "lignes commerciales". Ainsi deux services pouvant être groupés selon les critères ci-dessous, mais n'appartenant pas à  la même ligne commerciale, ne seront pas groupés dans une ligne technique. De ce fait, les lignes techniques sont considérés comme des subdivisions de lignes commerciales.

			Cependant, pour assurer la justesse des calculs, les regroupements en ligne doivent respecter des conditions strictes, constituant ainsi la théorie des lignes&nbsp;:
				- Les services d'une même ligne doivent desservir exactement les mêmes points d'arrêt, dans le même ordre
				- Les conditions de desserte de chaque point d'arrêt (départ/passage/arrivée et horaires saisis/non saisis) sont les mêmes pour tous les services d'une ligne
				- Les services d'une même ligne sont assurés par le même matériel roulant, observent les mêmes conditions de réservation, appartiennent au même réseau de transport, et suivent la même tarification
				- Un service d'une ligne ne doit pas desservir un point d'arrêt à la même heure qu'un autre service de la même ligne
				- Un service d'une ligne desservant un point d'arrêt avant un autre de la même ligne ne doit pas desservir un autre point d'arrêt après ce dernier&nbsp;: deux services ne doivent pas se doubler

			NB : la correspondance entre deux services d'une même ligne est interdite, sauf dans les axes libres.
		*/
		class JourneyPattern:
			public Object<JourneyPattern, JourneyPatternSchema>,
			public graph::Path,
			public impex::ImportableTemplate<JourneyPattern>
		{
		public:
			static const std::string ATTR_DIRECTION_TEXT;

			typedef std::set<LineStop*, cmpLineStop> LineStops;

		private:
			mutable boost::optional<calendar::Calendar> _calendar;
			bool _main;

			mutable LineStops _lineStops;

		public:

			JourneyPattern(
				util::RegistryKeyType id = 0,
				std::string name = std::string()
			);

			virtual ~JourneyPattern();



			//! @name Getters
			//@{
				const std::string&	getDirection ()				const;
				const std::string&	getTimetableName ()			const;
				vehicle::RollingStock*	getRollingStock()			const;
                TransportNetwork*	getNetwork()				const;
				bool				getWalkingLine ()			const;
				CommercialLine*		getCommercialLine()			const;
				bool				getWayBack()				const { return get<WayBack>(); }
				Destination*		getDirectionObj()			const { return get<LineDestination>() ? get<LineDestination>().get_ptr() : NULL; }
				graph::MetricOffset	getPlannedLength()			const { return get<PlannedLength>(); }
				virtual std::string getName() const { return get<Name>(); }
				calendar::Calendar& getCalendarCache() const;
				bool getMain() const { return get<Main>(); }
				const LineStops& getLineStops() const { return _lineStops; }
			//@}


			//! @name Setters
			//@{
				void setWalkingLine (bool isWalkingLine);
				void setRollingStock(vehicle::RollingStock*);
				void setNetwork(pt::TransportNetwork*);
				void setTimetableName (const std::string& timetableName);
				void setDirection (const std::string& direction);
				void setCommercialLine(CommercialLine* value);
				void setWayBack(bool value) { set<WayBack>(value); }
				void setDirectionObj(Destination* value){ set<LineDestination>(value ? boost::optional<Destination&>(*value) : boost::none); }
				void setPlannedLength(graph::MetricOffset value){ set<PlannedLength>(value); }
				void setName(const std::string& value){ set<Name>(value); }
				void setMain(bool value){ set<Main>(value); }
				virtual void setRules(const Rules& value);
			//@}



			//! @name Update methods
			//@{
				void addLineStop(const LineStop& lineStop) const;
				void removeLineStop(const LineStop& lineStop) const;
			//@}

			//! @name Services
			//@{
				const StopPoint* getDestination () const;
				const StopPoint* getOrigin () const;

				virtual std::string getRuleUserName() const;

				virtual bool isActive(const boost::gregorian::date& date) const;

				virtual bool isPedestrianMode() const;

				bool isReservable () const;

				void clearCalendarCache() const { _calendar.reset(); }

				virtual void toParametersMap(
					util::ParametersMap& pm,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;

				//////////////////////////////////////////////////////////////////////////
				/// Line stop getter.
				/// @param rank rank of the line stop in the journey pattern
				/// @param ignoreUnscheduledStops ignore the stops without schedules
				/// @return the line stop
				const LineStop* getLineStop(
					std::size_t rank,
					bool ignoreUnscheduledStops = false
				) const;

				size_t getRankInDefinedSchedulesVector(
					size_t rank
				) const;

				size_t getScheduledStopsNumber() const;


				/** Content comparison operator.
					@param stops Array of physical stops
					@return true if the line serves exactly the stops in the same order.
					@warning there is no test on the departure/arrival attributes in this version of the operator
				*/
				bool operator==(const std::vector<pt::StopPoint*>& stops) const;

				struct StopWithDepartureArrivalAuthorization
				{
					typedef std::set<StopPoint*> StopsSet;

					boost::optional<graph::MetricOffset> _metricOffset;
					StopsSet _stop;
					bool _departure;
					bool _arrival;
					boost::optional<bool> _withTimes;
					boost::shared_ptr<geos::geom::LineString> _geometry;

					StopWithDepartureArrivalAuthorization(
						const std::set<StopPoint*>& stop,
						boost::optional<graph::MetricOffset> metricOffset = boost::optional<graph::MetricOffset>(),
						bool departure = true,
						bool arrival = true,
						boost::optional<bool> withTimes = true,
						boost::shared_ptr<geos::geom::LineString> geometry = boost::shared_ptr<geos::geom::LineString>()
					);
				};
				typedef std::vector<StopWithDepartureArrivalAuthorization> StopsWithDepartureArrivalAuthorization;

				//////////////////////////////////////////////////////////////////////////
				/// Full content comparison operator.
				///	@param stops Array of physical stops with arrival/departure information
				/// @return true if the line serves exactly the stops in the same order with the same arrival/departure authorizations
				bool operator==(const StopsWithDepartureArrivalAuthorization& stops) const;

				//////////////////////////////////////////////////////////////////////////
				/// Full content comparison operator.
				///	@param stops Array of physical stops with arrival/departure information
				/// @return true if the line serves exactly the stops in the same order with the same arrival/departure authorizations
				bool operator==(const JourneyPattern& other) const;


				bool compareStopAreas(const StopsWithDepartureArrivalAuthorization& stops) const;

				//////////////////////////////////////////////////////////////////////////
				/// Checks if the journey pattern calls at the specified city
				bool callsAtCity(const geography::City& city) const;

				virtual SubObjects getSubObjects() const;

				virtual LinkedObjectsIds getLinkedObjectsIds(
					const Record& record
				) const;

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif
