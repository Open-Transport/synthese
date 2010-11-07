
//////////////////////////////////////////////////////////////////////////
/// DataSourceUpdateAction class header.
///	@file DataSourceUpdateAction.hpp
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#ifndef SYNTHESE_DataSourceUpdateAction_H__
#define SYNTHESE_DataSourceUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace impex
	{
		class DataSource;

		//////////////////////////////////////////////////////////////////////////
		/// 16.15 Action : DataSourceUpdateAction.
		/// @ingroup m16Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.2.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : DataSourceUpdateAction
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamid : id of the data source to update</li>
		///		<li>actionParamna : new value for the name of the source : must be non empty</li>
		///		<li>actionParamfo : new value for the format of the source : must be empty or belong to the factory keys of FileFormat</li>
		///		<li>actionParamic : new value for the icon of the source</li>
		///		<li>actionParamcs : new value for the characters set of the source</li>
		///	</ul>
		class DataSourceUpdateAction:
			public util::FactorableTemplate<server::Action, DataSourceUpdateAction>
		{
		public:
			static const std::string PARAMETER_DATA_SOURCE;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_FORMAT;
			static const std::string PARAMETER_ICON;
			static const std::string PARAMETER_CHARSET;

		private:
			boost::shared_ptr<DataSource> _dataSource;
			std::string _name;
			std::string _format;
			std::string _icon;
			std::string _charset;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			server::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const server::ParametersMap& map);

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
				void setDataSource(boost::shared_ptr<DataSource> value) { _dataSource = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_DataSourceUpdateAction_H__
