////////////////////////////////////////////////////////////////////////////////
///	DBLogHTMLView class header.
///	@file DBLogHTMLView.h
///	@author Hugues Romain (RCS)
///	@date jeu jan 29 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_DBLogHTMLView_h__
#define SYNTHESE_DBLogHTMLView_h__

#include "DateTime.h"
#include "DBLogEntry.h"
#include "ResultHTMLTable.h"
#include "Env.h"


namespace synthese
{
	namespace server
	{
		class Request;
		class ParametersMap;
	}
	
	namespace dblog
	{
		class DBLog;
		
		////////////////////////////////////////////////////////////////////////
		/// DBLogHTMLView class.
		/// @ingroup m13
		class DBLogHTMLView
		{
		public:
			// Typedefs
			
			static const std::string PARAMETER_PREFIX;
			static const std::string PARAMETER_SEARCH_USER;
			static const std::string PARAMETER_SEARCH_TYPE;
			static const std::string PARAMETER_START_DATE;
			static const std::string PARAMETER_END_DATE;
			static const std::string PARAMETER_SEARCH_TEXT;
			static const std::string PARAMETER_OBJECT_ID;

			
		private:
			// Attributes
			const std::string				_code;
			mutable util::Env				_env;
			boost::shared_ptr<DBLog>		_dbLog;
			time::DateTime					_searchStartDate;
			time::DateTime					_searchEndDate;
			DBLogEntry::Level				_searchLevel;
			util::RegistryKeyType			_searchUserId;
			std::string						_searchText;
			util::RegistryKeyType			_searchObjectId;
			html::ResultHTMLTable::RequestParameters	_requestParameters;
			html::ResultHTMLTable::ResultParameters		_resultParameters;
			
			std::string _getParameterName(const std::string& parameter) const;
		public:
			/////////////////////////////////////////////////////////////////////
			/// DBLogHTMLView Constructor.
			DBLogHTMLView(
				const std::string& code
			);


			
			//! @name Getters
			//@{
			//@}
		
			//! @name Setters
			//@{
				/** Global Modifier.
				* 
				* @param map Map containing the parameters described by the user query
				* @param code Code of the DBLog viewer (useless if only one viewer in a page)
				* @param logKey log key to override the map
				* @param searchStartDate 
				* @param searchEndDate 
				* @param searchLevel 
				* @param searchUserId 
				* @param searchText 
				* @param searchObjectId 
				*/
				void set(
					const server::ParametersMap& map,
					std::string logKey,
					time::DateTime searchStartDate = time::TIME_UNKNOWN,
					time::DateTime searchEndDate = time::TIME_UNKNOWN,
					DBLogEntry::Level searchLevel = DBLogEntry::DB_LOG_UNKNOWN,
					util::RegistryKeyType searchUserId = UNKNOWN_VALUE,
					std::string searchText = std::string(),
					util::RegistryKeyType searchObjectId = UNKNOWN_VALUE
				);
			//@}

			//! @name Modifiers
			//@{
			//@}

			//! @name Queries
			//@{
				void display(
					std::ostream& stream,
					const server::Request& searchRequest,
					bool withForm = true,
					bool withLinkToAdminPage = false
				) const;
			//@}
		
			//! @name Static algorithms
			//@{
			//@}
		};
	}
}

#endif // SYNTHESE_DBLogHTMLView_h__
