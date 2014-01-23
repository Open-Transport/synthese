
/** NoImportPolicy class header.
	@file NoImportPolicy.hpp

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

#ifndef SYNTHESE_impex_NoImportPolicy_hpp__
#define SYNTHESE_impex_NoImportPolicy_hpp__

#include "Importer.hpp"

namespace synthese
{
	namespace impex
	{
		/** NoImportPolicy class.
			@ingroup m16
		*/
		template<class FF>
		class NoImportPolicy:
			public Importer
		{
		public:
			static const bool IMPORTABLE;
			static const bool IS_PERMANENT_THREAD;

			NoImportPolicy(
				util::Env& env,
				const Import& import,
				ImportLogLevel minLogLevel,
				const std::string& logPath,
				boost::optional<std::ostream&> outputStream,
				util::ParametersMap& pm
			):	Importer(env, import, minLogLevel, logPath, outputStream, pm)
			{}

			virtual void setFromParametersMap(
				const util::ParametersMap& map,
				bool doImport
			) {}


			virtual util::ParametersMap getParametersMap() const { return util::ParametersMap(); }

			virtual db::DBTransaction _save() const { return db::DBTransaction(); }
			virtual bool parseFiles() const { return false; }
		};

		template<class FF>
		const bool NoImportPolicy<FF>::IMPORTABLE(false);

		template<class FF>
		const bool NoImportPolicy<FF>::IS_PERMANENT_THREAD(false);
}	}

#endif // SYNTHESE_impex_NoImportPolicy_hpp__
