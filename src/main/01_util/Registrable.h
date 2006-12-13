#ifndef SYNTHESE_UTIL_REGISTRABLE_H
#define SYNTHESE_UTIL_REGISTRABLE_H



#include <map>

#include <string>
#include <iostream>

#include "Registry.h"


namespace synthese
{
namespace util
{


/** Generic registry class for common environment
    operations (get, add, remove...).
    Note that the registry has the responsability of destroying
    registered objects.

@ingroup m01
*/
template<class K, class T>
class Registrable
{
 private:
    
    K _key;

 public:

    typedef typename synthese::util::Registry<K, T> Registry;
    typedef typename synthese::util::RegistryKeyException<K, T> RegistryKeyException;

    Registrable (const K& key);
    virtual ~Registrable ();


    //! @name Getters/Setters
    //@{
    const K& getKey () const;
	void setKey(const K& key);
//    const K& getId () const;
    //@}



 private:

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


template<class K, class T>
void
Registrable<K,T>::setKey (const K& key)
{
	_key = key;
}

/*
template<class K, class T>
const K&
Registrable<K,T>::getId () const
{
    return _key; 
}
*/





}
}
#endif


