
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

			Les vitesses maximales sont fabriqu�es automatiquement dans le programme d'administration. Lancer la proc�dure et patienter quelques minutes&nbsp;: le fichier <b>.vmax.per</b> est alors �crit. V�rifier � l'int�rieur de ce fichier les d�passements de 200 km/h qui sont tous signal�s par un message ad�quat (<i>Attention, V&gt;200 km/h</i>).

			Corriger �ventuellement les donn�es correspondant � ces d�passements s'ils sont anormaux.

			Les sources d'erreurs les plus fr�quentes sont&nbsp;:
			- Erreur de g�olocalisation d'un point d'arr�t (se voit lorsque beaucoup de vitesses sup�rieures � 200 sont signal�es sur un m�me point d'arr�t)
			- Erreur d'horaires. (voir le fichier <b>log.txt</b> pour d'�ventuelles correspondances avec des d�passements de vitesse de mat�riel, remarqu�s au stade du chargement des lignes)
			- Erreur de code point d'arr�t dans un fichier d'horaires

			Une fois valid� le fichier <b>.vmax.per</b>, lancer l'importation dans l'assistant de saisie des donn�es. Les Vmax sont saisies automatiquement dans le fichier des points d'arret, rempla�ant ainsi les anciens.

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
