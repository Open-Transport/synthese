
/** AccountAddAction class header.
	@file AccountAddAction.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_AccountAddAction_H__
#define SYNTHESE_AccountAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace accounts
	{
		class Currency;

		/** AccountAddAction action class.
			@ingroup m57Actions refActions
		*/
		class AccountAddAction
			: public util::FactorableTemplate<server::Action, AccountAddAction>
		{
		public:
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_CLASS;
			static const std::string PARAMETER_AUTO_INCREMENT_CLASS;
			static const std::string PARAMETER_CURRENCY;
			static const std::string PARAMETER_RIGHT_USER_ID;

		private:
			std::string								_name;
			std::string								_class;
			bool									_autoIncrementClass;
			boost::shared_ptr<const Currency>		_currency;
			boost::shared_ptr<const security::User>	_rightUser;

		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			AccountAddAction();

			void setClass(const std::string& value);
			void setCurrency(boost::shared_ptr<const Currency> value);
			void setAutoIncrementClass(bool value);
			void setRightUser(boost::shared_ptr<const security::User> value);
		};
	}
}

#endif // SYNTHESE_AccountAddAction_H__
