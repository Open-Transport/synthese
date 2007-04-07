
/** TextTemplateAddAction class implementation.
	@file TextTemplateAddAction.cpp

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

#include "30_server/ActionException.h"

#include "TextTemplateAddAction.h"
#include "TextTemplate.h"
#include "TextTemplateTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace messages
	{
		const string TextTemplateAddAction::PARAMETER_LONG_MESSAGE = Action_PARAMETER_PREFIX + "lm";
		const string TextTemplateAddAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string TextTemplateAddAction::PARAMETER_SHORT_MESSAGE = Action_PARAMETER_PREFIX + "sm";
		const string TextTemplateAddAction::PARAMETER_TYPE = Action_PARAMETER_PREFIX + "ty";


		ParametersMap TextTemplateAddAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_LONG_MESSAGE, _longMessage));
			map.insert(make_pair(PARAMETER_NAME, _name));
			map.insert(make_pair(PARAMETER_SHORT_MESSAGE, _shortMessage));
			map.insert(make_pair(PARAMETER_TYPE, Conversion::ToString((int) _level)));
			return map;
		}

		void TextTemplateAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			it = map.find(PARAMETER_NAME);
			if (it == map.end())
				throw ActionException("Name not found");
			_name = it->second;

			it = map.find(PARAMETER_TYPE);
			if (it == map.end())
				throw ActionException("Level not found");
			_level = (AlarmLevel) Conversion::ToInt(it->second);
			if (_level == ALARM_LEVEL_UNKNOWN)
				throw ActionException("Bad value for level");
			
			vector<TextTemplate*> v = TextTemplateTableSync::search(_level, _name, 0, 1);
			if (!v.empty())
			{
				delete v.front();
				throw ActionException("Un texte portant ce nom existe déjà.");
			}

			it = map.find(PARAMETER_LONG_MESSAGE);
			if (it == map.end())
				throw ActionException("Long message not found");
			_longMessage = it->second;
			
			 it = map.find(PARAMETER_SHORT_MESSAGE);
			 if (it == map.end())
				 throw ActionException("Short message not found");
			 _shortMessage = it->second;
		}

		TextTemplateAddAction::TextTemplateAddAction()
			: Action()
		{}

		void TextTemplateAddAction::run()
		{
			TextTemplate* tt = new TextTemplate;
			tt->setAlarmLevel(_level);
			tt->setLongMessage(_longMessage);
			tt->setShortMessage(_shortMessage);
			tt->setName(_name);
			TextTemplateTableSync::save(tt);
			delete tt;
		}
	}
}
