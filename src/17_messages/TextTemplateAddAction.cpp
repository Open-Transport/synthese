
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

#include "TextTemplateAddAction.h"
#include "TextTemplate.h"
#include "TextTemplateTableSync.h"

#include "MessagesLibraryLog.h"
#include "MessagesLibraryRight.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"

#include "Conversion.h"
#include "Env.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace security;

	template<> const string util::FactorableTemplate<Action, messages::TextTemplateAddAction>::FACTORY_KEY("mttaa");
	
	namespace messages
	{
		const string TextTemplateAddAction::PARAMETER_LONG_MESSAGE = Action_PARAMETER_PREFIX + "lm";
		const string TextTemplateAddAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string TextTemplateAddAction::PARAMETER_SHORT_MESSAGE = Action_PARAMETER_PREFIX + "sm";
		const string TextTemplateAddAction::PARAMETER_IS_FOLDER(Action_PARAMETER_PREFIX + "if");
		const string TextTemplateAddAction::PARAMETER_PARENT_ID(Action_PARAMETER_PREFIX + "pi");


		ParametersMap TextTemplateAddAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_LONG_MESSAGE, _longMessage);
			map.insert(PARAMETER_NAME, _name);
			map.insert(PARAMETER_SHORT_MESSAGE, _shortMessage);
			map.insert(PARAMETER_PARENT_ID, _parentId);
			map.insert(PARAMETER_IS_FOLDER, _isFolder);
			return map;
		}

		void TextTemplateAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);

			_isFolder = map.getBool(PARAMETER_IS_FOLDER, false, false, FACTORY_KEY);

			_parentId = map.getUid(PARAMETER_PARENT_ID, true, FACTORY_KEY);
			if (_parentId > 0)
			{
				shared_ptr<const TextTemplate> parent;
				try
				{
					parent = TextTemplateTableSync::Get(_parentId, *_env);
				}
				catch(...)
				{
					throw ActionException("No such folder");
				}
				if (!parent->getIsFolder())
					throw ActionException("This is not a folder");
			}

			Env env;
			TextTemplateTableSync::Search(env, _parentId, _isFolder, _name, NULL, 0, 1);
			if (!env.getRegistry<TextTemplate>().empty())
				throw ActionException("Un texte portant ce nom existe déjà.");

			_longMessage = map.getString(PARAMETER_LONG_MESSAGE, true, FACTORY_KEY);
			_shortMessage = map.getString(PARAMETER_SHORT_MESSAGE, true, FACTORY_KEY);
		}

		void TextTemplateAddAction::run(Request& request)
		{
			TextTemplate tt;
			tt.setLongMessage(_longMessage);
			tt.setShortMessage(_shortMessage);
			tt.setName(_name);
			tt.setIsFolder(_isFolder);
			tt.setParentId(_parentId);
			TextTemplateTableSync::Save(&tt);

			// Log
			MessagesLibraryLog::AddTemplateCreationEntry(tt, request.getUser().get());
		}



		bool TextTemplateAddAction::isAuthorized(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(WRITE);
		}



		void TextTemplateAddAction::setParentId(uid value)
		{
			_parentId = value;
		}



		void TextTemplateAddAction::setIsFolder( bool value )
		{
			_isFolder = value;
		}
	}
}
