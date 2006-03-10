#ifndef SYNTHESE_ENV_REGISTRABLE_H
#define SYNTHESE_ENV_REGISTRABLE_H



#include <map>

#include <string>
#include <iostream>

#include "Registry.h"


namespace synthese
{
namespace env
{


/** Generic registry class for common environment
    operations (get, add, remove...).
    Note that the registry has the responsability of destroying
    registered objects.

@ingroup m15
*/
template<class K, class T>
class Registrable
{
 private:
    
    const K _key;

 public:

    typedef typename synthese::env::Registry<K, T> Registry;
    typedef typename synthese::env::RegistryKeyException<K> RegistryKeyException;

    Registrable (const K& key);
    virtual ~Registrable ();


    //! @name Getters/Setters
    //@{
    const K& getKey () const;
    //@}



 private:

    Registrable ( const Registrable& ref );
    Registrable& operator= ( const Registrable& rhs );

};




template<class K,class T>
Registrable<K,T>::Registrable (const K& key)
    : _key (key)
{
}


template<class K, class T>
Registrable<K,T>::~Registrable ()
{
}


template<class K, class T>
const K&
Registrable<K,T>::getKey () const
{
    return _key; 
}






}
}
#endif

