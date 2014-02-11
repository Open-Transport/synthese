
/** ConnectionImporter class header.
	@file ConnectionImporter.hpp

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

#ifndef SYNTHESE_impex_ConnectionImporter_hpp__
#define SYNTHESE_impex_ConnectionImporter_hpp__

#include "Importer.hpp"

#include <fstream>

namespace synthese
{
	namespace impex
	{
		/** ConnectionImporter class.
			@ingroup m16
		*/
		template<class FF>
		class ConnectionImporter:
			virtual public Importer
		{
		public:
			static const bool IMPORTABLE;
			static const std::string PARAMETER_ADDRESS;
			static const std::string PARAMETER_PORT;

		protected:
			virtual bool _read(
			) const = 0;

			std::string _address;
			std::string _port;

			virtual void _setFromParametersMap(const util::ParametersMap& map) {}

			virtual util::ParametersMap _getParametersMap() const { return util::ParametersMap(); }

		public:
			ConnectionImporter(
				util::Env& env,
				const Import& import,
				impex::ImportLogLevel minLogLevel,
				const std::string& logPath,
				boost::optional<std::ostream&> outputStream,
				util::ParametersMap& pm
			):	Importer(env, import, minLogLevel, logPath, outputStream, pm)
			{}



			void setFromParametersMap(
				const util::ParametersMap& map,
				bool doImport
			){
				_address = map.getDefault<string>(PARAMETER_ADDRESS);
				_port = map.getDefault<string>(PARAMETER_PORT);
				_setFromParametersMap(map);
			}



			util::ParametersMap getParametersMap() const
			{
				util::ParametersMap result(_getParametersMap());
				result.insert(PARAMETER_ADDRESS, _address);
				result.insert(PARAMETER_PORT, _port);
				return result;
			}



			bool parseFiles(
			) const {

				if(!_address.empty())
				{
					return _read();
				}
				return false;
			}
		};

		template<class FF>
		const bool ConnectionImporter<FF>::IMPORTABLE(true);

		template<class FF>
		const std::string ConnectionImporter<FF>::PARAMETER_ADDRESS("address");

		template<class FF>
		const std::string ConnectionImporter<FF>::PARAMETER_PORT("port");
}	}

#endif // SYNTHESE_impex_ConnectionImporter_hpp__
