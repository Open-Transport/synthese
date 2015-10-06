
/** Import class implementation.
	@file Import.cpp

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

#include "Import.hpp"

#include "Env.h"
#include "FileFormat.h"
#include "Importer.hpp"
#include "Profile.h"
#include "User.h"

using namespace boost;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace util;

	CLASS_DEFINITION(impex::Import, "t105_imports", 105)
	FIELD_DEFINITION_OF_OBJECT(impex::Import, "import_id", "import_ids")


	FIELD_DEFINITION_OF_TYPE(FileFormatKey, "file_format", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(AutoImportDelay, "auto_import_delay", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(AutoImportTime, "auto_import_time", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(LogPath, "log_path", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(MinLogLevel, "min_log_level", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Forbidden, "forbidden", SQL_BOOLEAN)

	namespace impex
	{
		Import::Import(
			util::RegistryKeyType id /*= 0*/
		):	Registrable(id),
			Object<Import, ImportRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(DataSource),
					FIELD_DEFAULT_CONSTRUCTOR(FileFormatKey),
					FIELD_DEFAULT_CONSTRUCTOR(Parameters),
					FIELD_VALUE_CONSTRUCTOR(AutoImportDelay, time_duration(not_a_date_time)),
					FIELD_VALUE_CONSTRUCTOR(AutoImportTime, time_duration(not_a_date_time)),
					FIELD_VALUE_CONSTRUCTOR(Active, true),
					FIELD_DEFAULT_CONSTRUCTOR(LogPath),
					FIELD_VALUE_CONSTRUCTOR(MinLogLevel, IMPORT_LOG_WARN),
					FIELD_DEFAULT_CONSTRUCTOR(Documentation),
					FIELD_VALUE_CONSTRUCTOR(Forbidden, false)
			)	),
			_nextAutoImport(not_a_date_time)
		{}



		//////////////////////////////////////////////////////////////////////////
		/// Importer creation helper
		/// @param env the import environment
		/// @param minLogLevel if defined, override the the min log level with the specified value
		/// @param logPath if defined, override the the log path with the specified value
		boost::shared_ptr<Importer> Import::getImporter(
			util::Env& env,
			ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		) const {

			boost::shared_ptr<FileFormat> fileFormat(Factory<FileFormat>::create(get<FileFormatKey>()));
			return fileFormat->getImporter(env, *this, minLogLevel, logPath, outputStream, pm);
		}



		bool Import::canImport() const
		{
			if(!Factory<FileFormat>::contains(get<FileFormatKey>()))
			{
				return false;
			}

			boost::shared_ptr<FileFormat> fileFormat(Factory<FileFormat>::create(get<FileFormatKey>()));
			return fileFormat->canImport();
		}



		bool Import::isPermanentThread() const
		{
			if(!Factory<FileFormat>::contains(get<FileFormatKey>()))
			{
				return false;
			}

			boost::shared_ptr<FileFormat> fileFormat(Factory<FileFormat>::create(get<FileFormatKey>()));
			return fileFormat->isImportPermanentThread();
		}



		void Import::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(canImport())
			{
				// Compute the time of the next auto import
				_computeNextAutoImport();
			}
			else if(isPermanentThread())
			{
				if(get<Active>() && (&env == &Env::GetOfficialEnv()))
				{
					_getAutoImporter()->runPermanentThread();
				}
			}
		}



		void Import::unlink()
		{
			// Kill the thread if exists
			if(_autoImporter)
			{
				if(isPermanentThread())
				{
					_autoImporter->killPermanentThread();
				}

				// Delete the auto importer cache in case of parameter update
				recursive_mutex::scoped_lock lock(_autoImportMutex);
				_autoImporter.reset();
			}
		}



		void Import::_computeNextAutoImport() const
		{
			_nextAutoImport = ptime(not_a_date_time);
			ptime now(second_clock::local_time());
			if(!get<AutoImportDelay>().is_not_a_date_time())
			{
				now += get<AutoImportDelay>();
				_nextAutoImport = now;
			}
			else if(!get<AutoImportTime>().is_not_a_date_time())
			{
				if(now.time_of_day() > get<AutoImportTime>())
				{
					now += hours(24);
				}
				_nextAutoImport = ptime(now.date(), get<AutoImportTime>());
			}
		}



		boost::shared_ptr<Importer> Import::_getAutoImporter() const
		{

			recursive_mutex::scoped_lock lock(_autoImportMutex);
			if(!_autoImporter.get())
			{
				boost::shared_ptr<FileFormat> fileFormat(Factory<FileFormat>::create(get<FileFormatKey>()));
				_autoImporterEnv.reset(new Env);
				_autoImporter = fileFormat->getImporter(
					*_autoImporterEnv,
					*this,
					get<MinLogLevel>(),
					get<LogPath>(),
					optional<ostream&>(),
					_autoImporterPM
				);
				_autoImporter->setFromParametersMap(get<Parameters>(), true);
			}
			return _autoImporter;
		}



		void Import::runAutoImport() const
		{
			ptime startTime(second_clock::local_time());
			recursive_mutex::scoped_lock lock(_autoImportMutex);

			if(!_autoImporterEnv)
			{
				_getAutoImporter();
			}

			_autoImporterEnv->clear();
			_getAutoImporter()->openLogFile();
			bool result(_getAutoImporter()->parseFiles());
			if(result)
			{
				DBTransaction transaction(_getAutoImporter()->save());
				DBModule::DeactivateConditionalTablesUpdate();
				try
				{
				transaction.run();
			}
				catch(...)
				{

				}
				DBModule::ActivateConditionalTablesUpdate();
			}
			_getAutoImporter()->closeLogFile(
				result,
				false,
				startTime
			);

			// TODO clean

			// Compute the time of the next auto import
			_computeNextAutoImport();
		}



		void Import::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix
		) const	{

			map.merge(get<Parameters>());

			// Compute the time of the next auto import
			_computeNextAutoImport();

		}


		bool Import::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool Import::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool Import::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}

