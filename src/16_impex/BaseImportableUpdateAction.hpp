
//////////////////////////////////////////////////////////////////////////
/// BaseImportableUpdateAction class header.
///	@file BaseImportableUpdateAction.hpp
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

#ifndef SYNTHESE_BaseImportableUpdateAction_H__
#define SYNTHESE_BaseImportableUpdateAction_H__

#include "Importable.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace util
	{
		class Env;
		class ParametersMap;
	}

	namespace server
	{
		class Request;
	}

	namespace impex
	{
		//////////////////////////////////////////////////////////////////////////
		/// Base class for importable object update actions.
		/// To use it in an action class :
		///		- include this file in the action header
		///			@code #include "BaseImportableUpdateAction.hpp" @endcode
		///		- inherit public from this class
		///			@code public calendar::BaseImportableUpdateAction @endcode
		///		- in getParametersMap paste somewhere :
		///			@code _getImportableUpdateParametersMap(map); @endcode
		///		- in _setFromParametersMap paste somewhere :
		///			@code _setImportableUpdateFromParametersMap(*_env, map); @endcode
		///		- in run paste somewhere before the Save call (replace _object by the real attribute) :
		///			@code _doImportableUpdate(*_object, request); @endcode
		///		- be careful not to use same parameters name as defined in BaseImportableUpdateAction
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m16
		///	@author Hugues Romain
		///	@date 2012
		/// @since 3.3.0
		class BaseImportableUpdateAction
		{
		public:
			static const std::string PARAMETER_DATA_SOURCE_LINKS;

		private:
			boost::optional<impex::Importable::DataSourceLinks> _dataSourceLinks;


		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			void _getImportableUpdateParametersMap(
				util::ParametersMap& map
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param env the environment to populate when loading objects
			/// @param map Parameters map to read
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setImportableUpdateFromParametersMap(
				util::Env& env,
				const util::ParametersMap& map
			);


		public:
			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param object the object to update
			/// @param request the request which has launched the action
			void _doImportableUpdate(
				Importable& object,
				server::Request& request
			) const;



			//! @name Setters
			//@{
				void setDataSourceLinks(boost::optional<Importable::DataSourceLinks> value){ _dataSourceLinks = value; }
			//@}
		};
}	}

#endif // SYNTHESE_BaseImportableUpdateAction_H__
