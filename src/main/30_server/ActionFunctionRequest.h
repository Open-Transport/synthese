
/** ActionFunctionRequest class header.
	@file ActionFunctionRequest.h

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

#ifndef SYNTHESE_ActionFunctionRequest_h__
#define SYNTHESE_ActionFunctionRequest_h__

#include "30_server/FunctionRequest.h"

namespace synthese
{
	namespace server
	{
		/** Template for a Request containing an action and a function.
			@ingroup m18
		*/
		template<class A, class F>
		class ActionFunctionRequest : public FunctionRequest<F>
		{
		public:
			/** Constructor.
				@param request request to copy (default = NULL)
				@author Hugues Romain
				@date 2007				
			*/
			ActionFunctionRequest(const Request* request=NULL);

			boost::shared_ptr<A> getAction();
			boost::shared_ptr<const A> getAction() const;


		};

		template<class A, class F>
		boost::shared_ptr<A> synthese::server::ActionFunctionRequest<A, F>::getAction()
		{
			return boost::static_pointer_cast<A, Action> (_getAction());
		}

		template<class A, class F>
		boost::shared_ptr<const A> synthese::server::ActionFunctionRequest<A, F>::getAction() const
		{
			return boost::static_pointer_cast<const A, const Action> (_getAction());
		}

		template<class A, class F>
		ActionFunctionRequest<A, F>::ActionFunctionRequest(const Request* request)
			: FunctionRequest<F>(request)
		{
			_setAction(util::Factory<Action>::createSharedPtr<A>());
		}
	}
}

#endif // SYNTHESE_ActionFunctionRequest_h__
