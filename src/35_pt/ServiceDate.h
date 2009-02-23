
/** ServiceDate class header.
	@file ServiceDate.h

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

#ifndef SYNTHESE_ServiceDate_h__
#define SYNTHESE_ServiceDate_h__

#include "Date.h"
#include "Registrable.h"
#include "Registry.h"

namespace synthese
{
	namespace env
	{
		class NonPermanentService;
	}
	
	namespace pt
	{
		/** ServiceDate class.
			This class is available for the template instanciation of the ServiceDateTableSync.
			It can be used as a simple structure.
			CalendarDate
			@ingroup m35
		*/
		class ServiceDate
		:	public util::Registrable
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<ServiceDate>	Registry;
		
		private:
			env::NonPermanentService*	_service;
			time::Date	_date;
			
		public:
			ServiceDate(
				util::RegistryKeyType key = UNKNOWN_VALUE
			);
			
			env::NonPermanentService*	getService() const;
			const time::Date&				getDate() const;
			
			void setService(env::NonPermanentService* value);
			void setDate(const time::Date& value);
		};
	}
}

#endif // SYNTHESE_ServiceDate_h__
