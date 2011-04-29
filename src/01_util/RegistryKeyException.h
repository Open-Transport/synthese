
/** RegistryKeyException class header.
	@file RegistryKeyException.h

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

#ifndef SYNTHESE_UTIL_REGISTRYKEYEXCEPTION_H
#define SYNTHESE_UTIL_REGISTRYKEYEXCEPTION_H

// Util
#include "Conversion.h"
#include "Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
	namespace util
	{


		/** Registry key related exception class.
			Should not be used directly, use Registrable::RegistryKeyException.
		@ingroup m01Registry m01Exceptions refExceptions
		*/
		template<class T>
		class RegistryKeyException : public synthese::Exception
		{
		 private:

			const RegistryKeyType _key; //!< Key

		 public:

			RegistryKeyException(
				const std::string& message,
				const RegistryKeyType& key
			);
			~RegistryKeyException () throw ();


			//! @name Getters/Setters
			//@{
			const RegistryKeyType& getKey () const;
			//@}


			//! @name Query methods.
			//@{
			//@}


		 private:

		};


		template<class T>
		RegistryKeyException<T>::RegistryKeyException(
			const std::string& message
			, const RegistryKeyType& key
		): Exception (message + " : key=" + Conversion::ToString (key))
			, _key (key)
		{
		}




		template<class T>
		RegistryKeyException<T>::~RegistryKeyException () throw ()
		{
		}


		template<class T>
		const RegistryKeyType& RegistryKeyException<T>::getKey () const
		{
			return _key;
		}



	}
}
#endif
