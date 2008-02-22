
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
		class SQLite;
	}

	namespace security
	{
		class User;
	}

	namespace accounts
	{
		class TransactionPart;
	}

	namespace vinci
	{
		class VinciSite;
		class VinciBike;
		class VinciAntivol;
		
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
			std::string		_passport;

			time::DateTime	_late;
			double			_due;
			time::DateTime	_outDatedGuarantee;
			
		public:
			VinciContract(uid id=0);

			//!	\name Setters
			//@{
				void setUserId(uid id);
				void setSiteId(uid id);
				void setDate(const time::DateTime& date);
				void setPassport(const std::string& text);
				void setLate(const time::DateTime& date);
				void setDue(double value);
				void setOutDatedGuarantee(const time::DateTime& date);
			//@}

			//! \name Getters
			//@{
				uid						getUserId()				const;
				uid						getSiteId()				const;
				const time::DateTime&	getDate()				const;
				const std::string&		getPassport()			const;
				const time::DateTime&	getLate()				const;
				double					getDue()				const;
				const time::DateTime&	getOutDatedGuarantee()	const;
			//@}

			//! \name Queries
			//@{
				boost::shared_ptr<security::User>				getUser()								const;
				boost::shared_ptr<VinciSite>					getSite()								const;
				boost::shared_ptr<VinciBike>					getCurrentBike()						const;
				boost::shared_ptr<accounts::TransactionPart>	getCurrentRentTransactionPart()			const;
				boost::shared_ptr<VinciAntivol>					getCurrentLock()						const;
				boost::shared_ptr<accounts::TransactionPart>	getCurrentGuaranteeTransactionPart()	const;
			//@}
		};
	}
}

#endif // SYNTHESE_VinciContract_H__
