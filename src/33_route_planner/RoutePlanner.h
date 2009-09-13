
/** RoutePlanner class header.
	@file RoutePlanner.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#ifndef SYNTHESE_ROUTEPLANNER_ROUTEPLANNER_H
#define SYNTHESE_ROUTEPLANNER_ROUTEPLANNER_H

#include <deque>
#include <vector>
#include <map>

#include "Log.h"

#include "DateTime.h"

#include "Place.h"
#include "VertexAccessMap.h"
#include "ServiceUse.h"
#include "AccessParameters.h"

#include "33_route_planner/Types.h"

#include <boost/optional.hpp>

namespace synthese
{

	namespace time
	{
		class DateTime;
	}

	namespace graph
	{
		class Edge;
		class Path;
		class Vertex;
		class Service;
		class SquareDistance;
		class ServicePointer;
		class Journey;
	}

	namespace routeplanner
	{
		typedef enum {DEPARTURE_FIRST, ARRIVAL_FIRST} PlanningOrder;

		/** Route planner class.

			Potential optimization by maximal speeds

			Les vitesses maximales interviennent dans l'optimisaion.

			Les vitesses maximales sont fabriquées automatiquement dans le programme d'administration. Lancer la procédure et patienter quelques minutes&nbsp;: le fichier <b>.vmax.per</b> est alors écrit. Vérifier à l'intérieur de ce fichier les dépassements de 200 km/h qui sont tous signalés par un message adéquat (<i>Attention, V&gt;200 km/h</i>).

			Corriger éventuellement les données correspondant à ces dépassements s'ils sont anormaux.

			Les sources d'erreurs les plus fréquentes sont&nbsp;:
			- Erreur de géolocalisation d'un point d'arrêt (se voit lorsque beaucoup de vitesses supérieures à 200 sont signalées sur un même point d'arrêt)
			- Erreur d'horaires. (voir le fichier <b>log.txt</b> pour d'éventuelles correspondances avec des dépassements de vitesse de matériel, remarqués au stade du chargement des lignes)
			- Erreur de code point d'arrêt dans un fichier d'horaires

			Une fois validé le fichier <b>.vmax.per</b>, lancer l'importation dans l'assistant de saisie des données. Les Vmax sont saisies automatiquement dans le fichier des points d'arret, remplaçant ainsi les anciens.

			@ingroup m53
		*/
		class RoutePlanner
		{
		public:
			class Result
			{
			public:
				bool samePlaces;
				JourneyBoardJourneys journeys;

				void clear();
			};

		private:
			
			//! @name Parameters
			//@{
				graph::VertexAccessMap		_originVam;
				graph::VertexAccessMap		_destinationVam;
				const graph::AccessParameters	_accessParameters;
				const time::DateTime		_journeySheetStartTime;  //!< Start time of schedule sheet.
				const time::DateTime		_journeySheetEndTime;    //!< End time of schedule sheet.
				const PlanningOrder			_planningOrder;  //!< Define planning sequence.
				const boost::optional<std::size_t>	_maxSolutionsNumber;
			//@}

			//! @name Working variables
			//@{
				time::DateTime				_minDepartureTime;  //!< Min departure time.
				time::DateTime				_maxArrivalTime;  //!< Max arrival time.
				boost::posix_time::time_duration _previousContinuousServiceDuration;  //!< Journey duration in previously found continuous service.
				time::DateTime				_previousContinuousServiceLastDeparture;  //!< End time of validity range of previously found continuous service.
				Result						_result;
				std::ostream* const			_logStream;
				const util::Log::Level		_logLevel;
				//!< 
			//@}

				


			/** Best journey finder.
				@param accessDirection Type of computing : search of better arrival or of a better departure
				@param ovam Vertex access map containing each departure physical stops.
				@param dvam Vertex access map containing each destination stops.
				@param currentJourney Journey conducting to the departure vam
				@param sctrictTime Filter : 
					- true : solutions starting at the vam time are only selected
					- false : all solutions are selected
				@param optim Filter : 
					- true : solutions allowing a comfort raising and a time saving are selected
					- false :solutions allowing a time saving are only selected
			*/
			void findBestJourney(
				graph::AccessDirection accessDirection,
				graph::Journey& result
				, const graph::VertexAccessMap& startVam
				, const graph::VertexAccessMap& endVam
				, const time::DateTime& startTime
				, bool strictTime
				, bool inverted
			);

			void computeRoutePlanningDepartureArrival(
				graph::Journey& result
				, const graph::VertexAccessMap& ovam
				, const graph::VertexAccessMap& dvam
			);

		public:
			/** Constructor.
				@param origin Origin place
				@param destination Destination place
				@param accessParameters Access Parameters
				@param planningOrder Planning order
				@param journeySheetStartTime Start date time
				@param journeySheetEndTime End date time
				@param maxSolutionsNumber Maximum of solutions number
				@author Hugues Romain
				@date 2007
			*/
			RoutePlanner(
				 const geography::Place* origin,
				 const geography::Place* destination,
				 const graph::AccessParameters& accessParameters,
				 const PlanningOrder& planningOrder,
				 const time::DateTime& journeySheetStartTime,
				 const time::DateTime& journeySheetEndTime,
				 const boost::optional<std::size_t> maxSolutionsNumber = boost::optional<std::size_t>(),
				 std::ostream* logStream = NULL,
				 util::Log::Level logLevel = util::Log::LEVEL_NONE
			);


			//! @name Query methods
			//@{
				/** Launch of the route planning, applying the "from the departure to the arrival" method.
					@return JourneyBoardJourneys The founded journeys
					@warning The journeys must be deleted after use to avoid memory leak
					@author Hugues Romain
					@date 2007
				*/
				const Result& computeJourneySheetDepartureArrival ();
			//@}
		};
	}
}
#endif
