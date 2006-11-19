

#include "11_interfaces/LibraryInterfaceElement.h"
#include "33_route_planner/Journey.h"
#include <vector>

namespace synthese
{
	namespace env
	{
		class ConnectionPlace;
	}
	namespace interfaces
	{
		/** Timetable generator.
			@code schedules_table @endcode
		*/
		class SchedulesTableInterfaceElement : public LibraryInterfaceElement
		{
		private:
			static const bool _registered;

			typedef std::vector<const synthese::env::ConnectionPlace*> PlaceList;
			typedef std::vector<bool> LockedLinesList;

			static size_t OrdrePAEchangeSiPossible( const synthese::routeplanner::JourneyVector&, PlaceList&, const LockedLinesList&, size_t PositionOrigine, size_t PositionSouhaitee );
			
			/** Insertion d'un arr�t de passage dans la liste des arr�ts d'une fiche horaire.
				@param ArretLogique Arr�t � ins�rer
				@param Position Position minimale � donner � l'arr�t

				L'insertion d�cale les arr�ts suivants une ligne plus bas. Si un trajet pi�ton (repr�sent� par deux fl�ches devant �tre attenantes) se trouve � la position demand�e, alors l'arr�t est plac� en suivant pour ne pas rompre le cheminement pi�ton.
			*/
			static size_t OrdrePAInsere(PlaceList&, const LockedLinesList&, const synthese::env::ConnectionPlace*, size_t Position );
			
			/** Contr�le de la compatibilit� entre l'ordre des arr�ts dans la grille horaire et les arr�ts du trajet. */
			static void OrdrePAConstruitLignesAPermuter( const PlaceList&, const synthese::routeplanner::Journey& __TrajetATester, bool* Resultat, size_t LigneMax );
			
			/** Recherche de point d'arr�t dans la liste des points d'arr�t.			*/
			static bool OrdrePARechercheGare( const PlaceList&, size_t& i, const synthese::env::ConnectionPlace* GareAChercher );

			static PlaceList getStopsListForScheduleTable( const synthese::routeplanner::JourneyVector& );

		public:
			/** Display.
				@param object cTrajets * : List of journeys
			*/
			void display(std::ostream& stream, const ParametersVector& parameters, const void* object = NULL, const server::Request* request= NULL) const;
			void parse( const std::string& text);
		};

	}
}

