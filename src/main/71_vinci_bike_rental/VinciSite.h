
/** VinciSite class header.
	@file VinciSite.h

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Rapha�l Murat - Vinci Park <rmurat@vincipark.com>

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

#ifndef SYNTHESE_VinciSite_H__
#define SYNTHESE_VinciSite_H__

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace vinci
	{
		class VinciSite : public util::Registrable<uid, VinciSite>
		{
		private:
			std::string _name;
			std::string _address;
			std::string _phone;

		public:
			VinciSite();

			void setName(const std::string& name);
			void setAddress(const std::string& address);
			void setPhone(const std::string& phone);

			const std::string& getName() const;
			const std::string& getAddress() const;
			const std::string& getPhone() const;
		};
	}
}

#endif // SYNTHESE_VinciSite_H__

