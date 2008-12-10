
/** Place class header.
	@file Place.h

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

#ifndef SYNTHESE_ENV_PLACE_H
#define SYNTHESE_ENV_PLACE_H


#include <map>
#include <string>
#include <vector>

#include "Registrable.h"

#include "15_env/Types.h"

#include "IsoBarycentre.h"

namespace synthese
{
	namespace geometry
	{
		class Point2D;
	}

	namespace env
	{
		class AccessParameters;
		class City;
		class Vertex;
		struct VertexAccess;
		class VertexAccessMap;

		//////////////////////////////////////////////////////////////////////////
		/// Place base class.
		///
		///	Place is the base for any class which is associated with a name
		///	and a city. However, a place is not related to the concept
		///	of geographical location in terms of coordinates; it is rather
		///	a human abstraction.
		///
		///	@ingroup m35
		//////////////////////////////////////////////////////////////////////////
		class Place
			: public virtual util::Registrable
		{
		public:

		protected:
			mutable bool _isoBarycentreToUpdate;
			mutable geometry::IsoBarycentre _isoBarycentre;

		private:

			std::string _name;  //!< The name
			std::string _name13;
			std::string _name26;
			const City* _city; //!< The city where this place is located

		protected:

			Place(
				const std::string& name,
				const City* city
			);

		public:

			virtual ~Place ();


			//! @name Getters
			//@{

				/** Gets name of this place.
				*/
				const std::string& getName () const;
				const std::string& getName13() const;
				const std::string& getName26() const;


				/** Gets official name of this place.
				* The default implementation is to return this name.
				*/
				virtual const std::string& getOfficialName () const;

				const std::string getFullName() const;


				/** Gets city where this place is located.
				*/
				const City* getCity () const;

			//@}

			//! @name Setters
			//@{
				void setName (const std::string& name);
				void setCity(const City* city);
				void setName13(const std::string& name);
				void setName26(const std::string& name);
			//@}



			//! @name Query methods
			//@{

			virtual VertexAccess getVertexAccess (
				const AccessDirection& accessDirection
				, const AccessParameters& accessParameters
				, const Vertex* destination
				, const Vertex* origin
			) const;


			virtual void getImmediateVertices(
				VertexAccessMap& result
				, const AccessDirection& accessDirection
				, const AccessParameters& accessParameters
				, SearchAddresses returnAddresses
				, SearchPhysicalStops returnPhysicalStops
				, const Vertex* origin = NULL
			) const = 0;

			virtual const geometry::Point2D& getPoint() const = 0;

			virtual bool includes(const Place* place) const;

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

			//@}

		    
		};


	}
}

#endif

