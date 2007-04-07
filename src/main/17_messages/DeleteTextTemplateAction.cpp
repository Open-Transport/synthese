
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

#include "02_db/DBEmptyResultException.h"

#include "30_server/ActionException.h"

#include "DeleteTextTemplateAction.h"
#include "TextTemplate.h"
#include "TextTemplateTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace db;
	
	namespace messages
	{
		const string DeleteTextTemplateAction::PARAMETER_TEXT_ID = Action_PARAMETER_PREFIX + "tt";


		ParametersMap DeleteTextTemplateAction::getParametersMap() const
		{
			ParametersMap map;
			if (_text)
				map.insert(make_pair(PARAMETER_TEXT_ID, Conversion::ToString(_text->getKey())));
			return map;
		}

		void DeleteTextTemplateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_TEXT_ID);
				if (it == map.end())
					throw ActionException("Text template not specified");

				_text = TextTemplateTableSync::get(Conversion::ToLongLong(it->second));
			}
			catch (DBEmptyResultException e)
			{
				throw ActionException("Specified text template not found");
			}
		}

		DeleteTextTemplateAction::DeleteTextTemplateAction()
			: Action()
			, _text(NULL)
		{}

		void DeleteTextTemplateAction::run()
		{
			TextTemplateTableSync::remove(_text->getKey());
		}

		DeleteTextTemplateAction::~DeleteTextTemplateAction()
		{
			delete _text;
		}
	}
}
