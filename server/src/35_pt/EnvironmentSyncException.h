#ifndef SYNTHESE_ENVLSSQL_ENVIRONMENTSYNCEXCEPTION_H
#define SYNTHESE_ENVLSSQL_ENVIRONMENTSYNCEXCEPTION_H


#include "Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
namespace envlssql
{


/** Environment synchronization related exception class
@ingroup m17
*/

class EnvironmentSyncException : public synthese::Exception
{
 private:

 public:

    EnvironmentSyncException ( const std::string& message);
    ~EnvironmentSyncException () throw ();

 private:


};




}

}
#endif

