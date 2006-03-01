#ifndef SYNTHESE_UTIL_EXCEPTION_H
#define SYNTHESE_UTIL_EXCEPTION_H


#include "module.h"

#include <string>
#include <iostream>



namespace synthese
{
namespace util
{

/** Base exception class
@ingroup m04
*/
class Exception
{
 private:

    const std::string _message; //!< Error message


 public:

    Exception ( const std::string& message );
    ~Exception ();


    //! @name Getters/Setters
    //@{
    const std::string& getMessage () const;
    //@}
    

 private:

    Exception ( const Exception& ref );
    Exception& operator= ( const Exception& rhs );

};

std::ostream& operator<< ( std::ostream& os, const Exception& op );



}

}
#endif
