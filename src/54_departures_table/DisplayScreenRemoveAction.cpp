
/** DisplayScreenRemoveAction class implementation.
	@file DisplayScreenRemoveAction.cpp

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

#include "DisplayScreenRemoveAction.h"
#include "DisplayScreenTableSync.h"
#include "ArrivalDepartureTableLog.h"
#include "ArrivalDepartureTableRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "Right.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	template<> const string util::FactorableTemplate<Action, departurestable::DisplayScreenRemoveAction>::FACTORY_KEY("dsra");

	namespace departurestable
	{
		const string DisplayScreenRemoveAction::PARAMETER_DISPLAY_SCREEN_ID(Action_PARAMETER_PREFIX + "dsi");


		ParametersMap DisplayScreenRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_DISPLAY_SCREEN_ID, _displayScreen->getKey());
			return map;
		}

		void DisplayScreenRemoveAction::_setFromParametersMap(const ParametersMap& map) throw(ActionException)
		{
			uid id(map.getUid(PARAMETER_DISPLAY_SCREEN_ID, true, FACTORY_KEY));
			
			try
			{
				_displayScreen = DisplayScreenTableSync::Get(id, *_env);
			}
			catch (...)
			{
				throw ActionException("Specified display screen not found");
			}
		}

		void DisplayScreenRemoveAction::run(
			Request& request
		) throw(ActionException) {

			DisplayScreenTableSync::Remove(_displayScreen->getKey());

			ArrivalDepartureTableLog::addRemoveEntry(_displayScreen.get(), request.getUser().get());

		}

		bool DisplayScreenRemoveAction::isAuthorized(const server::Session* session) const
		{
			assert(_displayScreen.get() != NULL);
			if (_displayScreen->getLocalization() != NULL)
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(DELETE_RIGHT, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_displayScreen->getLocalization()->getKey()));
			}
			else
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE);
			}
		}

		void DisplayScreenRemoveAction::setDisplayScreen( boost::shared_ptr<const DisplayScreen> screen )
		{
			_displayScreen = screen;
		}
	}
}
