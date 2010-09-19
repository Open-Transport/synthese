#ifndef SYNTHESE_DBRING_DBRINGEXCEPTION_H
#define SYNTHESE_DBRING_DBRINGEXCEPTION_H


#include "Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
namespace dbring
{


/** Db ring related exception class.
	@ingroup m101

*/

class DbRingException : public synthese::Exception
{
 private:

 public:

    DbRingException ( const std::string& message);
    ~DbRingException () throw ();

 private:


};




}

}
#endif

