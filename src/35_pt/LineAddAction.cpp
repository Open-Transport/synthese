
//////////////////////////////////////////////////////////////////////////
/// LineAddAction class implementation.
/// @file LineAddAction.cpp
/// @author Hugues
/// @date 2010
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
#include "LineAddAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "CommercialLine.h"
#include "Line.h"
#include "LineTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace env;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::LineAddAction>::FACTORY_KEY("LineAddAction");
	}

	namespace pt
	{
		const string LineAddAction::PARAMETER_COMMERCIAL_LINE_ID = Action_PARAMETER_PREFIX + "cl";
		
		
		
		ParametersMap LineAddAction::getParametersMap() const
		{
			ParametersMap map;
			if(_commercialLine.get())
			{
				map.insert(PARAMETER_COMMERCIAL_LINE_ID, _commercialLine->getKey());
			}
			return map;
		}
		
		
		
		void LineAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_commercialLine = Env::GetOfficialEnv().getEditable<CommercialLine>(map.get<RegistryKeyType>(PARAMETER_COMMERCIAL_LINE_ID));
			}
			catch(ObjectNotFoundException<CommercialLine>& e)
			{
				throw ActionException("No such commercial line");
			}

		}
		
		
		
		void LineAddAction::run(
			Request& request
		){
			Line object;
			object.setCommercialLine(_commercialLine.get());
			LineTableSync::Save(&object);
			//::AddCreationEntry(object, request.getUser().get());
			request.setActionCreatedId(object.getKey());
		}
		
		
		
		bool LineAddAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}



		void LineAddAction::setCommercialLine( boost::shared_ptr<env::CommercialLine> value )
		{
			_commercialLine = value;
		}
	}
}
