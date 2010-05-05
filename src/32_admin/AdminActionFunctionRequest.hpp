
/** AdminActionFunctionRequest class header.
	@file AdminActionFunctionRequest.hpp

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

#ifndef SYNTHESE_admin_AdminActionFunctionRequest_hpp__
#define SYNTHESE_admin_AdminActionFunctionRequest_hpp__

#include "AdminFunction.h"
#include "StaticActionFunctionRequest.h"

namespace synthese
{
	namespace admin
	{
		/** AdminActionFunctionRequest class.
			@ingroup m14
		*/
		template<class A, class P>
		class AdminActionFunctionRequest:
			public server::StaticActionFunctionRequest<A, AdminFunction>
		{
		public:
			AdminActionFunctionRequest(
				const server::StaticFunctionRequest<AdminFunction>& request
			):	server::Request(request),
				server::StaticActionFunctionRequest<A, AdminFunction>(request, true)
			{
				boost::shared_ptr<P> p(
					request.getFunction()->getPage() ?
					request.getFunction()->getPage()->template getNewOtherPage<P>() :
					boost::shared_ptr<P>(new P)
				);
				this->getFunction()->setPage(p);
				if(request.getFunction()->getPage() && P::FACTORY_KEY == request.getFunction()->getPage()->getFactoryKey())
				{
					this->getFunction()->getPage()->setActiveTab(request.getFunction()->getPage()->getCurrentTab());
				}
			}
			
			boost::shared_ptr<P> getPage() const
			{
				return boost::static_pointer_cast<P, AdminInterfaceElement>(
					this->getFunction()->getPage()
				); 
			}
		};
	}
}

#endif // SYNTHESE_admin_AdminActionFunctionRequest_hpp__

