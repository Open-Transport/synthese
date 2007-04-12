
/** FunctionRequest class header.
	@file FunctionRequest.h

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

#ifndef SYNTHESE_FunctionRequest_h__
#define SYNTHESE_FunctionRequest_h__

#include "30_server/Request.h"

#include "01_util/Factory.h"

namespace synthese
{
	namespace server
	{
		/** Template for a Request containing a function and no action.
			@ingroup m30
		*/
		template<class F>
		class FunctionRequest : public Request
		{
		public:
			FunctionRequest(const Request* request=NULL);

			boost::shared_ptr<F> getFunction();
			boost::shared_ptr<const F> getFunction() const;
		};

		template<class F>
		FunctionRequest<F>::FunctionRequest(const Request* request)
			: Request(request, util::Factory<Function>::create<F>())
		{
		}

		template<class F>
		boost::shared_ptr<F> FunctionRequest<F>::getFunction()
		{
			return boost::static_pointer_cast<F, Function> (_getFunction());
		}

		template<class F>
		boost::shared_ptr<const F> FunctionRequest<F>::getFunction() const
		{
			return boost::static_pointer_cast<const F, const Function> (_getFunction());
		}
	}
}

#endif // SYNTHESE_FunctionRequest_h__
