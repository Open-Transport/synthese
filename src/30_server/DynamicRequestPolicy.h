
/** DynamicRequestPolicy class header.
	@file DynamicRequestPolicy.h

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

#ifndef SYNTHESE_server_DynamicRequestPolicy_h__
#define SYNTHESE_server_DynamicRequestPolicy_h__

#include "StaticRequestPolicy.h"
#include "Request.h"

namespace synthese
{
	namespace server
	{
		class Action;
		class Function;

		/** DynamicRequestPolicy class.
			@ingroup m15
		*/
		class DynamicRequestPolicy
		{
		public:
			template<class T>
			class BadTypeException:
				public util::Exception
			{
				BadTypeException(const std::string& factoryKey):
					util::Exception("Bad type conversion on DynamicRequestPolicy : " + factoryKey + " was attempted to be converted into "+ T::FACTORY_KEY)
				{
				}
			};

			typedef Action ActionType;
			typedef Function FunctionType;


		private:
			boost::shared_ptr<Action> _action;
			boost::shared_ptr<Function> _function;
			ParametersMap _parametersMap;
			
		protected:
			template<class ActionT, class FunctionT>
			DynamicRequestPolicy(const StaticActionFunctionRequestPolicy<ActionT,FunctionT>& policy):
				_action(policy._getAction()),
				_function(policy._getFunction())
			{

			}

			template<class ActionT>
			DynamicRequestPolicy(const StaticActionRequestPolicy<ActionT>& policy):
				_action(policy._getAction())
			{
			}

			template<class FunctionT>
			DynamicRequestPolicy(const StaticFunctionRequestPolicy<FunctionT>& policy):
				_function(policy._getFunction())
			{
			}

			void _loadFunction(
				bool actionException,
				const std::string& errorMessage,
				boost::optional<util::RegistryKeyType> actionCreatedId
			);
			void _loadAction();

			void _setParametersMap(const ParametersMap& value) { _parametersMap = value; }

			boost::shared_ptr<Action> _getAction() { return _action; }
			boost::shared_ptr<Function> _getFunction() { return _function; }
			boost::shared_ptr<const Action> _getAction() const { return boost::const_pointer_cast<const Action>(_action); }
			boost::shared_ptr<const Function> _getFunction() const { return boost::const_pointer_cast<const Function>(_function); }

			template<class ActionT>
			boost::shared_ptr<ActionT> getAction()
			{
				if(_action.get() && _action->getFactoryKey() != ActionT::FACTORY_KEY)
				{
					throw BadTypeException<ActionT>(_action->getFactoryKey());
				}
				return boost::static_pointer_cast<ActionT, Action>(_action);
			}

			template<class FunctionT>
			boost::shared_ptr<FunctionT> getFunction()
			{
				if(_function.get() && _function->getFactoryKey() != FunctionT::FACTORY_KEY)
				{
					throw BadTypeException<ActionT>(_function->getFactoryKey());
				}
				return boost::static_pointer_cast<FunctionT, Function>(_action);
			}

			template<class ActionT>
			boost::shared_ptr<const ActionT> getAction() const
			{
				if(_action.get() && _action->getFactoryKey() != ActionT::FACTORY_KEY)
				{
					throw BadTypeException<ActionT>(_action->getFactoryKey());
				}
				return boost::static_pointer_cast<const ActionT, Action>(_action);
			}

			template<class FunctionT>
			boost::shared_ptr<const FunctionT> getFunction() const
			{
				if(_function.get() && _function->getFactoryKey() != FunctionT::FACTORY_KEY)
				{
					throw BadTypeException<ActionT>(_function->getFactoryKey());
				}
				return boost::static_pointer_cast<const FunctionT, Function>(_action);
			}

			void _deleteAction()
			{
				_action.reset();
			}
		};
	}
}

#endif // SYNTHESE_server_DynamicRequestPolicy_h__
