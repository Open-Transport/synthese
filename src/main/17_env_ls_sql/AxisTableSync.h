#ifndef SYNTHESE_ENVLSSQL_AXISTABLESYNC_H
#define SYNTHESE_ENVLSSQL_AXISTABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class Axis;
}


namespace envlssql
{


/** 

Axis SQLite table synchronizer.

@ingroup m17
*/

class AxisTableSync : public ComponentTableSync
{
 private:

 public:

    AxisTableSync (synthese::env::Environment::Registry& environments);
    ~AxisTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:

    synthese::env::Axis* createFromRow (const synthese::env::Environment& env,
					const synthese::db::SQLiteResult& rows, 
					int rowIndex) const;
};




}

}
#endif
