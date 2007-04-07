
/** UpdateTextTemplateAction class implementation.
	@file UpdateTextTemplateAction.cpp

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

#include "02_db/DBEmptyResultException.h"

#include "30_server/ActionException.h"

#include "UpdateTextTemplateAction.h"
#include "TextTemplate.h"
#include "TextTemplateTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace db;
	
	namespace messages
	{
		const string UpdateTextTemplateAction::PARAMETER_TEXT_ID = Action_PARAMETER_PREFIX + "tid";
		const string UpdateTextTemplateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "n";
		const string UpdateTextTemplateAction::PARAMETER_SHORT_MESSAGE = Action_PARAMETER_PREFIX + "sm";
		const string UpdateTextTemplateAction::PARAMETER_LONG_MESSAGE = Action_PARAMETER_PREFIX + "lm";


		ParametersMap UpdateTextTemplateAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void UpdateTextTemplateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it;

				// Text ID
				it = map.find(PARAMETER_TEXT_ID);
				if (it == map.end())
					throw ActionException("Text template not specified");
				_text = TextTemplateTableSync::get(Conversion::ToLongLong(it->second));
				
				// Name
				it = map.find(PARAMETER_NAME);
				if (it == map.end())
					throw ActionException("Name not specified");
				_name = it->second;
				if (_name.empty())
					throw ActionException("Le nom ne peut être vide");
				vector<TextTemplate*> v = TextTemplateTableSync::search(_text->getAlarmLevel(), _name, 0, 1);
				if (!v.empty())
				{
					delete v.front();
					throw ActionException("Un texte portant ce nom existe déjà.");
				}

				// Short message
				it = map.find(PARAMETER_SHORT_MESSAGE);
				if (it == map.end())
					throw ActionException("Short message not specified");
				_shortMessage = it->second;
				
				// Long message
				it = map.find(PARAMETER_LONG_MESSAGE);
				if (it == map.end())
					throw ActionException("Long message not specified");
				_longMessage = it->second;
			}
			catch (DBEmptyResultException e)
			{
				throw ActionException("Specified text template not found");
			}
		}

		UpdateTextTemplateAction::UpdateTextTemplateAction()
			: Action()
			, _text(NULL)
		{}

		void UpdateTextTemplateAction::run()
		{
			_text->setName(_name);
			_text->setShortMessage(_shortMessage);
			_text->setLongMessage(_longMessage);
			TextTemplateTableSync::save(_text);
		}

		UpdateTextTemplateAction::~UpdateTextTemplateAction()
		{
			delete _text;
		}
	}
}