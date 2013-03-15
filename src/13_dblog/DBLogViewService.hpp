
//////////////////////////////////////////////////////////////////////////////////////////
///	DBLogViewService class header.
///	@file DBLogViewService.hpp
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

#ifndef SYNTHESE_DBLogViewService_H__
#define SYNTHESE_DBLogViewService_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include "DBLogEntry.h"

namespace synthese
{
	namespace dblog
	{
		class DBLog;

		//////////////////////////////////////////////////////////////////////////
		///	13.15 Function : DBLogViewService.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m13Functions refFunctions
		///	@author hromain
		///	@date 2013
		/// @since 3.6.0
		class DBLogViewService:
			public util::FactorableTemplate<server::Function,DBLogViewService>
		{
		public:
			static const std::string PARAMETER_LOG_KEY;
			static const std::string PARAMETER_FIRST;
			static const std::string PARAMETER_NUMBER;
			static const std::string PARAMETER_UP_SORT;
			static const std::string PARAMETER_SEARCH_USER;
			static const std::string PARAMETER_SEARCH_TYPE;
			static const std::string PARAMETER_START_DATE;
			static const std::string PARAMETER_END_DATE;
			static const std::string PARAMETER_SEARCH_TEXT;
			static const std::string PARAMETER_OBJECT_ID;
			static const std::string PARAMETER_OBJECT_ID2;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<DBLog>		_dbLog;
				boost::posix_time::ptime		_searchStartDate;
				boost::posix_time::ptime		_searchEndDate;
				DBLogEntry::Level				_searchLevel;
				boost::optional<util::RegistryKeyType>			_searchUserId;
				std::string						_searchText;
				boost::optional<util::RegistryKeyType>			_searchObjectId;
				boost::optional<util::RegistryKeyType>			_searchObjectId2;
				size_t _first;
				size_t _number;
				bool _upSort;
			//@}
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author hromain
			/// @date 2013
			/// @since 3.6.0
			util::ParametersMap _getParametersMap() const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author hromain
			/// @date 2013
			/// @since 3.6.0
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);
			
			
		public:
			DBLogViewService();


			//! @name Setters
			//@{
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author hromain
			/// @date 2013
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author hromain
			/// @date 2013
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author hromain
			/// @date 2013
			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_DBLogViewService_H__

