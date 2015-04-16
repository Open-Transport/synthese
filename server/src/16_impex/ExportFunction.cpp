
/** ExportFunction class implementation.
	@file ExportFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "ExportFunction.hpp"

#include "RequestException.h"
#include "Request.h"
#include "Export.hpp"
#include "Exporter.hpp"
#include "FileFormat.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,impex::ExportFunction>::FACTORY_KEY("ExportFunction");

	namespace impex
	{
		const string ExportFunction::PARAMETER_FILE_FORMAT("ff");



		ParametersMap ExportFunction::_getParametersMap() const
		{
			return _pm;
		}



		void ExportFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Designated export
			RegistryKeyType id(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0));
			if(decodeTableId(id) == Export::CLASS_NUMBER)
			{
				_export = Env::GetOfficialEnv().getEditable<Export>(id);
			}
			else
			{
				// Case generated export
				string fileFormatKey(map.get<string>(PARAMETER_FILE_FORMAT));
				_export.reset(new Export);
				_export->set<FileFormatKey>(fileFormatKey);
			}
			_exporter = _export->getExporter();

			_exporter->setFromParametersMap(map);
		}



		util::ParametersMap ExportFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			util::ParametersMap pm;
			_exporter->build(stream);
			return pm;
		}



		bool ExportFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ExportFunction::getOutputMimeType() const
		{
			return _export.get() ? _export->getExporter()->getOutputMimeType() : "text/plain";
		}
	}
}
