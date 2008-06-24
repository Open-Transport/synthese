
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

#include "UpdateTextTemplateAction.h"
#include "TextTemplate.h"
#include "TextTemplateTableSync.h"

#include "13_dblog/DBLogModule.h"

#include "17_messages/MessagesLibraryLog.h"
#include "17_messages/MessagesLibraryRight.h"

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"
#include "30_server/Request.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace util;
	using namespace dblog;
	using namespace security;

	template<> const string util::FactorableTemplate<Action, messages::UpdateTextTemplateAction>::FACTORY_KEY("utta");
	
	namespace messages
	{
		const string UpdateTextTemplateAction::PARAMETER_TEXT_ID = Action_PARAMETER_PREFIX + "tid";
		const string UpdateTextTemplateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "n";
		const string UpdateTextTemplateAction::PARAMETER_SHORT_MESSAGE = Action_PARAMETER_PREFIX + "sm";
		const string UpdateTextTemplateAction::PARAMETER_LONG_MESSAGE = Action_PARAMETER_PREFIX + "lm";
		const string UpdateTextTemplateAction::PARAMETER_FOLDER_ID = Action_PARAMETER_PREFIX + "fi";


		ParametersMap UpdateTextTemplateAction::getParametersMap() const
		{
			ParametersMap map;
			if (_text.get())
				map.insert(PARAMETER_TEXT_ID, _text->getKey());
			return map;
		}

		void UpdateTextTemplateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				// Text ID
				uid id = map.getUid(PARAMETER_TEXT_ID, true, FACTORY_KEY);
				_text = TextTemplateTableSync::GetUpdateable(id);
		
				id = map.getUid(PARAMETER_FOLDER_ID, true, FACTORY_KEY);
				_folder = TextTemplateTableSync::Get(id);

				// Name
				_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);
				if (_name.empty())
					throw ActionException("Le nom ne peut être vide");
				vector<shared_ptr<TextTemplate> > v = TextTemplateTableSync::Search(ALARM_LEVEL_UNKNOWN, _text->getParentId(), false, _name, _text.get(), 0, 1);
				if (!v.empty())
					throw ActionException("Un texte portant ce nom existe déjà.");

				// Short message
				_shortMessage = map.getString(PARAMETER_SHORT_MESSAGE, true, FACTORY_KEY);
				
				// Long message
				_longMessage = map.getString(PARAMETER_LONG_MESSAGE, true, FACTORY_KEY);
			}
			catch (TextTemplate::ObjectNotFoundException& e)
			{
				throw ActionException(e.getMessage());
			}
		}

		void UpdateTextTemplateAction::run()
		{
			stringstream logChanges;

			DBLogModule::appendToLogIfChange(logChanges, "Nom", _text->getName(), _name);
			_text->setName(_name);
			DBLogModule::appendToLogIfChange(logChanges, "Message court", _text->getShortMessage(), _shortMessage);
			_text->setShortMessage(_shortMessage);
			DBLogModule::appendToLogIfChange(logChanges, "Message long", _text->getLongMessage(), _longMessage);
			_text->setLongMessage(_longMessage);
			DBLogModule::appendToLogIfChange(logChanges, "Déplacement", _text->getParentId(), _folder->getKey());
			_text->setParentId(_folder->getKey());

			TextTemplateTableSync::save(_text.get());

			MessagesLibraryLog::AddTemplateUpdateEntry(*_text, logChanges.str(), _request->getUser().get());
		}



		void UpdateTextTemplateAction::setTemplate( boost::shared_ptr<TextTemplate> text )
		{
			_text = text;
		}



		bool UpdateTextTemplateAction::_isAuthorized() const
		{
			return _request->isAuthorized<MessagesLibraryRight>(WRITE);
		}
	}
}