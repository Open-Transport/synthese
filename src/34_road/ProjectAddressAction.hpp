
//////////////////////////////////////////////////////////////////////////
/// ProjectAddressAction class header.
///	@file ProjectAddressAction.hpp
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

#ifndef SYNTHESE_ProjectAddressAction_H__
#define SYNTHESE_ProjectAddressAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace geos
{
	namespace geom
	{
		class Point;
	}
}

namespace synthese
{
	namespace util
	{
		class Registrable;
	}

	namespace road
	{
		//////////////////////////////////////////////////////////////////////////
		/// 34.15 Action : ProjectAddressAction.
		/// @ingroup m34Actions refActions
		///	@author Hugues Romain
		///	@date 2012
		/// @since 3.4.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : ProjectAddressAction
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamid</dt><dd>id of the object to update</dd>
		///	</dl>
		class ProjectAddressAction:
			public util::FactorableTemplate<server::Action, ProjectAddressAction>
		{
		public:
			static const std::string PARAMETER_ADDRESS_ID;
			static const std::string PARAMETER_MAX_DISTANCE;

		private:
			boost::shared_ptr<util::Registrable> _address;
			boost::shared_ptr<geos::geom::Point> _point;
			double _maxDistance;

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
				template<class T>
				void setAddress(boost::shared_ptr<T> value) { _address = boost::dynamic_pointer_cast<util::Registrable, T>(value); }

				void setMaxDistance(double value){ _maxDistance = value; }
			//@}

			ProjectAddressAction();
		};
	}
}

#endif // SYNTHESE_ProjectAddressAction_H__

