
/** StaticActionRequest class header.
	@file StaticActionRequest.h

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

#ifndef SYNTHESE_server_StaticActionRequest_h__
#define SYNTHESE_server_StaticActionRequest_h__

#include "Request.h"
#include "Action.h"
#include "Function.h"

namespace synthese
{
	namespace server
	{
		//////////////////////////////////////////////////////////////////////////
		/// Request which calls a statically defined action.
		/// @ingroup m15
		template<class ActionT>
		class StaticActionRequest:
			public virtual Request
		{
		public:
			StaticActionRequest():
				Request()
			{
				_action = boost::shared_ptr<Action>(new ActionT);
				_redirectAfterAction = false;
			}

			explicit StaticActionRequest(const Request& request):
				Request(request)
			{
				_action = boost::shared_ptr<Action>(new ActionT);

				if(request.getAction().get())
				{
					_action->setEnv(request.getAction()->getEnv());
				}
				else if(request.getFunction().get())
				{
					_action->setEnv(request.getFunction()->getEnv());
				}
				else
				{
					throw Exception("The source request does not allow copy");
				}

				_redirectAfterAction = false;
			}


			boost::shared_ptr<ActionT> getAction() { return boost::static_pointer_cast<ActionT>(_action); }
			boost::shared_ptr<const ActionT> getAction() const { return boost::static_pointer_cast<const ActionT>(_action); }
		};
	}
}

#endif // SYNTHESE_server_StaticActionRequest_h__
