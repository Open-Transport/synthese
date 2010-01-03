
/** StaticFunctionRequest class header.
	@file StaticFunctionRequest.h

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

#ifndef SYNTHESE_server_StaticFunctionRequest_h__
#define SYNTHESE_server_StaticFunctionRequest_h__

#include "Request.h"
#include "Function.h"

namespace synthese
{
	namespace server
	{
		/** StaticFunctionRequest class.
			@ingroup m15
		*/
		template<class FunctionT>
		class StaticFunctionRequest:
			public virtual Request
		{
		public:
			StaticFunctionRequest():
				Request()
			{
				_function = boost::shared_ptr<Function>(new FunctionT);
			}


			StaticFunctionRequest(const Request& request, bool copyFunction = true):
			Request(request)
			{
				if(!request.getFunction().get())
				{
					throw util::Exception("The source request does not allow copy");
				}
				if (FunctionT::FACTORY_KEY == request.getFunction()->getFactoryKey())
				{
					if(copyFunction)
					{
						_function = boost::shared_ptr<Function>(new FunctionT);
						_function->_copy(request.getFunction());
					}
					else
					{
						_function = boost::static_pointer_cast<FunctionT>(boost::const_pointer_cast<Function>(request.getFunction()));
					}
				}
				else
				{
					_function = boost::shared_ptr<Function>(new FunctionT);
				}
				_function->setEnv(request.getFunction()->getEnv());
			}



			boost::shared_ptr<FunctionT> getFunction() { return boost::static_pointer_cast<FunctionT>(_function); }
			boost::shared_ptr<const FunctionT> getFunction() const { return boost::static_pointer_cast<const FunctionT>(_function); }
		};
	}
}

#endif // SYNTHESE_server_StaticFunctionRequest_h__
