////////////////////////////////////////////////////////////////////////////////
///	DBLogDynamicViewFunction class header.
///	@file DBLogDynamicViewFunction.h
///	@date 04/06/2014
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

#ifndef SYNTHESE_DBLogDynamicViewFunction_h__
#define SYNTHESE_DBLogDynamicViewFunction_h__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace dblog
	{
		class DBLog;
	}

	namespace resa
	{
		/** DBLogDynamicViewFunction class
			@ingroup m51Functions refFunctions
			This function is called by AJAX requests and works directly with the /lib/synthese/logEntries.js
			Its goal is to display reservations log entries splited by different tabs without
			slowing the administration interface when operators create reservations.
			If parameter count is true, it will ask for all the user's entries and send back occurences number for
			each defined types (reservation,cancel,missing...).
			If called with an entryType, it will get precisely the related entries from database in order to not
			slow down the web interface.
		*/
		class DBLogDynamicViewFunction:
			public util::FactorableTemplate<server::Function, DBLogDynamicViewFunction>
		{
		public:
			// Typedefs
			static const std::string PARAMETER_USER;
			static const std::string PARAMETER_ENTRYTYPE;
			static const std::string PARAMETER_GETCOUNT;
			static const std::string PARAMETER_FIRST;
			static const std::string PARAMETER_PAGE_ID;

			static const std::string FILTER_ALL;
			static const std::string FILTER_RESERVATION;
			static const std::string FILTER_CANCEL;
			static const std::string FILTER_CANCEL_DEADLINE;
			static const std::string FILTER_MISSING;

		private:
			boost::shared_ptr<const security::User>			_user;
			bool											_count;
			int												_first;
			std::string										_entryType;
			boost::shared_ptr<dblog::DBLog>					_dbLog;

		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			util::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception FunctionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;

			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_DBLogDynamicViewFunction_h__
