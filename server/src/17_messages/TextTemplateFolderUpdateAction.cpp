////////////////////////////////////////////////////////////////////////////////
/// TextTemplateFolderUpdateAction class implementation.
///	@file TextTemplateFolderUpdateAction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "TextTemplateFolderUpdateAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "User.h"
#include "MessagesLibraryLog.h"
#include "TextTemplate.h"
#include "TextTemplateTableSync.h"
#include "DBLogModule.h"
#include "Env.h"
#include "MessagesLibraryRight.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace dblog;
	using namespace util;
	using namespace security;


	namespace util
	{
		template<> const string FactorableTemplate<Action, messages::TextTemplateFolderUpdateAction>::FACTORY_KEY("TextTemplateFolderUpdateAction");
	}

	namespace messages
	{
		const string TextTemplateFolderUpdateAction::PARAMETER_FOLDER_ID(Action_PARAMETER_PREFIX + "fi");
		const string TextTemplateFolderUpdateAction::PARAMETER_NAME(Action_PARAMETER_PREFIX + "na");
		const string TextTemplateFolderUpdateAction::PARAMETER_PARENT_ID(Action_PARAMETER_PREFIX + "pi");



		TextTemplateFolderUpdateAction::TextTemplateFolderUpdateAction()
			: util::FactorableTemplate<Action, TextTemplateFolderUpdateAction>()
		{
		}



		ParametersMap TextTemplateFolderUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if (_folder.get())
				map.insert(PARAMETER_FOLDER_ID, _folder->getKey());
			return map;
		}



		void TextTemplateFolderUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			setFolderId(map.get<RegistryKeyType>(PARAMETER_FOLDER_ID));
			_name = map.get<string>(PARAMETER_NAME);
			RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_PARENT_ID));
			if (id > 0)
			{
				_parent = TextTemplateTableSync::Get(id, *_env);
			}

			Env env;
			TextTemplateTableSync::Search(
				env,
				_folder->getParentId(),
				true,
				_name,
				_folder.get(),
				0, 1
			);

			if (!env.getRegistry<TextTemplate>().empty())
				throw ActionException("Un répertoire de même nom existe déjà à l'emplacement spécifié");
		}



		void TextTemplateFolderUpdateAction::run(Request& request)
		{
			stringstream s;

			DBLogModule::appendToLogIfChange(s, "Nom", _folder->getName(), _name);
			_folder->setName(_name);

			// Parent
			DBLogModule::appendToLogIfChange(
				s,
				"Répertoire parent",
				lexical_cast<string>(_folder->getParentId()),
				lexical_cast<string>(_parent->getKey())
			);
			_folder->setParentId(_parent.get() ? _parent->getKey() : 0);

			TextTemplateTableSync::Save(_folder.get());

			MessagesLibraryLog::AddTemplateFolderUpdateEntry(*_folder, s.str(), request.getUser().get());
		}



		void TextTemplateFolderUpdateAction::setFolderId(RegistryKeyType id )
		{
			try
			{
				_folder = TextTemplateTableSync::GetEditable(id, *_env);
			}
			catch(...)
			{
				throw ActionException("No such folder");
			}
		}



		bool TextTemplateFolderUpdateAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(WRITE);
		}
	}
}
