
/** Exception class implementation.
	@file Exception.cpp

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

#include "KillProcessException.h"


namespace synthese
{
	KillProcessException::KillProcessException ( const std::string& message ) throw ()
		: _message (message)
	{
	}


	KillProcessException::~KillProcessException () throw ()
	{
	}



	KillProcessException::KillProcessException ( const KillProcessException& ref )
		: _message (ref._message)
	{
	}



	const std::string&
	KillProcessException::getMessage () const
	{
		return _message;
	}


	const char*
	KillProcessException::what () const throw ()
	{
		return _message.c_str ();
	}
}

