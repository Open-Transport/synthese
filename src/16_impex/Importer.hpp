
/** Importer class header.
	@file Importer.hpp

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

#ifndef SYNTHESE_impex_Importer_hpp__
#define SYNTHESE_impex_Importer_hpp__

#include "ImportLogger.hpp"

#include <ostream>
#include <boost/optional.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace db
	{
		class DBTransaction;
	}

	namespace server
	{
		class Request;
	}

	namespace util
	{
		class Env;
		class ParametersMap;
	}

	namespace impex
	{
		class Import;
		class ImportLogger;

		/** Importer class.
			@ingroup m16
		*/
		class Importer
		{
		public:

			Importer(
				util::Env& env,
				const Import& import,
				const ImportLogger& logger
			);

		protected:
			util::Env&				_env;
			const Import&			_import;
			const ImportLogger&		_logger;

			virtual db::DBTransaction _save() const = 0;

			void _log(
				ImportLogger::Level level,
				const std::string& content
			) const;
			void _logError(const std::string& content) const;
			void _logWarning(const std::string& content) const;
			void _logDebug(const std::string& content) const;
			void _logInfo(const std::string& content) const;
			void _logLoad(const std::string& content) const;
			void _logCreation(const std::string& content) const;

		public:
			//! @name Getters
			//@{
				const Import& getImport() const { return _import; }
			//@}

			//! @name Setters
			//@{
			//@}

			virtual bool beforeParsing() { return true; }
			virtual bool afterParsing() { return true; }



			//////////////////////////////////////////////////////////////////////////
			/// Purge the obsolete data imported by the source
			/// @param firstDayToKeep the first day to keep
			virtual void cleanObsoleteData(
				const boost::gregorian::date& firstDayToKeep
			) const {}



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// @param map Parameters map to interpret
			/// @param doImport tests if the parameters are valid for an import
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			virtual void setFromParametersMap(
				const util::ParametersMap& map,
				bool doImport
			) = 0;

			virtual util::ParametersMap getParametersMap() const = 0;


			//////////////////////////////////////////////////////////////////////////
			/// Launches the parsing of the files, with output on an admin page.
			/// @param request
			/// @return true if it is allowed to save the data
			virtual bool parseFiles(
				boost::optional<const server::Request&> request
			) const = 0;



			//////////////////////////////////////////////////////////////////////////
			/// Interface for the save method.
			/// @return transaction to run
			db::DBTransaction save() const;
		};
}	}

#endif // SYNTHESE_impex_Importer_hpp__
