
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

#include "Path.h"
#include "ImportableTemplate.hpp"
#include "Registry.h"
#include "Calendar.h"
#include "Edge.h"

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
		class JourneyPatternCopy;
		class Destination;
		class LineStop;
		class LineAlarmBroadcast;
		class CommercialLine;


		/** Technical line.
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

			If a service is responsible of a break of the preceding rules, then the line is copied as a JourneyPatternCopy, and the service is linked to the new line. The _sublines container keeps a pointer on each JourneyPatternCopy.

			@warning If a new attribute is added on the JourneyPattern class, don't forget to update the constructor of JourneyPatternCopy.
		*/
		class JourneyPattern:
			public graph::Path,
			public impex::ImportableTemplate<JourneyPattern>
		{
		public:
			static const std::string ATTR_DIRECTION_TEXT;

			/// Chosen registry class.
			typedef util::Registry<JourneyPattern>	Registry;

			typedef std::vector<pt::JourneyPatternCopy*> SubLines;

		private:
			// If a new attribute is added on the JourneyPattern class, don't forget to update the constructor of JourneyPatternCopy.

			std::string _timetableName; //!< Name for timetable
			std::string _direction;		//!< Direction (shown on vehicles)
			Destination* _directionObj;
			std::string _name;
			bool _isWalkingLine;
			mutable boost::optional<calendar::Calendar> _calendar;

			SubLines	_subLines;	//!< Copied lines handling services which not serve the line theory

			bool _wayBack;	//!< true if back route, false else (forward route or unknown)
			bool _main; //!< can be considered as a main route of the line

			graph::MetricOffset _plannedLength; //!< For DRT

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
				const SubLines		getSubLines()				const;
				bool				getWayBack()				const { return _wayBack; }
				Destination*		getDirectionObj()			const { return _directionObj; }
				bool				getMain()					const { return _main; }
				graph::MetricOffset	getPlannedLength()			const { return _plannedLength; }
				virtual std::string getName() const { return _name; }
				calendar::Calendar& getCalendarCache() const;
			//@}


			//! @name Setters
			//@{
				void setWalkingLine (bool isWalkingLine);
				void setRollingStock(vehicle::RollingStock*);
				void setNetwork(pt::TransportNetwork*);
				void setTimetableName (const std::string& timetableName);
				void setDirection (const std::string& direction);
				void setCommercialLine(CommercialLine* value);
				void setWayBack(bool value) { _wayBack = value; }
				void setDirectionObj(Destination* value){ _directionObj = value; }
				void setMain(bool value){ _main = value; }
				void setPlannedLength(graph::MetricOffset value){ _plannedLength = value; }
				void setName(const std::string& value){ _name = value; }
			//@}



			//! @name Update methods
			//@{

				/** Adds a sub-line to the line.
					@param line sub-line to add
					@return int rank of the sub-line in the array of sub-lines
					@author Hugues Romain
					@date 2008
				*/
				int addSubLine(pt::JourneyPatternCopy* line);


				/** Adds a service to a line.
					@param service Service to add
					@param ensureLineTheory If true, the method verifies if the service is compatible
							with the other ones, by the way of the lines theory. If not, then it attempts
							to register the service in an existing JourneyPatternCopy, or creates one if necessary.
							Note : in this case, the service is NOT added to the current line.

					@author Hugues Romain
					@date 2007
				*/
				virtual void addService(
					graph::Service& service,
					bool ensureLineTheory
				);



				virtual bool loadFromRecord(
					const Record& record,
					util::Env& env
				);
			//@}

			//! @name Services
			//@{
				virtual std::string getRuleUserName() const;

				virtual bool isActive(const boost::gregorian::date& date) const;

				virtual bool isPedestrianMode() const;

				bool isReservable () const;

				void clearCalendarCache() const { _calendar.reset(); }

				const pt::StopPoint* getDestination () const;
				const pt::StopPoint* getOrigin () const;

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

				std::size_t getScheduledStopsNumber() const;

				/** Tests if the line theory would be respected if the service were inserted into the line.
					@param service service to test
					@return bool true if the line theory would be respected
					@author Hugues Romain
					@date 2008
				*/
				bool respectsLineTheory(
					bool RTData,
					const graph::Service& service
				) const;

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
		};
	}
}

#endif
