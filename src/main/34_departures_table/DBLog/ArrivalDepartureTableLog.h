#ifndef SYNTHESE_ARRIVAL_DEPARTURE_TABLE_LOG
#define SYNTHESE_ARRIVAL_DEPARTURE_TABLE_LOG

#include "01_util/DBLog.h"

namespace synthese
{
	namespace util
	{
		/** Journal relatif à la gestion des paramètres du téléaffichage.

			Colonnes :
				- UID objet modifié
				- Action effectuée				
		*/
		class ArrivalDepartureTableLog : public DBLog
		{
		};
	}
}

#endif
