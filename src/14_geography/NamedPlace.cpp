
/** NamedPlace class implementation.
	@file NamedPlace.cpp

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

#include "NamedPlace.h"
#include "City.h"
#include "WithoutAccentsFilter.h"
#include "PlainCharFilter.h"

#include <boost/iostreams/filtering_stream.hpp>
#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	namespace geography
	{
		NamedPlace::NamedPlace(
		):	_city(NULL)
		{

		}



		const std::string& 
			NamedPlace::getName () const
		{
			return _name;
		}



		void 
			NamedPlace::setName (const std::string& name)
		{
			_name = name;
		}




		const std::string& 
			NamedPlace::getOfficialName () const
		{
			return getName ();
		}



		const City* NamedPlace::getCity () const
		{
			return _city;
		}



		const std::string& NamedPlace::getName13() const
		{
			return _name13;
		}

		const std::string& NamedPlace::getName26() const
		{
			return _name26;
		}

		const std::string NamedPlace::getFullName() const
		{
			return ((_city != NULL) ? (_city->getName() + " ") : "") + getName();
		}

		void NamedPlace::setCity( const City* city )
		{
			_city = city;
		}


		void NamedPlace::setName13( const std::string& name )
		{
			_name13 = name;
		}

		void NamedPlace::setName26( const std::string& name )
		{
			_name26 = name;
		}

		string NamedPlace::getName13OrName() const
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

		std::string NamedPlace::getName26OrName() const
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
