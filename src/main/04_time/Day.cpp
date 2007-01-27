
/** Day class implementation.
	@file Day.cpp

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

#include "01_util/Constants.h"

#include "04_time/Day.h"

#include "assert.h"


namespace synthese
{
namespace time
{

Day::Day ( int value )
        : _value ( value )
{
    assert (( _value == UNKNOWN_VALUE ) || ( ( _value >= 1 ) && ( _value <= 31 ) ));
}


Day::Day ( const Day& ref )
        : _value ( ref._value )
{
    assert (( _value == UNKNOWN_VALUE ) || ( ( _value >= 1 ) && ( _value <= 31 ) ));
}


int
Day::getValue () const
{
    return _value;
}




Day&
Day::operator= ( const int value )
{
    _value = value;
    return *this;
}

Day&
Day::operator--( int )
{
    _value--;
    return *this;
}

Day&
Day::operator++( int )
{
    _value++;
    return *this;
}



std::ostream& operator<< ( std::ostream& os, const Day& op )
{
    os << op.getValue ();
    return os;
}


}
}

