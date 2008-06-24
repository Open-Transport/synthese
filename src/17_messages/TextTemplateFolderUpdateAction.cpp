
/** TextTemplateFolderUpdateAction class implementation.
	@file TextTemplateFolderUpdateAction.cpp
	@author Hugues Romain
	@date 2008

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

#include <sstream>

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"
#include "30_server/Request.h"

#include "TextTemplateFolderUpdateAction.h"

#include "17_messages/MessagesLibraryLog.h"
#include "17_messages/TextTemplate.h"
#include "17_messages/TextTemplateTableSync.h"

#include "13_dblog/DBLogModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace dblog;
	using namespace util;
	
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
			setFolderId(map.getUid(PARAMETER_FOLDER_ID, true, FACTORY_KEY));
			_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);
			uid id(map.getUid(PARAMETER_PARENT_ID, true, FACTORY_KEY));
			if (id > 0)
				_parent = TextTemplateTableSync::Get(id);

			vector<shared_ptr<TextTemplate> >	folders(TextTemplateTableSync::Search(
				ALARM_LEVEL_UNKNOWN, _folder->getParentId(), true, _name, _folder.get(), 0, 1
				));

			if (!folders.empty())
				throw ActionException("Un répertoire de même nom existe déjà à l'emplacement spécifié");
		}
		
		
		
		void TextTemplateFolderUpdateAction::run()
		{
			stringstream s;

			DBLogModule::appendToLogIfChange(s, "Nom", _folder->getName(), _name);
			_folder->setName(_name);
			DBLogModule::appendToLogIfChange(s, "Répertoire parent", Conversion::ToString(_folder->getParentId()), Conversion::ToString(_parent->getKey()));
			_folder->setParentId(_parent.get() ? _parent->getKey() : 0);

			TextTemplateTableSync::save(_folder.get());

			MessagesLibraryLog::AddTemplateFolderUpdateEntry(*_folder, s.str(), _request->getUser().get());
		}



		void TextTemplateFolderUpdateAction::setFolderId( uid id )
		{
			try
			{
				_folder = TextTemplateTableSync::GetUpdateable(id);
			}
			catch(...)
			{
				throw ActionException("No such folder");
			}
		}
	}
}
