
/** AccountLockAction class header.
	@file AccountLockAction.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_AccountLockAction_H__
#define SYNTHESE_AccountLockAction_H__

#include "30_server/Action.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace accounts
	{
		class Account;

		/** AccountLockAction action class.
			@ingroup m37Actions refActions
		*/
		class AccountLockAction
			: public util::FactorableTemplate<server::Action, AccountLockAction>
		{
		public:

		private:
			boost::shared_ptr<Account>	_account;

		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run();
			
			AccountLockAction();
		};
	}
}

#endif // SYNTHESE_AccountLockAction_H__
