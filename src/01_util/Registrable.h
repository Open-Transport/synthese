
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

#include "UtilTypes.h"
#include "01_util/Constants.h"

namespace synthese
{
	namespace util
	{

		/** Base class for all registrable objects.

			As this class is very much used, it is dangerous to derivate directly from it.
			Use a virtual derivation to ensure that there is always only one instantiation of the
			class in a sub-class instantiation.

			Each Registrable class must determinate its Registry class by a typedef called Registry.
			Two ways are possible :
				- util::Registry : standard registry, allows abstract classes, but auto creation is forbidden
				- util::RegistryWithAutocreationEnabled : allows auto creation but incompatible with abstract classes
		
			@ingroup m01Registry
		*/
		class Registrable
		{
		private:
			RegistryKeyType _key;	//!< ID of the object

		protected:

			/** Constructor.
				@param key ID of the object.
			*/
			Registrable(RegistryKeyType key);

		public:
			virtual ~Registrable();

			//! \name Getters
			//@{
				/** ID of the object getter.
					@return the ID of the object.
				*/
				RegistryKeyType getKey() const;
			//@}

			//! \name Setters
			//@{
				/** ID of the object setter.
					@param key the ID of the object to set 
				*/
				void setKey(RegistryKeyType key);
			//@}
		};
	}
}

#endif
