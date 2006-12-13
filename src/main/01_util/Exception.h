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
@ingroup m01
*/
class Exception : public std::exception
{
 private:

    const std::string _message; //!< Error message


 public:

    Exception ( const std::string& message ) throw ();
    Exception ( const Exception& ref );

    ~Exception () throw ();
    

    //! @name Getters/Setters
    //@{
    const std::string& getMessage () const;
    //@}
    
    //! @name Query methods
    //@{
	const char* what () const throw ();

 private:

    Exception& operator= ( const Exception& rhs );

};




}

}
#endif

