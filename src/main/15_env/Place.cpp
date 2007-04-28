
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

#include "15_env/Place.h"
#include "15_env/City.h"

#include "01_util/WithoutAccentsFilter.h"
#include "01_util/PlainCharFilter.h"

#include <sstream>
#include <boost/iostreams/filtering_stream.hpp>

using namespace std;

namespace synthese
{
	using namespace util;

	namespace env
	{
		Place::Place (const std::string& name,
				  const City* city)
			: _name (name)
			, _city (city)
		{
			stringstream ss;
			boost::iostreams::filtering_ostream out;
			out.push (WithoutAccentsFilter());
			out.push (PlainCharFilter());
			out.push (ss);
			out << _name << std::flush;

			_name13 = ss.str().substr(0, 13);
			_name26 = ss.str().substr(0, 26);
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
			stringstream ss;
			boost::iostreams::filtering_ostream out;
			out.push (WithoutAccentsFilter());
			out.push (PlainCharFilter());
			out.push (ss);
			out << name << std::flush;

			_name = name;
			
			if (_name13.empty())
				_name13 = ss.str().substr(0, 13);
			if (_name26.empty())
				_name26 = ss.str().substr(0, 26);
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
			VertexAccess access;
			access.approachDistance = 0;
			access.approachTime = 0;

			return access;
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

	}
}
