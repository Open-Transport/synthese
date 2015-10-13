////////////////////////////////////////////////////////////////////////////////
/// SecurityPolicy class header.
///	@file SecurityPolicy.h
///	@author Marc Jambert
///	@date 2015-10-12 14:00
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_SecurityPolicy_h__
#define SYNTHESE_SecurityPolicy_h__

#include <boost/shared_ptr.hpp>
#include "ParametersMap.h"
#include "Session.h"

namespace synthese
{

	namespace server
	{

		/** SecurityPolicy class.
			@ingroup m15

			A security policy defines an interface to determine at runtime whether or not a Session is
			allowed to use something (a function, or something else with a key).

		*/
		class SecurityPolicy
		{
		private:

			static const SecurityPolicy* INSTANCE;

		protected:

			SecurityPolicy() {}
			virtual ~SecurityPolicy() {}

		public:

			static const SecurityPolicy& GetInstance();
			static void SetInstance(const SecurityPolicy&);

			virtual bool isAuthorized(const std::string& key, const Session* session) const = 0;

		};
}	}

#endif // SYNTHESE_SecurityPolicy_h__
