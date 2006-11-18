#ifndef SYNTHESE_UTIL_REGISTRY_H
#define SYNTHESE_UTIL_REGISTRY_H



#include <map>
#include "RegistryKeyException.h"



namespace synthese
{
namespace util
{

class Exception;

/** Generic registry class for common environment
    operations (get, add, remove...).
    Note that the registry has the responsability of destroying
    registered objects.
    This class should not be used directly : use instead the
    "template" typedef in the classes derived from Registrable.

@ingroup m01
*/
template<class K, class T>
class Registry
{
 private:
    
    std::map<K, T*> _registry;

 public:

    Registry ();
    ~Registry ();

    //! @name Getters/Setters
    //@{
    //@}


    //! @name Query methods
    //@{
    bool contains (const K& key) const;
    T* get (const K& key);
    const T* get (const K& key) const;

    size_t size () const;
    //@}

    
    //! @name Update methods
    //@{

    /** Removes and destroy all the registered objects.
     */
    void clear ();

    void add (T* ptr);
    void replace (T* ptr);
    void remove (const K& key);
    //@}
    

 private:

    Registry ( const Registry& ref );
    Registry& operator= ( const Registry& rhs );

};




template<class K, class T>
Registry<K,T>::Registry ()
{
}



template<class K, class T>
Registry<K,T>::~Registry ()
{
}



template<class K, class T>
bool 
Registry<K,T>::contains (const K& key) const
{
    return _registry.find (key) != _registry.end ();
}



template<class K, class T>
T* 
Registry<K,T>::get (const K& key)
{
    if (contains (key) == false) 
	throw RegistryKeyException<K,T> ("No such key in registry", key);

    return _registry.find (key)->second;
}



template<class K, class T>
const T* 
Registry<K,T>::get (const K& key) const
{
    if (contains (key) == false) 
	throw RegistryKeyException<K,T> ("No such key in registry", key);

    return _registry.find (key)->second;
}





template<class K, class T>
size_t 
Registry<K,T>::size () const
{
    return _registry.size ();
}



template<class K, class T>
void 
Registry<K,T>::clear ()
{
    for (typename std::map<K,T*>::iterator iter = _registry.begin ();
	 iter != _registry.end (); ++iter)
    {
	delete iter->second;
    }
    _registry.clear ();
}



template<class K, class T>
void 
Registry<K,T>::add (T* ptr)
{
    if (contains (ptr->getKey ())) 
	throw RegistryKeyException<K,T> ("Duplicate key in registry", ptr->getKey ());
    
    _registry.insert (std::make_pair (ptr->getKey (), ptr));
}


template<class K, class T>
void 
Registry<K,T>::replace (T* ptr)
{
    remove (ptr->getKey ());
    _registry.insert (std::make_pair (ptr->getKey (), ptr));
}



template<class K, class T>
void 
Registry<K,T>::remove (const K& key)
{
    if (contains (key) == false) 
	throw RegistryKeyException<K,T> ("No such key in registry", key);

    delete _registry.find (key)->second;
    _registry.erase (key);
}







}
}
#endif

