
/** VinciStockAlert class implementation.
	@file VinciStockAlert.cpp

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

#include "VinciStockAlert.h"

#include "01_util/Constants.h"

namespace synthese
{
	namespace vinci
	{


		VinciStockAlert::VinciStockAlert()
			: _max_alert(UNKNOWN_VALUE)
			, _min_alert(UNKNOWN_VALUE)
		{

		}

		void VinciStockAlert::setSiteId( uid value )
		{
			_site_id = value;
		}

		void VinciStockAlert::setAccountId( uid value )
		{
			_account_id = value;
		}

		void VinciStockAlert::setMinAlert( double value )
		{
			_min_alert = value;
		}

		void VinciStockAlert::setMaxAlert( double value )
		{
			_max_alert = value;
		}

		uid VinciStockAlert::getSiteId() const
		{
			return _site_id;
		}

		uid VinciStockAlert::getAccountId() const
		{
			return _account_id;
		}

		double VinciStockAlert::getMinAlert() const
		{
			return _min_alert;
		}

		double VinciStockAlert::getMaxAlert() const
		{
			return _max_alert;
		}
	}
}
