
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

#include "ImpExTypes.hpp"
#include "ImportableTableSync.hpp"

#include <ostream>
#include <fstream>
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

		/** Importer class.
			@ingroup m16
		*/
		class Importer
		{
		public:

			static const std::string ATTR_IMPORT_START_TIME;
			static const std::string TAG_LOG_ENTRY;
			static const std::string ATTR_LEVEL;
			static const std::string ATTR_TEXT;

			Importer(
				util::Env& env,
				const Import& import,
				ImportLogLevel minLogLevel,
				const std::string& logPath,
				boost::optional<std::ostream&> outputStream,
				util::ParametersMap& pm
			);

		protected:
			util::Env&				_env;
			const Import&			_import;
			mutable ImportLogLevel _maxLoggedLevel;	//!< Records the worse log level of the content
			const ImportLogLevel _minLogLevel;	//!< Minimal level of entries to store in the content
			mutable std::auto_ptr<std::ofstream> _fileStream;		//!< Stream on the file where the content must be sent
			mutable boost::optional<std::ostream&> _outputStream;		//!< An output stream where the content must be sent
			util::ParametersMap& _pm;	//!< Parameters map where entries must be stored


			virtual db::DBTransaction _save() const = 0;

		private:
			void _log(
				ImportLogLevel level,
				const std::string& content
			) const;

		public:
			void _logError(const std::string& content) const;
			void _logWarning(const std::string& content) const;
			void _logDebug(const std::string& content) const;
			void _logInfo(const std::string& content) const;
			void _logLoad(const std::string& content) const;
			void _logCreation(const std::string& content) const;
			void _logRaw(
				const std::string& content
			) const;

		protected:
			ImportLogLevel _getMaxLoggedLevel() const { return _maxLoggedLevel; }

			template<class T>
			typename T::ObjectType* _loadOrCreateObject(
				impex::ImportableTableSync::ObjectBySource<T>& objects,
				const std::string& id,
				const impex::DataSource& source,
				const std::string& logName
			) const;

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
			/// @param result the parameters map where output can be done
			/// @return true if it is allowed to save the data
			virtual bool parseFiles() const = 0;



			//////////////////////////////////////////////////////////////////////////
			/// Interface for the save method.
			/// @return transaction to run
			db::DBTransaction save() const;
		};



		template<class T>
		typename T::ObjectType* Importer::_loadOrCreateObject(
			ImportableTableSync::ObjectBySource<T>& objects,
			const std::string& id,
			const impex::DataSource& source,
			const std::string& logName
		) const {
			std::set<typename T::ObjectType*> loadedObjects(objects.get(id));
			if(!loadedObjects.empty())
			{
				if(!logName.empty())
				{
					std::stringstream logStream;
					logStream << "Link between " << logName << " " << id << " and ";
					BOOST_FOREACH(typename T::ObjectType* o, loadedObjects)
					{
						logStream << o->getKey();
					}
					_logLoad(logStream.str());
				}
				return *loadedObjects.begin();
			}
			boost::shared_ptr<typename T::ObjectType> o(new typename T::ObjectType(T::getId()));

			Importable::DataSourceLinks links;
			links.insert(make_pair(&source, id));
			o->setDataSourceLinksWithoutRegistration(links);
			_env.getEditableRegistry<typename T::ObjectType>().add(o);
			objects.add(*o);

			if(!logName.empty())
			{
				_logCreation(
					"Creation of the "+ logName +"  with key "+ id
				);
			}
			return o.get();
		}
}	}

#endif // SYNTHESE_impex_Importer_hpp__
