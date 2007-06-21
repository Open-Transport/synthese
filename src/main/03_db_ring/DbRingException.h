#ifndef SYNTHESE_DBRING_DBRINGEXCEPTION_H
#define SYNTHESE_DBRING_DBRINGEXCEPTION_H


#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
namespace dbring
{


/** Db ring related exception class.
	@ingroup m03

*/

class DbRingException : public synthese::util::Exception
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

