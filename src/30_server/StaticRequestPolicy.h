
/** StaticRequestPolicy class header.
	@file StaticRequestPolicy.h

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

#ifndef SYNTHESE_server_StaticRequestPolicy_h__
#define SYNTHESE_server_StaticRequestPolicy_h__

#include "Action.h"
#include "Function.h"

namespace synthese
{
	namespace server
	{
		class Action;

		/** StaticRequestPolicy class.
			@ingroup m15
		*/
		template<class ActionT>
		class StaticActionRequestPolicy
		{
		public:
			typedef ActionT ActionType;

		private:
			boost::shared_ptr<ActionT> _action;

		public:
			StaticActionRequestPolicy():
				_action(new ActionT)
			{
			}

			StaticActionRequestPolicy(const Request& request):
				_action(new ActionT)
			{
				if(request._getAction().get())
				{
					_action->setEnv(request._getAction()->getEnv());
				}
				else if(request._getFunction().get())
				{
					_action->setEnv(request._getFunction()->getEnv());
				}
				else
				{
					throw util::Exception("The source request does not allow copy");
				}
			}


			template<class T>
			StaticActionRequestPolicy(
				const StaticActionRequestPolicy<T>& request
				):	_action(new ActionT)
			{
				if(request._getAction().get())
				{
					_action->setEnv(request._getAction()->getEnv());
				}
				else if(request._getFunction().get())
				{
					_action->setEnv(request._getFunction()->getEnv());
				}
				else
				{
					throw util::Exception("The source request does not allow copy");
				}
			}

			boost::shared_ptr<ActionT> getAction() { return _action; }
			boost::shared_ptr<const ActionT> getAction() const { return boost::const_pointer_cast<const ActionT>(_action); }
			boost::shared_ptr<Action> _getAction() { return boost::static_pointer_cast<Action, ActionT>(_action); }
			boost::shared_ptr<const Action> _getAction() const { return boost::static_pointer_cast<const Action, ActionT>(_action); }

			boost::shared_ptr<Function> _getFunction() { return boost::shared_ptr<Function>(); }
			boost::shared_ptr<const Function> _getFunction() const { return boost::shared_ptr<const Function>(); }

			void _loadFunction(
				bool actionException,
				const std::string& errorMessage,
				boost::optional<util::RegistryKeyType> actionCreatedId
			){}
			void _loadAction(){}

			void _deleteAction()
			{
				_action.reset();
			}
		};


		template<class FunctionT>
		class StaticFunctionRequestPolicy
		{
		public:
			typedef FunctionT FunctionType;

		private:
			boost::shared_ptr<FunctionT> _function;

		public:
			StaticFunctionRequestPolicy():
				_function(new FunctionT)
			{
			}

			StaticFunctionRequestPolicy(const Request& request):
				_function(new FunctionT)
			{
				if(!request._getFunction().get())
				{
					throw util::Exception("The source request does not allow copy");
				}
				if (FunctionT::FACTORY_KEY == request._getFunction()->getFactoryKey())
				{
					_function->_copy(request._getFunction());
				}
				_function->setEnv(request._getFunction()->getEnv());
			}

			boost::shared_ptr<FunctionT> getFunction() { return _function; }
			boost::shared_ptr<const FunctionT> getFunction() const { return boost::const_pointer_cast<const FunctionT>(_function); }
			boost::shared_ptr<Function> _getFunction() { return boost::static_pointer_cast<Function, FunctionT>(_function); }
			boost::shared_ptr<const Function> _getFunction() const { return boost::static_pointer_cast<const Function, FunctionT>(_function); }

			boost::shared_ptr<Action> _getAction() { return boost::shared_ptr<Action>(); }
			boost::shared_ptr<const Action> _getAction() const { return boost::shared_ptr<const Action>(); }

			void _loadFunction(
				bool actionException,
				const std::string& errorMessage,
				boost::optional<util::RegistryKeyType> actionCreatedId
			){}
			void _loadAction(){}

			void _deleteAction()
			{
			}
		};


		template<class ActionT, class FunctionT>
		class StaticActionFunctionRequestPolicy:
			public StaticActionRequestPolicy<ActionT>,
			public StaticFunctionRequestPolicy<FunctionT>
		{
		public:
			StaticActionFunctionRequestPolicy():
				StaticActionRequestPolicy<ActionT>(),
				StaticFunctionRequestPolicy<FunctionT>()
			{
			}



			StaticActionFunctionRequestPolicy(const Request& request):
				StaticActionRequestPolicy<ActionT>(request),
				StaticFunctionRequestPolicy<FunctionT>(request)
			{
			}

			boost::shared_ptr<Action> _getAction() { return StaticActionRequestPolicy<ActionT>::_getAction(); }
			boost::shared_ptr<const Action> _getAction() const { return StaticActionRequestPolicy<ActionT>::_getAction(); }
			boost::shared_ptr<Function> _getFunction() { return StaticFunctionRequestPolicy<FunctionT>::_getFunction(); }
			boost::shared_ptr<const Function> _getFunction() const { return StaticFunctionRequestPolicy<FunctionT>::_getFunction(); }
			void _deleteAction()	{ StaticActionRequestPolicy<ActionT>::_deleteAction(); }
			void _loadFunction(
				bool actionException,
				const std::string& errorMessage,
				boost::optional<util::RegistryKeyType> actionCreatedId
			){}
			void _loadAction(){}
		};
	}
}

#endif // SYNTHESE_server_StaticRequestPolicy_h__
