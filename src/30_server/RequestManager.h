
/** RequestManager class header.
	@file RequestManager.h

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

#ifndef SYNTHESE_server_RequestManager_h__
#define SYNTHESE_server_RequestManager_h__

#include "Request.h"

#include "RequestException.h"
#include "RedirectException.h"
#include "HTTPRequest.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include <ostream>
#include <map>

namespace synthese
{
	namespace server
	{
		//////////////////////////////////////////////////////////////////////////
		/// Policy based request request definition.
		/// @ingroup m15
		template<class StaticDynamicPolicyT>
		class RequestManager:
			public StaticDynamicPolicyT,
			public Request
		{
		protected:
			typedef StaticDynamicPolicyT StaticDynamicPolicy;

			virtual void _loadAction();
			virtual void _loadFunction(
				bool actionException,
				const std::string& errorMessage,
				boost::optional<util::RegistryKeyType> actionCreatedId
			) { StaticDynamicPolicy::_loadFunction(actionException, errorMessage, actionCreatedId); }
			virtual void _deleteAction() { StaticDynamicPolicy::_deleteAction(); }

		public:

			//! \name Constructor and destructor
			//@{
				/** Construction from a HTTP request object to parse.
					@param httpRequest The HTTP request to parse
					@throw RequestException if the string is incomplete or contains refused values according to the parameters validators
					@author Hugues Romain
					@date 2007-2009		
				*/
				explicit RequestManager(const HTTPRequest& httpRequest);


				/** Construction of an empty request.
					@author Hugues Romain
					@date 2007
					Use the public setters to fill the request.					
				*/
				RequestManager();



				/** Construction of a request from an other one.
					@param request Request to copy (default/NULL = no copy)
					@author Hugues Romain
					@date 2009
					Use the public setters to fill the request.					
				*/
				RequestManager(const Request& request);
			//@}

			//! @name Getters
			//@{
				virtual boost::shared_ptr<Function> _getFunction() { return StaticDynamicPolicy::_getFunction(); }
				virtual boost::shared_ptr<const Function> _getFunction() const { return StaticDynamicPolicy::_getFunction(); }

				/** Action getter.
					@return const Action* The action of the request
					@author Hugues Romain
					@date 2007					
				*/
				virtual boost::shared_ptr<const Action> _getAction() const { return StaticDynamicPolicy::_getAction(); }
				virtual boost::shared_ptr<Action> _getAction() { return StaticDynamicPolicy::_getAction(); }
			//@}


			//! @name Modifiers
			//@{
			//@}
		};



		template<class StaticDynamicPolicy>
		void synthese::server::RequestManager<StaticDynamicPolicy>::_loadAction()
		{
			StaticDynamicPolicy::_loadAction();
		}



		template<class StaticDynamicPolicy>
		RequestManager<StaticDynamicPolicy>::RequestManager(
			const Request& request
		):	StaticDynamicPolicy(request),
			Request(request)
		{
		}



		template<class StaticDynamicPolicy>
		RequestManager<StaticDynamicPolicy>::RequestManager():
			StaticDynamicPolicy(),
			Request(string())
		{
		}



		template<class StaticDynamicPolicy>
		RequestManager<StaticDynamicPolicy>::RequestManager(const HTTPRequest& httpRequest):
			StaticDynamicPolicy(),
			Request(httpRequest)
		{
			_setParametersMap(ParametersMap(httpRequest.postData));
		}
	}
}

#endif // SYNTHESE_server_RequestManager_h__
