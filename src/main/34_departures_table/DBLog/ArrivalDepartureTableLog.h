#ifndef SYNTHESE_ARRIVAL_DEPARTURE_TABLE_LOG
#define SYNTHESE_ARRIVAL_DEPARTURE_TABLE_LOG

#include "01_util/DBLog.h"

namespace synthese
{
	namespace util
	{
		/** Journal relatif � la gestion des param�tres du t�l�affichage.

			Colonnes :
				- UID objet modifi�
				- Action effectu�e				
		*/
		class ArrivalDepartureTableLog : public DBLog
		{
		};
	}
}

#endif
