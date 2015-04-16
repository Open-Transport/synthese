
//////////////////////////////////////////////////////////////////////////
/// ObjectUpdateAction class header.
///	@file ObjectUpdateAction.hpp
///	@author Hugues Romain
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_ObjectUpdateAction_H__
#define SYNTHESE_ObjectUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "ParametersMap.h"

#include <boost/logic/tribool.hpp>

namespace synthese
{
	class ObjectBase;

	namespace db
	{
		class DBDirectTableSync;

		//////////////////////////////////////////////////////////////////////////
		/// 10.15 Action : ObjectUpdateAction.
		/// @ingroup m10Actions refActions
		///	@author Hugues Romain
		///	@date 2012
		/// @since 3.4.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : ObjectUpdateAction
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamid</dt><dd>id of the object to update</dd>
		///	</dl>
		class ObjectUpdateAction:
			public util::FactorableTemplate<server::Action, ObjectUpdateAction>
		{
		public:
			static const std::string PARAMETER_OBJECT_ID;
			static const std::string PARAMETER_FIELD_PREFIX;

		private:
			const ObjectBase* _object;
			boost::shared_ptr<DBDirectTableSync> _tableSync;
			boost::shared_ptr<ObjectBase> _value;
			util::ParametersMap _values;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			ObjectUpdateAction();



			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);



			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;



			//! @name Setters
			//@{
				void _setObject(
					const ObjectBase& value
				);

				template<class T>
				void setObject(const T& value){
					_setObject(static_cast<const ObjectBase&>(value));
				}

				/// Simple parameters only
				template<class C>
				void set(const typename C::Type& value)
				{
					C::SaveToParametersMap(value, *_object, _values, PARAMETER_FIELD_PREFIX, boost::logic::indeterminate);
				}

				/// All parameters version
				template<class C>
				void set(
					const typename C::Type& value,
					const ObjectBase& object
				){
					C::SaveToParametersMap(value, object, _values, PARAMETER_FIELD_PREFIX, boost::logic::indeterminate);
				}
			//@}


			static std::string _GetInputName(const std::string& fieldName);

			/// For simple object fields only. Use _GetInputName for complex fields.
			template<class F>
			static std::string GetInputName()
			{
				return _GetInputName(F::FIELD.name);
			}
		};
	}
}

#endif // SYNTHESE_ObjectUpdateAction_H__

