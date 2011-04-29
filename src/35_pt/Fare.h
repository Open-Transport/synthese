
/** Fare class header.
	@file Fare.h

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

#ifndef SYNTHESE_ENV_FARE_H
#define SYNTHESE_ENV_FARE_H

#include "Registrable.h"
#include "Registry.h"

#include <string>

namespace synthese
{
	namespace pt
	{
		/** Fare handling class

			Compliance values :
				- true : with toll
				- indeterminate : no sense
				- false : toll free

			@ingroup m35
		*/
		class Fare
		:	public virtual util::Registrable
		{
		 public:

			typedef enum
			{
				FARE_TYPE_FREE = 0,
				FARE_TYPE_SECTION = 1,
				FARE_TYPE_DISTANCE = 2,
				FARE_TYPE_ZONAL = 3
			} FareType;

			/// Chosen registry class.
			typedef util::Registry<Fare>	Registry;

		 private:

			std::string _name; //!< Fare name
			FareType _type; //!< Fare type

		public:
			Fare(
				util::RegistryKeyType key = 0
			);
			~Fare ();


			//! @name Getters
			//@{
				const std::string& getName () const { return _name; }
				const FareType& getType () const { return _type; }
			//@}

			//! @name Setters
			//@{
				void setName (const std::string& name) { _name = name; }
				void setType (const FareType& fareType) { _type = fareType; }
			//@}
		};
	}
}

#endif
