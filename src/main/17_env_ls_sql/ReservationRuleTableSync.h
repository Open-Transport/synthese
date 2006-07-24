#ifndef SYNTHESE_ENVLSSQL_RESERVATIONRULETABLESYNC_H
#define SYNTHESE_ENVLSSQL_RESERVATIONRULETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class ReservationRule;
}


namespace envlssql
{


/** 

ReservationRule SQLite table synchronizer.

@ingroup m17
*/

class ReservationRuleTableSync : public ComponentTableSync
{
 private:

 public:

    ReservationRuleTableSync (synthese::env::Environment::Registry& environments);
    ~ReservationRuleTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:

};




}

}
#endif
