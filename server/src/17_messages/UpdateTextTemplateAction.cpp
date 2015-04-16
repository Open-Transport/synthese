
/** UpdateTextTemplateAction class implementation.
	@file UpdateTextTemplateAction.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "Profile.h"
#include "Session.h"
#include "TextTemplate.h"
#include "TextTemplateTableSync.h"
#include "User.h"
#include "DBLogModule.h"
#include "MessagesLibraryLog.h"
#include "MessagesLibraryRight.h"
#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "Env.h"

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
				_text = TextTemplateTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_TEXT_ID), *_env);

				//id = map.get<RegistryKeyType>(PARAMETER_FOLDER_ID);
				//_folder = TextTemplateTableSync::Get(id, *_env);

				// Name
				_name = map.get<string>(PARAMETER_NAME);
				if (_name.empty())
					throw ActionException("Le nom ne peut être vide");

				Env env;
				TextTemplateTableSync::Search(env, _text->getParentId(), false, _name, _text.get(), 0, 1);
				if (!env.getRegistry<TextTemplate>().empty())
					throw ActionException("Un texte portant ce nom existe déjà.");

				// Short message
				_shortMessage = map.get<string>(PARAMETER_SHORT_MESSAGE);

				// Long message
				_longMessage = map.get<string>(PARAMETER_LONG_MESSAGE);
			}
			catch (ObjectNotFoundException<TextTemplate>& e)
			{
				throw ActionException("text template", e, *this);
			}
			catch(ParametersMap::MissingParameterException e)
			{
				throw ActionException(e, *this);
			}
		}

		void UpdateTextTemplateAction::run(Request& request)
		{
			stringstream logChanges;

			DBLogModule::appendToLogIfChange(logChanges, "Nom", _text->getName(), _name);
			_text->setName(_name);
			DBLogModule::appendToLogIfChange(logChanges, "Message court", _text->getShortMessage(), _shortMessage);
			_text->setShortMessage(_shortMessage);
			DBLogModule::appendToLogIfChange(logChanges, "Message long", _text->getLongMessage(), _longMessage);
			_text->setLongMessage(_longMessage);
			//DBLogModule::appendToLogIfChange(logChanges, "Déplacement", _text->getParentId(), _folder->getKey());
			//_text->setParentId(_folder->getKey());

			TextTemplateTableSync::Save(_text.get());

			MessagesLibraryLog::AddTemplateUpdateEntry(*_text, logChanges.str(), request.getUser().get());
		}



		void UpdateTextTemplateAction::setTemplate( boost::shared_ptr<TextTemplate> text )
		{
			_text = text;
		}



		bool UpdateTextTemplateAction::isAuthorized(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(WRITE);
		}
	}
}
