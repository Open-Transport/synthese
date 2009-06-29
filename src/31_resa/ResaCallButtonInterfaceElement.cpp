
/** ResaCallButtonInterfaceElement class implementation.
	@file ResaCallButtonInterfaceElement.cpp
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

#include "ResaCallButtonInterfaceElement.h"

#include "11_interfaces/ValueElementList.h"

#include "AdminInterfaceElement.h"

#include "31_resa/CallBeginAction.h"
#include "31_resa/CallEndAction.h"
#include "31_resa/ReservationRoutePlannerAdmin.h"
#include "31_resa/ResaEditLogEntryAdmin.h"
#include "31_resa/ResaModule.h"
#include "31_resa/ResaRight.h"

#include "30_server/ActionFunctionRequest.h"

#include "05_html/HTMLModule.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace admin;
	using namespace html;
	using namespace server;
	using namespace security;
	
	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, resa::ResaCallButtonInterfaceElement>::FACTORY_KEY("callbutton");
	}

	namespace resa
	{
		void ResaCallButtonInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_takeCallIcon = vel.front();
			_takeCallText = vel.front();
			_stopCallIcon = vel.front();
			_stopCallText = vel.front();
		}

		string ResaCallButtonInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {

			if (!request->isAuthorized<ResaRight>(WRITE,UNKNOWN_RIGHT_LEVEL))
				return string();

			uid callId(ResaModule::GetCurrentCallId(request->getSession()));

			if (callId == UNKNOWN_VALUE)
			{ // Case call start

				ActionFunctionRequest<CallBeginAction,AdminRequest> callRequest(request);
				callRequest.getFunction()->setPage<ReservationRoutePlannerAdmin>();
				stream << HTMLModule::getLinkButton(callRequest.getURL(), _takeCallText->getValue(parameters,variables,object,request),string(), _takeCallIcon->getValue(parameters,variables,object,request));
			}
			else
			{
				ActionFunctionRequest<CallEndAction,AdminRequest> callRequest(request);
				callRequest.getFunction()->setPage<ResaEditLogEntryAdmin>();
				callRequest.setObjectId(callId);
				stream << HTMLModule::getLinkButton(callRequest.getURL(), _stopCallText->getValue(parameters,variables,object,request),string(), _stopCallIcon->getValue(parameters,variables,object,request));
			}

			return string();
		}

		ResaCallButtonInterfaceElement::~ResaCallButtonInterfaceElement()
		{
		}
	}
}
