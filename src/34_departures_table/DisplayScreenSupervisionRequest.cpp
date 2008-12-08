
/** DisplayScreenSupervisionRequest class implementation.
	@file DisplayScreenSupervisionRequest.cpp

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

#include "RequestException.h"

#include "DisplayScreenSupervisionRequest.h"
#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace dblog;

	template<> const string util::FactorableTemplate<Function,departurestable::DisplayScreenSupervisionRequest>::FACTORY_KEY("tds");

	namespace departurestable
	{
		const std::string DisplayScreenSupervisionRequest::PARAMETER_DISPLAY_SCREEN_ID = "tb";
		const std::string DisplayScreenSupervisionRequest::PARAMETER_STATUS = "status";
		
		ParametersMap DisplayScreenSupervisionRequest::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_DISPLAY_SCREEN_ID, _displayScreen->getKey());
			map.insert(PARAMETER_STATUS, _text);
			return map;
		}

		void DisplayScreenSupervisionRequest::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_displayScreen = DisplayScreenTableSync::Get(map.getUid(PARAMETER_DISPLAY_SCREEN_ID, true, "dssr"));
				_text = map.getString(PARAMETER_STATUS, true, "dssr");
			}
			catch(ObjectNotFoundException<DisplayScreen>& e)
			{
				throw RequestException("Display screen not found / " + e.getMessage());
			}
			catch (...)
			{
				throw RequestException("Unknown error");
			}
		}

		void DisplayScreenSupervisionRequest::_run( std::ostream& stream ) const
		{
			_displayScreen->recordSupervision(_text);
		}
	}
}
