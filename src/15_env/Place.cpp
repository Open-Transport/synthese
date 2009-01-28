
/** Place class implementation.
	@file Place.cpp

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

#include "Place.h"
#include "City.h"
#include "VertexAccessMap.h"

#include "WithoutAccentsFilter.h"
#include "PlainCharFilter.h"

#include <sstream>
#include <boost/iostreams/filtering_stream.hpp>

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace graph;

	namespace env
	{
		Place::Place(
			const std::string& name,
			const City* city
		):	_name (name)
			, _city (city)
			, _isoBarycentreToUpdate(false)
		{
		}


		Place::~Place ()
		{

		}



		const std::string& 
		Place::getName () const
		{
			return _name;
		}



		void 
		Place::setName (const std::string& name)
		{
			_name = name;
		}




		const std::string& 
		Place::getOfficialName () const
		{
			return getName ();
		}



		const City* 
		Place::getCity () const
		{
			return _city;
		}
		    

		    
		VertexAccess
		Place::getVertexAccess (const AccessDirection& accessDirection,
					const AccessParameters& accessParameters,
					const Vertex* destination,
					const Vertex* origin) const
		{
			return VertexAccess(0, 0, Journey(accessDirection));
		}

		const std::string& Place::getName13() const
		{
			return _name13;
		}

		const std::string& Place::getName26() const
		{
			return _name26;
		}

		const std::string Place::getFullName() const
		{
			return ((_city != NULL) ? (_city->getName() + " ") : "") + getName();
		}

		void Place::setCity( const City* city )
		{
			_city = city;
		}

		bool Place::includes( const Place* place ) const
		{
			return place == this;
		}

		void Place::setName13( const std::string& name )
		{
			_name13 = name;
		}

		void Place::setName26( const std::string& name )
		{
			_name26 = name;
		}

		string Place::getName13OrName() const
		{
			if (_name13.empty())
			{
				stringstream ss;
				boost::iostreams::filtering_ostream out;
				out.push (WithoutAccentsFilter());
				out.push (PlainCharFilter());
				out.push (ss);
				out << _name << std::flush;

				return ss.str().substr(0, 13);
			}
			else
				return _name13;
		}

		std::string Place::getName26OrName() const
		{
			if (_name26.empty())
			{
				stringstream ss;
				boost::iostreams::filtering_ostream out;
				out.push (WithoutAccentsFilter());
				out.push (PlainCharFilter());
				out.push (ss);
				out << _name << std::flush;

				return ss.str().substr(0, 26);
			}
			else
				return _name26;

		}
		
	}
}
