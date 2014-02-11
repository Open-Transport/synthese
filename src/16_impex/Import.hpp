
/** Import class header.
	@file Import.hpp

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

#ifndef SYNTHESE_impex_Import_hpp__
#define SYNTHESE_impex_Import_hpp__

#include "Object.hpp"

#include "CMSScriptField.hpp"
#include "DataSource.h"
#include "EnumObjectField.hpp"
#include "ImpExTypes.hpp"
#include "ParametersMapField.hpp"
#include "PtimeField.hpp"
#include "SchemaMacros.hpp"
#include "SecondsField.hpp"
#include "StringField.hpp"
#include "TimeField.hpp"

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace synthese
{
	namespace util
	{
		class Env;
	}

	FIELD_STRING(FileFormatKey)
	FIELD_STRING(LogPath)
	FIELD_SECONDS(AutoImportDelay)
	FIELD_TIME(AutoImportTime)
	FIELD_ENUM(MinLogLevel, impex::ImportLogLevel)
		
	namespace impex
	{
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(DataSource),
			FIELD(FileFormatKey),
			FIELD(Parameters),
			FIELD(AutoImportDelay),
			FIELD(AutoImportTime),
			FIELD(Active),
			FIELD(LogPath),
			FIELD(MinLogLevel),
			FIELD(Documentation)
		> ImportRecord;



		//////////////////////////////////////////////////////////////////////////
		/// Import class.
		///	@ingroup m19
		/// @author Hugues Romain
		/// @since 3.5.0
		class Import:
			public Object<Import, ImportRecord>
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<Import>	Registry;


		private:
			mutable boost::posix_time::ptime _nextAutoImport;
			mutable boost::shared_ptr<Importer> _autoImporter;
			mutable boost::shared_ptr<util::Env> _autoImporterEnv;
			void _computeNextAutoImport() const;
		
		public:
			Import(util::RegistryKeyType id = 0);

			//! @name Services
			//@{
				boost::shared_ptr<Importer> getImporter(
					util::Env& env,
					ImportLogLevel minLogLevel,
					const std::string& logPath,
					boost::optional<std::ostream&> outputStream,
					util::ParametersMap& pm
				) const;

				bool canImport() const;
				const boost::posix_time::ptime& getNextAutoImport() const { return _nextAutoImport; }
				void runAutoImport() const;
				virtual void addAdditionalParameters(util::ParametersMap& map, std::string prefix) const;
			//@}

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}
		};
}	}

#endif // SYNTHESE_cms_Import_hpp__
