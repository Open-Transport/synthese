
/** Collection of advanced SQL queries.
	@file AdvancedSelectTableSync.h

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

#include <map>
#include <utility>

namespace synthese
{
	namespace time
	{
		class Date;
	}

	namespace vinci
	{
		class VinciRate;

		/** @addtogroup m71LS
			@{
		*/

		struct RentReportResult { int starts; int ends; int renewals; } ;
		std::map<time::Date, RentReportResult> getRentsPerDay(const time::Date& start, const time::Date& end);
		std::map<std::pair<time::Date, const VinciRate*>, RentReportResult> getRentsPerRatePerDay(const time::Date& start, const time::Date& end);
		std::map<uid, RentReportResult> getRentsPerRate(const time::Date& start, const time::Date& end);

		struct StockSizeResult { double size; double minAlert; double maxAlert; double unitPrice; };
		typedef std::map<std::pair<uid,uid>, StockSizeResult> StocksSize;
		StocksSize getStocksSize(uid accountId, uid siteId);

		/** @} */
	}
}
