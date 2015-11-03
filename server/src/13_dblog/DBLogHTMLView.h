////////////////////////////////////////////////////////////////////////////////
///	DBLogHTMLView class header.
///	@file DBLogHTMLView.h
///	@author Hugues Romain (RCS)
///	@date jeu jan 29 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_DBLogHTMLView_h__
#define SYNTHESE_DBLogHTMLView_h__

#include "DBLogEntry.h"
#include "ResultHTMLTable.h"
#include "Env.h"


namespace synthese
{
	namespace security
	{
		class Profile;
	}

	namespace server
	{
		class Request;
	}

	namespace util
	{
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
			static const std::string PARAMETER_OBJECT_ID2;

			static const std::string FILTER_ALL;
			static const std::string FILTER_RESA;
			static const std::string FILTER_ABS;
			static const std::string FILTER_CANCEL;
			static const std::string FILTER_CANC_D;

		private:
			// Attributes
			const std::string				_code;
			mutable util::Env				_env;
			boost::shared_ptr<DBLog>		_dbLog;
			boost::posix_time::ptime		_searchStartDate;
			bool							_fixedStartDate;
			boost::posix_time::ptime		_searchEndDate;
			bool							_fixedEndDate;
			Level							_searchLevel;
			bool							_fixedLevel;
			boost::optional<util::RegistryKeyType>			_searchUserId;
			bool							_fixedUserId;
			std::string						_searchText;
			bool							_fixedText;
			boost::optional<util::RegistryKeyType>			_searchObjectId;
			boost::optional<util::RegistryKeyType>			_searchObjectId2;
			bool							_fixedObjectId;
			bool							_fixedObjectId2;
			html::ResultHTMLTable::RequestParameters	_requestParameters;

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
			//@}

			//! @name Modifiers
			//@{
				void setLogKey(const std::string& value);

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
					const util::ParametersMap& map,
					std::string logKey,
					boost::optional<util::RegistryKeyType> searchObjectId = boost::optional<util::RegistryKeyType>(),
					boost::optional<util::RegistryKeyType> searchObjectId2 = boost::optional<util::RegistryKeyType>(),
					boost::optional<util::RegistryKeyType> searchUserId = boost::optional<util::RegistryKeyType>(),
					Level searchLevel = DB_LOG_UNKNOWN,
					boost::posix_time::ptime searchStartDate = boost::posix_time::ptime(boost::posix_time::not_a_date_time),
					boost::posix_time::ptime searchEndDate = boost::posix_time::ptime(boost::posix_time::not_a_date_time),
					std::string searchText = std::string()
				);
			//@}

			//! @name Queries
			//@{
				std::string getLogKey() const;
				std::string getLogName() const;

				void display(
					std::ostream& stream,
					const server::Request& searchRequest,
					bool withForm = true,
					bool withLinkToAdminPage = false,
					const std::string& type = FILTER_ALL
				) const;

				bool isAuthorized(
					const security::Profile& profile
				) const;

				util::ParametersMap getParametersMap(
				) const;
			//@}

			//! @name Static algorithms
			//@{
			//@}
		};
	}
}

#endif // SYNTHESE_DBLogHTMLView_h__
