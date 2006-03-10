#ifndef SYNTHESE_ENV_REGISTRYEXCEPTION_H
#define SYNTHESE_ENV_REGISTRYEXCEPTION_H


#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
namespace env
{


/** Registry related exception class
@ingroup m01
*/
template<class K>
class RegistryException : public synthese::util::Exception
{
 private:

    const K* _key; //!< Key

 public:

    RegistryException ( const std::string& message, const K* key = 0 )
    ~RegistryException ()


    //! @name Getters/Setters
    //@{
    const K& getKey () const;
    //@}
    

 private:

    Exception ( const Exception& ref );
    Exception& operator= ( const Exception& rhs );

};

std::ostream& operator<< ( std::ostream& os, const Exception& op );



}

}
#endif
