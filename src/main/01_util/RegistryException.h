#ifndef SYNTHESE_UTIL_REGISTRYEXCEPTION_H
#define SYNTHESE_UTIL_REGISTRYEXCEPTION_H


#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
namespace util
{


/** Registry related exception class
@ingroup m01
*/
template<class K, class T>
class RegistryException : public synthese::util::Exception
{
 private:

    const K* _key; //!< Key

 public:

    RegistryException ( const std::string& message, const K* key = 0 );
    ~RegistryException ();


    //! @name Getters/Setters
    //@{
    const K& getKey () const;
    //@}
    

 private:


};



}

}
#endif
