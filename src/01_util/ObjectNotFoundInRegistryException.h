
/** ObjectNotFoundInRegistryException class header.
	@file ObjectNotFoundInRegistryException.h

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

#ifndef SYNTHESE_util_ObjectNotFoundInRegistryException_h__
#define SYNTHESE_util_ObjectNotFoundInRegistryException_h__

#include "01_util/ObjectNotFoundException.h"

#include "01_util/UId.h"
#include "01_util/Conversion.h"

namespace synthese
{
	namespace util
	{
		/** ObjectNotFoundInRegistryException class.
			@ingroup m01Exceptions refExceptions
		*/
		template<class K, class T>
		class ObjectNotFoundInRegistryException : public ObjectNotFoundException<K,T>
		{
		public:
			ObjectNotFoundInRegistryException(const K& key) throw ()
				: ObjectNotFoundException<K,T>(key, "Object not found in registry")
			{}

			~ObjectNotFoundInRegistryException() throw () {}

		};

		template<class T>
		class ObjectNotFoundInRegistryException<uid,T> : public ObjectNotFoundException<uid,T>
		{
		public:

			ObjectNotFoundInRegistryException(const uid& key) throw ()
				: ObjectNotFoundException<uid,T>(key, "Object not found in registry " + Conversion::ToString(decodeTableId(key)))
			{}

			~ObjectNotFoundInRegistryException() throw () {}

		};

	}
}

#endif // SYNTHESE_util_ObjectNotFoundInRegistryException_h__
