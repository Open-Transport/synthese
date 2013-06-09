
//////////////////////////////////////////////////////////////////////////
/// InterSYNTHESEPackageAutoLockAction class header.
///	@file InterSYNTHESEPackageAutoLockAction.hpp
///	@author hromain
///	@date 2013
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

#ifndef SYNTHESE_InterSYNTHESEPackageAutoLockAction_H__
#define SYNTHESE_InterSYNTHESEPackageAutoLockAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/thread/mutex.hpp>

namespace synthese
{
	namespace inter_synthese
	{
		class InterSYNTHESEPackage;

		//////////////////////////////////////////////////////////////////////////
		/// 19.15 Action : InterSYNTHESEPackageAutoLockAction.
		/// @ingroup m19Actions refActions
		///	@author hromain
		///	@date 2013
		/// @since 3.9.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : InterSYNTHESEPackageAutoLockAction
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamid</dt><dd>id of the object to update</dd>
		///	</dl>
		class InterSYNTHESEPackageAutoLockAction:
			public util::FactorableTemplate<server::Action, InterSYNTHESEPackageAutoLockAction>
		{
		public:
			static const std::string PARAMETER_PACKAGE_ID;
			static const std::string PARAMETER_LOCK;

		private:
			boost::shared_ptr<boost::mutex::scoped_lock> _lockMutex;
			bool _lock;
			InterSYNTHESEPackage* _package;

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
			InterSYNTHESEPackageAutoLockAction();



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
				// void setObject(boost::shared_ptr<Object> value) { _object = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_InterSYNTHESEPackageAutoLockAction_H__

