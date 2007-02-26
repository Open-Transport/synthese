
/** VinciContract class header.
	@file VinciContract.h

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#ifndef SYNTHESE_VinciContract_H__
#define SYNTHESE_VinciContract_H__

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "02_db/SQLiteTableSyncTemplate.h"

#include "04_time/DateTime.h"

namespace synthese
{
	namespace db
	{
		class SQLiteQueueThreadExec;
	}

	namespace security
	{
		class User;
	}

	namespace vinci
	{
		class VinciSite;
		
		/** Vinci bike rent contract.

			@warning the cached user belongs to the contract and is deleted with the contract
			@ingroup m71
		*/
		class VinciContract : public util::Registrable<uid, VinciContract>
		{
		private:
			uid				_userId;
			uid				_siteId;
			time::DateTime	_date;

			security::User*	_user;
			VinciSite*		_site;
			
		public:
			VinciContract(uid id=0);
			~VinciContract();

			void setUserId(uid id);
			void setSiteId(uid id);
			void setDate(const time::DateTime& date);

			uid						getUserId() const;
			uid						getSiteId() const;
			const time::DateTime&	getDate()	const;
			
			security::User*			getUser() const;
			VinciSite*				getSite() const;
		};
	}
}

#endif // SYNTHESE_VinciContract_H__

