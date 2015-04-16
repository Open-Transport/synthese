////////////////////////////////////////////////////////////////////////////////
/// Action class header.
///	@file Action.h
///	@author Hugues Romain
///	@date 2008-12-26 17:58
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

#ifndef SYNTHESE_Action_H__
#define SYNTHESE_Action_H__

#include "FactoryBase.h"
#include "FunctionAPI.h"
#include "Env.h"

#define Action_PARAMETER_PREFIX std::string("actionParam")

////////////////////////////////////////////////////////////////////
/// @defgroup refActions Actions
///	@ingroup ref

namespace synthese
{
	namespace security
	{
		class Profile;
	}

	namespace util
	{
		class ParametersMap;
	}

	namespace server
	{
		class ActionException;
		class Request;
		class Session;

		//////////////////////////////////////////////////////////////////////////
		/// Action abstract class to run before the display of a function result.
		///	The server module contains a factory of actions.
		///
		///	@ingroup m15
		//////////////////////////////////////////////////////////////////////////
		class Action
		:	public util::FactoryBase<Action>
		{
		protected:
			boost::shared_ptr<util::Env>	_env;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			//////////////////////////////////////////////////////////////////////////
			Action(
				boost::shared_ptr<util::Env> env = boost::shared_ptr<util::Env>(new util::Env())
			):	util::FactoryBase<Action>(),
				_env(env)
			{
			}


			/** Authorization control.
				@return True if the action run is authorized
				@author Hugues Romain
				@date 2007
			*/
			virtual bool isAuthorized(
				const server::Session* session
			) const = 0;

			/** Conversion from generic parameters map to attributes.
				@param map Map to analyse
			*/
			virtual void _setFromParametersMap(const util::ParametersMap& map) = 0;

			/** Conversion from attributes to generic parameter maps.
			*/
			virtual util::ParametersMap getParametersMap() const = 0;


			//////////////////////////////////////////////////////////////////////////
			/// Action to run, defined by each subclass.
			///	@return the id of the object created by the action, if any,
			virtual void run(
				Request& request
			) = 0;

			boost::shared_ptr<util::Env> getEnv() const { return _env; }
			void setEnv(boost::shared_ptr<util::Env> value) { _env = value; }

			///
			/// \brief getAPI
			/// \return the API of this Action
			///
			virtual FunctionAPI getAPI() const
			{
				return FunctionAPI();
			}
		};
	}
}

#endif // SYNTHESE_Action_H__
