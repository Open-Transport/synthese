#ifndef SYNTHESE_DBRING_UPDATECHRONOLOGYEXCEPTION_H
#define SYNTHESE_DBRING_UPDATECHRONOLOGYEXCEPTION_H


#include "03_db_ring/DbRingException.h"

#include <string>
#include <iostream>


namespace synthese
{
namespace dbring
{


/** Db ring related exception class.
	@ingroup m03

*/

class UpdateChronologyException : public DbRingException
{
 private:

 public:

    UpdateChronologyException ( const std::string& message) : DbRingException (message) {}
    ~UpdateChronologyException () throw () {};

 private:


};




}

}
#endif

