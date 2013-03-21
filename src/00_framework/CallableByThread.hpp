
/** CallableByThread class header.
	@file CallableByThread.hpp

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

#ifndef SYNTHESE_framework_CallableByThread_hpp__
#define SYNTHESE_framework_CallableByThread_hpp__

#include "Factory.h"
#include "ModuleClass.h"

#include <boost/foreach.hpp>

namespace synthese
{
	/** CallableByThread class.
		@ingroup m00
	*/
	template<class Callable>
	class CallableByThread
	{
		Callable _func;

	public:
		CallableByThread(Callable func):
		  _func(func) {}


		void operator()()
		{
			// Launch each module opening trigger
			BOOST_FOREACH(
				const boost::shared_ptr<server::ModuleClass>& module,
				util::Factory<server::ModuleClass>::GetNewCollection()
			){
				module->initThread();
			}

			try
			{
				// Launch the function
				_func();
			}
			catch(boost::thread_interrupted)
			{
			}
			catch(...)
			{
			}


			// Launch each module closing trigger
			BOOST_FOREACH(
				const boost::shared_ptr<server::ModuleClass>& module,
				util::Factory<server::ModuleClass>::GetNewCollection()
			){
				module->closeThread();
			}
		}

	};
}

#endif // SYNTHESE_framework_CallableByThread_hpp__

