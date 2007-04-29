
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

#include "33_route_planner/Journey.h"

#include "11_interfaces/LibraryInterfaceElement.h"

#include <vector>

namespace synthese
{
	namespace env
	{
		class ConnectionPlace;
	}
	namespace routeplanner
	{
		/** Timetable generator.
			@code schedules_table @endcode
			@ingroup m33Library refLibrary
		*/
		class SchedulesTableInterfaceElement : public interfaces::LibraryInterfaceElement
		{
		private:
			static const bool _registered;

			typedef std::vector<const synthese::env::ConnectionPlace*> PlaceList;
			typedef std::vector<bool> LockedLinesList;

			static size_t OrdrePAEchangeSiPossible( const 
				Journeys&, PlaceList&, const LockedLinesList&, size_t PositionOrigine, size_t PositionSouhaitee );
			
			/** Insertion d'un arr�t de passage dans la liste des arr�ts d'une fiche horaire.
				@param ArretLogique Arr�t � ins�rer
				@param Position Position minimale � donner � l'arr�t

				L'insertion d�cale les arr�ts suivants une ligne plus bas. Si un trajet pi�ton (repr�sent� par deux fl�ches devant �tre attenantes) se trouve � la position demand�e, alors l'arr�t est plac� en suivant pour ne pas rompre le cheminement pi�ton.
			*/
			static size_t OrdrePAInsere(PlaceList&, const LockedLinesList&, const synthese::env::ConnectionPlace*, size_t Position );
			
			/** Contr�le de la compatibilit� entre l'ordre des arr�ts dans la grille horaire et les arr�ts du trajet. */
			static std::vector<bool> OrdrePAConstruitLignesAPermuter( const PlaceList&, const Journey& __TrajetATester, size_t LigneMax );
			
			/** Recherche de point d'arr�t dans la liste des points d'arr�t.			*/
			static bool OrdrePARechercheGare( const PlaceList&, size_t& i, const synthese::env::ConnectionPlace* GareAChercher );

			static PlaceList getStopsListForScheduleTable( const synthese::routeplanner::Journeys& );

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
			void parse( const std::string& text);
		};

	}
}


