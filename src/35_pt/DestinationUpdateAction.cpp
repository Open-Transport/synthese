
//////////////////////////////////////////////////////////////////////////
/// DestinationUpdateAction class implementation.
/// @file DestinationUpdateAction.cpp
/// @author RCSobility
/// @date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ActionException.h"
#include "ParametersMap.h"
#include "DestinationUpdateAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "DestinationTableSync.hpp"
#include "ImportableAdmin.hpp"
#include "ImportableTableSync.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace impex;
	

	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::DestinationUpdateAction>::FACTORY_KEY("DestinationUpdateAction");
	}

	namespace pt
	{
		const string DestinationUpdateAction::PARAMETER_DESTINATION_ID = Action_PARAMETER_PREFIX + "destination_id";
		const string DestinationUpdateAction::PARAMETER_COMMENT = Action_PARAMETER_PREFIX + "comment";
		const string DestinationUpdateAction::PARAMETER_DISPLAYED_TEXT = Action_PARAMETER_PREFIX + "displayed_text";
		const string DestinationUpdateAction::PARAMETER_TTS_TEXT = Action_PARAMETER_PREFIX + "tts_text";
		
		
		
		ParametersMap DestinationUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_destination.get())
			{
				map.insert(PARAMETER_DESTINATION_ID, _destination->getKey());
			}
			if(_comment)
			{
				map.insert(PARAMETER_COMMENT, *_comment);
			}
			if(_displayedText)
			{
				map.insert(PARAMETER_DISPLAYED_TEXT, *_displayedText);
			}
			if(_ttsText)
			{
				map.insert(PARAMETER_TTS_TEXT, *_ttsText);
			}
			if(_dataSourceLinks)
			{
				map.insert(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS, ImportableTableSync::SerializeDataSourceLinks(*_dataSourceLinks));
			}
			return map;
		}
		
		
		
		void DestinationUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Destination
			if(map.isDefined(PARAMETER_DESTINATION_ID))
			{
				try
				{
					_destination = DestinationTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_DESTINATION_ID), *_env);
				}
				catch(ObjectNotFoundException<Destination>&)
				{
					throw ActionException("No such destination");
				}
			}
			else
			{
				_destination.reset(new Destination);
			}

			// Comment
			if(map.isDefined(PARAMETER_COMMENT))
			{
				_comment = map.get<string>(PARAMETER_COMMENT);
			}

			// Displayed text
			if(map.isDefined(PARAMETER_DISPLAYED_TEXT))
			{
				_displayedText = map.get<string>(PARAMETER_DISPLAYED_TEXT);
			}

			// TTS Text
			if(map.isDefined(PARAMETER_TTS_TEXT))
			{
				_ttsText = map.get<string>(PARAMETER_TTS_TEXT);
			}

			// Data source links
			if(map.isDefined(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS))
			{
				_dataSourceLinks = ImportableTableSync::GetDataSourceLinksFromSerializedString(map.get<string>(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS), *_env);
			}
		}
		
		
		
		void DestinationUpdateAction::run(
			Request& request
		){
			// Comment
			if(_comment)
			{
				_destination->setComment(*_comment);
			}

			// Displayed text
			if(_displayedText)
			{
				_destination->setDisplayedText(*_displayedText);
			}

			// TTS Text
			if(_ttsText)
			{
				_destination->setTTSText(*_ttsText);
			}

			// Data source links
			if(_dataSourceLinks)
			{
				_destination->setDataSourceLinks(*_dataSourceLinks);
			}

			// Saving
			DestinationTableSync::Save(_destination.get());

			// Return of created object id
			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_destination->getKey());
			}
		}
		
		
		
		bool DestinationUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
