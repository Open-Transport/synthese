
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

#include "VertexAccessMap.h"
#include "ServiceUse.h"
#include "AccessParameters.h"
#include "RoutePlannerTypes.h"
#include "Exception.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace graph
	{
		class Edge;
		class Path;
		class Vertex;
		class Service;
		class ServicePointer;
		class Journey;
	}

	namespace geography
	{
		class Place;
	}

	namespace algorithm
	{
		class JourneyTemplates;

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

			@ingroup m33
		*/
		class RoutePlanner
		{
		public:
			typedef graph::Journey Result;
			
			/** Exception to throw when the route planning has no sense because of the presence of a common vertex in
				both the the departure and the arrival vertex access maps.
			*/
			class SamePlacesException:
				public util::Exception
			{
			public:
				SamePlacesException();
			};

		private:
			//! @name Query parameters
			//@{
				const graph::VertexAccessMap&		_originVam;
				const graph::VertexAccessMap&		_destinationVam;
				const PlanningOrder					_planningOrder;  //!< Define planning sequence.
				const graph::AccessParameters		_accessParameters;
				const boost::optional<boost::posix_time::time_duration>	_maxDuration;
				const boost::posix_time::ptime&				_minBeginTime;
				const boost::posix_time::ptime&				_maxBeginTime;
				const boost::posix_time::ptime&				_maxEndTime;
				const graph::GraphIdType			_whatToSearch;
				const graph::GraphIdType			_graphToUse;
				boost::optional<const JourneyTemplates&> _journeyTemplates;
			//@}

			//! @name Logging
			//@{
				std::ostream* const					_logStream;
			//@}
				

			//! @name Intermediate values
			//@{
				const int _totalDistance;
			//@}

			/** Best journey finder.
				@param accessDirection Type of computing : search of better arrival or of a better departure
				@param currentJourney Journey conducting to the departure vam
				@param sctrictTime Filter : 
					- true : solutions starting at the vam time are only selected
					- false : all solutions are selected
				@param optim Filter : 
					- true : solutions allowing a comfort raising and a time saving are selected
					- false :solutions allowing a time saving are only selected
			*/
			void _findBestJourney(
				graph::Journey& result,
				const graph::VertexAccessMap& originVam,
				const graph::VertexAccessMap& destinationVam,
				graph::AccessDirection direction,
				const boost::posix_time::ptime& originDateTime,
				const boost::posix_time::ptime& maxMinDateTimeAtOrigin,
				const boost::posix_time::ptime& maxMinDateTimeAtDestination,
				bool secondTime,
				boost::optional<boost::posix_time::time_duration> maxDuration
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
				const graph::VertexAccessMap&		originVam,
				const graph::VertexAccessMap&		destinationVam,
				PlanningOrder					planningOrder,  //!< Define planning sequence.
				graph::AccessParameters		accessParameters,
				boost::optional<boost::posix_time::time_duration>	maxDuration,
				const boost::posix_time::ptime&				minBeginTime,
				const boost::posix_time::ptime&				maxBeginTime,
				const boost::posix_time::ptime&				maxEndTime,
				graph::GraphIdType			whatToSearch,
				graph::GraphIdType			graphToUse,
				std::ostream* logStream = NULL,
				boost::optional<const JourneyTemplates&> journeyTemplates = boost::optional<const JourneyTemplates&>()
			);

			
			
			/** Launches the computing ans return the result.
				@return The result (Journey object)
					- non empty if the graph allows to reach the destination from the origin according to the access parameters
					- empty if no solution has been found
				@throws SamePlacesException if the departure and the arrival places intersects.
				@author Hugues
				@date 2009				
			*/
			Result run();
		};
	}
}
#endif
