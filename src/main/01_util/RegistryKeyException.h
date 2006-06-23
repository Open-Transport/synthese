#ifndef SYNTHESE_UTIL_REGISTRYKEYEXCEPTION_H
#define SYNTHESE_UTIL_REGISTRYKEYEXCEPTION_H


#include "01_util/Exception.h"

#include <string>
#include <iostream>
#include <sstream>


namespace synthese
{
namespace util
{


/** Registry key related exception class.
    Should not be used directly, use Registrable::RegistryKeyException.
@ingroup m01
*/
template<class K>
class RegistryKeyException : public synthese::util::Exception
{
 private:

    const K _key; //!< Key

 public:

    RegistryKeyException ( const std::string& message, const K& key ) throw ();
    ~RegistryKeyException () throw ();


    //! @name Getters/Setters
    //@{
    const K& getKey () const;
    //@}


    //! @name Query methods.
    //@{
	const char* what () const throw ();
    //@}


 private:

};


template<class K>
RegistryKeyException<K>::RegistryKeyException ( const std::string& message, 
						const K& key ) throw ()
    : synthese::util::Exception (message)
    , _key (key)
{
}




template<class K>
RegistryKeyException<K>::~RegistryKeyException () throw ()
{
}


template<class K>
const K& 
RegistryKeyException<K>::getKey () const
{
    return _key;
}

template<class K>
const char* 
RegistryKeyException<K>::what () const throw ()
{
	// TODO : a revoir !!
	std::stringstream ss;
	ss << getMessage () << " : key=" << getKey () << std::flush;
	return ss.str ().c_str ();
}


/*
template<class K>
std::ostream& operator<< ( std::ostream& os, const RegistryKeyException<K>& op )
{
    os << op.getMessage () << " : key=" << op.getKey ();
}
*/


 
}
}
#endif
