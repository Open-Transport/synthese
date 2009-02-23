
/** ServiceDateTableSync class header.
	@file ServiceDateTableSync.h

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

#ifndef SYNTHESE_ServiceDateTableSync_H__
#define SYNTHESE_ServiceDateTableSync_H__

#include <vector>
#include <string>
#include <iostream>

#include "Date.h"

#include "SQLiteRegistryTableSyncTemplate.h"

namespace synthese
{
	namespace env
	{
		class NonPermanentService;
	}
	
	namespace pt
	{
		class ServiceDate;
		
		/** Service dates table synchronizer.
			@ingroup m35LS refLS
		*/
		class ServiceDateTableSync
		:	public db::SQLiteRegistryTableSyncTemplate<ServiceDateTableSync,ServiceDate>
		{
		public:
			static const std::string COL_SERVICEID;
			static const std::string COL_DATE;
			
			ServiceDateTableSync();


			/** Sets all active dates on a service.
			 * 
			 * @param service The service to update
			 */
			static void SetActiveDates(
				env::NonPermanentService& service
			);
			
			static void DeleteDatesFromNow(
				util::RegistryKeyType serviceId
			);
		};
	}
}

#endif // SYNTHESE_ServiceDateTableSync_H__
