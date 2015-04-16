
/** AddRightAction class header.
	@file AddRightAction.h

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

#ifndef SYNTHESE_AddRightAction_H__
#define SYNTHESE_AddRightAction_H__

#include <boost/shared_ptr.hpp>

#include "SecurityTypes.hpp"
#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace security
	{
		class Profile;

		/** AddRightAction action class.
			@ingroup m12Actions refActions
		*/
		class AddRightAction:
			public util::FactorableTemplate<server::Action, AddRightAction>
		{
		public:
			static const std::string PARAMETER_PROFILE_ID;
			static const std::string PARAMETER_RIGHT;
			static const std::string PARAMETER_PUBLIC_LEVEL;
			static const std::string PARAMETER_PRIVATE_LEVEL;
			static const std::string PARAMETER_PARAMETER;

		private:
			boost::shared_ptr<Profile>		_profile;
			std::string		_rightName;
			std::string		_parameter;
			RightLevel	_privateLevel;
			RightLevel	_publicLevel;

		protected:
			/** Conversion from attributes to generic parameter maps.
				@warning Not yet implemented
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(
				server::Request& request
			);

			virtual bool isAuthorized(
				const server::Session* session
			) const;

			void setProfile(boost::shared_ptr<Profile> value);
			void setProfile(boost::shared_ptr<const Profile> value);
		};
	}
}

#endif // SYNTHESE_AddRightAction_H__
