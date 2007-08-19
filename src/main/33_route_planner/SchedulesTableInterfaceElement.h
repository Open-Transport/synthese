
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

#include "11_interfaces/LibraryInterfaceElement.h"

#include <vector>
#include <sstream>

namespace synthese
{
	namespace env
	{
		class ConnectionPlace;
		class Journey;
	}

	namespace routeplanner
	{
		/** Timetable generator.
			@code schedules_table @endcode
			@ingroup m53Library refLibrary
		*/
		class SchedulesTableInterfaceElement : public interfaces::LibraryInterfaceElement
		{
		private:
			static const bool _registered;

			struct PlaceInformation
			{
				const env::ConnectionPlace* place;
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
				@param ArretLogique Arrêt à insérer
				@param Position Position minimale à donner à l'arrêt

				L'insertion décale les arrêts suivants une ligne plus bas. Si un trajet piéton (représenté par deux flèches devant être attenantes) se trouve à la position demandée, alors l'arrêt est placé en suivant pour ne pas rompre le cheminement piéton.
			*/
			static int OrdrePAInsere(
				PlaceList&
				, const env::ConnectionPlace*
				, int Position
				, bool isLockedAtTheTop
				, bool isLockedAtTheEnd
			);
			
			/** Contrôle de la compatibilité entre l'ordre des arrêts dans la grille horaire et les arrêts du trajet. */
			static std::vector<bool> OrdrePAConstruitLignesAPermuter(
				const PlaceList&
				, const env::Journey& __TrajetATester
				, int LigneMax
			);
			
			/** Recherche de point d'arrêt dans la liste des points d'arrêt.			*/
			static bool OrdrePARechercheGare(
				const PlaceList&
				, int& i
				, const env::ConnectionPlace* GareAChercher
			);

			static PlaceList getStopsListForScheduleTable( const JourneyBoardJourneys& );

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


