
/** DeleteTextTemplateAction class implementation.
	@file DeleteTextTemplateAction.cpp

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

#include "DeleteTextTemplateAction.h"
#include "TextTemplateTableSync.h"

#include "17_messages/MessagesLibraryLog.h"
#include "17_messages/MessagesLibraryRight.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace util;
	using namespace security;

	template<> const string util::FactorableTemplate<Action, messages::DeleteTextTemplateAction>::FACTORY_KEY("dtta");
	
	namespace messages
	{
		const string DeleteTextTemplateAction::PARAMETER_TEXT_ID = Action_PARAMETER_PREFIX + "tt";


		ParametersMap DeleteTextTemplateAction::getParametersMap() const
		{
			ParametersMap map;
			if (_text.get())
				map.insert(PARAMETER_TEXT_ID, _text->getKey());
			return map;
		}

		void DeleteTextTemplateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				uid id(map.getUid(PARAMETER_TEXT_ID, true, FACTORY_KEY));
				_text = TextTemplateTableSync::Get(id, *_env);
			}
			catch (ObjectNotFoundException<TextTemplate>& e)
			{
				throw ActionException(e.getMessage());
			}
		}
		void DeleteTextTemplateAction::run(Request& request)
		{
			MessagesLibraryLog::AddTemplateDeleteEntry(*_text, request.getUser().get());

			TextTemplateTableSync::Remove(_text->getKey());
		}



		void DeleteTextTemplateAction::setTemplate( boost::shared_ptr<const TextTemplate> value )
		{
			_text = value;
		}



		bool DeleteTextTemplateAction::isAuthorized(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(DELETE_RIGHT);
		}
	}
}
