
/** DurationInterfacePage class implementation.
	@file DurationInterfacePage.cpp

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

#include "DurationInterfacePage.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;

	template<> const std::string util::FactorableTemplate<interfaces::InterfacePage,interfaces::DurationInterfacePage>::FACTORY_KEY("duration");

	namespace interfaces
	{
		const string DurationInterfacePage::DATA_HOURS("hours");
		const string DurationInterfacePage::DATA_MINUTES("minutes");
		const string DurationInterfacePage::DATA_TOTAL_MINUTES("total_minutes");

		void DurationInterfacePage::display(std::ostream& stream, int duration, VariablesMap& variables, const void* object /*= NULL*/, const server::Request* request /*= NULL*/) const
		{
			ParametersVector pv;
			pv.push_back(Conversion::ToString(duration));
			pv.push_back(Conversion::ToString(duration / 60));
			pv.push_back(Conversion::ToString(duration % 60));

			InterfacePage::_display(stream, pv, variables, object, request);
		}



		DurationInterfacePage::DurationInterfacePage()
			: Registrable(UNKNOWN_VALUE)
		{

		}
	}
}
