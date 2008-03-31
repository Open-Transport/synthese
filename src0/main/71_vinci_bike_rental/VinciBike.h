
/** VinciBike class header.
	@file VinciBike.h

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

#ifndef SYNTHESE_VinciBike_H__
#define SYNTHESE_VinciBike_H__

#include <string>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace accounts
	{
		class Transaction;
	}

	namespace vinci
	{
		class VinciContract;

		/** Bike to rent.
			@ingroup m71
		*/
		class VinciBike : public util::Registrable<uid, VinciBike>
		{
		public:
			typedef struct
			{
				boost::shared_ptr<VinciContract>			lastContract;		//!< Last rent contract
				boost::shared_ptr<accounts::Transaction>	lastTransaction;	//!< Last transaction
			} Complements;

		private:
			//! \name Attributes
			//@{
			std::string _number;
			std::string _markedNumber;
			//@}

			Complements _complements;

		public:
			VinciBike(uid id=0);

			const std::string& getNumber() const;
			const std::string& getMarkedNumber() const;

			Complements getComplements() const;

			void setNumber(const std::string& number);
			void setMarkedNumber(const std::string& markedNumber);
			void setComplements(const Complements& complements);
		};
	}
}

#endif // SYNTHESE_VinciBike_H__
