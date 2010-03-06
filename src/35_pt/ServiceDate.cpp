
/** Service date class implementation.
	@file ServiceDate.cpp

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

#include "ServiceDate.h"

using namespace boost::gregorian;


namespace synthese
{
	using namespace util;
	
	namespace util
	{
		template<> const std::string Registry<pt::ServiceDate>::KEY("ServiceDate");
	}

	namespace pt
	{
		ServiceDate::ServiceDate(
			RegistryKeyType key
		):	_service(NULL),
			Registrable(key)
		{ }
		
		NonPermanentService*	ServiceDate::getService() const
		{
			return _service;
		}
		
		const date&				ServiceDate::getDate() const
		{
			return _date;
		}
		
		void ServiceDate::setService(NonPermanentService* value)
		{
			_service = value;
		}
		
		void ServiceDate::setDate(const date& value)
		{
			_date = value;
		}
 	}
 }
