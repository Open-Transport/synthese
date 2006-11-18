
#ifndef SYNTHESE_JourneyBoardJunctionCellInterfacePage_H__
#define SYNTHESE_JourneyBoardJunctionCellInterfacePage_H__


#include "11_interfaces/InterfacePage.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/Alarm.h"

namespace synthese
{

	namespace interfaces
	{
		/** Journey board cell for use of a junction.
		@code journey_board_junction_cell @endcode
		*/
		class JourneyBoardJunctionCellInterfacePage : public InterfacePage
		{
		public:
			/** Display.
				@param place (0) Arr?t ? rejoindre ? pied</th><th>Index de l'arr?t
				@param alarm (1/2) Message d'alerte de l'arr?t ? rejoindre ? pied (RIEN = Pas d'alerte)
				@param site Displayed site
			*/
			void display( std::ostream& stream
				, const synthese::env::ConnectionPlace* place
				, const synthese::env::Alarm* alarm
				, bool color
				, const Site* site = NULL ) const;

		};
	}
}
#endif // SYNTHESE_JourneyBoardJunctionCellInterfacePage_H__
