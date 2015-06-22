
/** DynamicRequest class header.
	@file DynamicRequest.h

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

#ifndef SYNTHESE_server_DynamicRequest_h__
#define SYNTHESE_server_DynamicRequest_h__

#include "Request.h"
#include "ParametersMap.h"

namespace synthese
{
	namespace server
	{
		class Action;
		class Function;
		struct HTTPRequest;

		//////////////////////////////////////////////////////////////////////////
		/// Request which calls dynamically defined action and/or function.
		///	@ingroup m15
		class DynamicRequest:
			public virtual Request
		{
		public:
			std::string name;
			template<class T>
			class BadTypeException:
				public synthese::Exception
			{
				BadTypeException(const std::string& factoryKey):
					Exception("Bad type conversion on DynamicRequestPolicy : " + factoryKey + " was attempted to be converted into "+ T::FACTORY_KEY)
				{
				}
			};

			typedef Function FunctionType;


		private:
			util::ParametersMap _allParametersMap;
			util::ParametersMap _getPostParametersMap;

		public:
			DynamicRequest(
				const HTTPRequest& httpRequest
			);

			virtual util::ParametersMap getParametersMap() const;

		protected:
			virtual void _loadFunction(
				const std::string& errorMessage,
				boost::optional<util::RegistryKeyType> actionCreatedId
			);


/*
			template<class FunctionT>
			boost::shared_ptr<FunctionT> getFunction()
			{
				if(_function.get() && _function->getFactoryKey() != FunctionT::FACTORY_KEY)
				{
					throw BadTypeException<ActionT>(_function->getFactoryKey());
				}
				return boost::static_pointer_cast<FunctionT, Function>(_function);
			}


			template<class FunctionT>
			boost::shared_ptr<const FunctionT> getFunction() const
			{
				if(_function.get() && _function->getFactoryKey() != FunctionT::FACTORY_KEY)
				{
					throw BadTypeException<ActionT>(_function->getFactoryKey());
				}
				return boost::static_pointer_cast<const FunctionT, Function>(_function);
			}
*/



			virtual void _loadAction();


/*
			template<class ActionT>
			boost::shared_ptr<ActionT> getAction()
			{
				if(_action.get() && _action->getFactoryKey() != ActionT::FACTORY_KEY)
				{
					throw BadTypeException<ActionT>(_action->getFactoryKey());
				}
				return boost::static_pointer_cast<ActionT, Action>(_action);
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
*/
		private:
			void _setupSession();
		};

	}
}

#endif // SYNTHESE_server_DynamicRequestPolicy_h__
