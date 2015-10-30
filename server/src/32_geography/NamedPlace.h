
/** NamedPlace class header.
	@file NamedPlace.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_geography_NamedPlace_h__
#define SYNTHESE_geography_NamedPlace_h__

#include "FactoryBase.h"
#include "Place.h"

namespace synthese
{
	namespace geography
	{
		class City;

		//////////////////////////////////////////////////////////////////////////
		/// Named Place class.
		///	Named place is the base for any class which is associated with a name
		///	and a city. However, a place is not related to the concept
		///	of geographical location in terms of coordinates; it is rather
		///	a human abstraction.
		///
		///	@ingroup m32
		//////////////////////////////////////////////////////////////////////////
		class NamedPlace:
			public util::FactoryBase<NamedPlace>,
			public virtual util::Registrable,
			public virtual Place
		{
		private:
			std::string _name;  //!< The name
			std::string _name13;
			std::string _name26;
			City* _city; //!< The city where this place is located

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Default constructor.
			NamedPlace();


			virtual bool _getCityNameBeforePlaceName() const;

		public:
			//! @name Getters
			//@{

				/** Gets name of this place.
				*/
				virtual std::string getName () const { return _name; }
				const std::string& getName13() const { return _name13; }
				const std::string& getName26() const { return _name26; }

				/** Gets city where this place is located.
				*/
				City* getCity () const { return _city; }
			//@}

			//! @name Setters
			//@{
				virtual void setName (const std::string& value){ _name = value; }
				virtual void setCity(City* value){ _city = value; }
				virtual void setName13(const std::string& value){ _name13 = value; }
				virtual void setName26(const std::string& value){ _name26 = value; }
			//@}


			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Gets a concatenation between city name and place name.
				/// @return city name, a space, and place name. Empty string if the city is
				///	undefined
				std::string getFullName() const;



				/** Gets a 13 characters name for the place.
					@return string :
						- the name13 record if defined
						- the 13 first characters of the name else
					@author Hugues Romain
					@date 2007
				*/
				std::string getName13OrName() const;

				/** Gets a 26 characters name for the place.
					@return string :
						- the name26 record if defined
						- the 26 first characters of the name else
					@author Hugues Romain
					@date 2007
				*/
				std::string getName26OrName() const;

				virtual std::string getNameForAllPlacesMatcher(
					std::string text = std::string()
				) const = 0;


				/** Gets official name of this place.
				* The default implementation is to return this name.
				*/
				virtual const std::string getOfficialName () const;
			//@}
		};
}	}

#endif // SYNTHESE_geography_NamedPlace_h__
