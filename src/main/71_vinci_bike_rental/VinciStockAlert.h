
/** VinciStockAlert class header.
	@file VinciStockAlert.h

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

#ifndef SYNTHESE_vinci_VinciStockAlert_h__
#define SYNTHESE_vinci_VinciStockAlert_h__

#include "01_util/UId.h"
#include "01_util/Registrable.h"

namespace synthese
{
	namespace vinci
	{
		/** VinciStockAlert class.
			@ingroup m71
		*/
		class VinciStockAlert : public util::Registrable<uid, VinciStockAlert>
		{
			uid		_site_id;
			uid		_account_id;
			double	_min_alert;
			double	_max_alert;

		public:
			VinciStockAlert();

			void setSiteId(uid value);
			void setAccountId(uid value);
			void setMinAlert(double value);
			void setMaxAlert(double value);

			uid		getSiteId()		const;
			uid		getAccountId()	const;
			double	getMinAlert()	const;
			double	getMaxAlert()	const;

		};
	}
}

#endif // SYNTHESE_vinci_VinciStockAlert_h__
