
/** SchedulesTableInterfaceElement class header.
	@file SchedulesTableInterfaceElement.h

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

#include "33_route_planner/Types.h"

#include "LibraryInterfaceElement.h"
#include "FactorableTemplate.h"

#include <vector>
#include <sstream>

namespace synthese
{
	namespace graph
	{
		class Journey;
	}
	
	namespace env
	{
		class Place;
	}
	
	namespace routeplanner
	{
		/** Timetable generator.
			@code schedules_table @endcode
			@ingroup m53Library refLibrary
		*/
		class SchedulesTableInterfaceElement
		:	public util::FactorableTemplate<interfaces::LibraryInterfaceElement,SchedulesTableInterfaceElement>
		{
		private:
			static const bool _registered;

			struct PlaceInformation
			{
				const env::Place* place;
				bool isOrigin;
				bool isDestination;
				std::ostringstream* content;
			};
			typedef std::vector<PlaceInformation> PlaceList;
			
			static int OrdrePAEchangeSiPossible(
				const JourneyBoardJourneys&
				, PlaceList&
				, int PositionOrigine
				, int PositionSouhaitee
				);
			
			/** Insertion d'un arrêt de passage dans la liste des arrêts d'une fiche horaire.
				@param ArretLogique Arrêt �  insérer
				@param Position Position minimale �  donner �  l'arrêt

				L'insertion décale les arrêts suivants une ligne plus bas. Si un trajet piéton (représenté par deux fl�šches devant être attenantes) se trouve �  la position demandée, alors l'arrêt est placé en suivant pour ne pas rompre le cheminement piéton.
			*/
			static int OrdrePAInsere(
				PlaceList&
				, const env::Place*
				, int Position
				, bool isLockedAtTheTop
				, bool isLockedAtTheEnd
			);
			
			/** Contr��le de la compatibilité entre l'ordre des arrêts dans la grille horaire et les arrêts du trajet. */
			static std::vector<bool> OrdrePAConstruitLignesAPermuter(
				const PlaceList&
				, const graph::Journey& __TrajetATester
				, int LigneMax
			);
			
			/** Recherche de point d'arrêt dans la liste des points d'arrêt.			*/
			static bool OrdrePARechercheGare(
				const PlaceList&
				, int& i
				, const env::Place* GareAChercher
			);

			static PlaceList getStopsListForScheduleTable(
				const JourneyBoardJourneys&
				, const env::Place* departurePlace
				, const env::Place* arrivalPlace
			);

		public:
			/** Display.
				@param object cTrajets * : List of journeys
			*/
			std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request= NULL) const;

			/** Parser.
				@param vel list of parameters :
			*/
			void storeParameters(interfaces::ValueElementList& vel);
		};
	}
}
