
/** Registrable class header.
	@file Registrable.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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


