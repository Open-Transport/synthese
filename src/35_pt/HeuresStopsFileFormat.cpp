
/** HeuresStopsFileFormat class implementation.
	@file HeuresStopsFileFormat.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "HeuresStopsFileFormat.hpp"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "StopPointTableSync.hpp"
#include "PTPlaceAdmin.h"
#include "StopArea.hpp"
#include "DataSource.h"
#include "ImpExModule.h"
#include "Importer.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLModule.h"
#include "HTMLForm.h"
#include "DBModule.h"
#include "PTFileFormat.hpp"

#include <geos/geom/Point.h>
#include <geos/opDistance.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>

using namespace boost::filesystem;
using namespace std;
using namespace boost;

namespace synthese
{
	using namespace html;
	using namespace admin;
	using namespace impex;
	using namespace util;
	using namespace db;
	using namespace pt;
	using namespace server;


	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,HeuresStopsFileFormat>::FACTORY_KEY("HeuresStops");
	}

	namespace pt
	{
		bool HeuresStopsFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& stream,
			boost::optional<const admin::AdminRequest&> request
		) const {
			if(!request)
			{
				return false;
			}

			ifstream inFile;
			inFile.open(filePath.file_string().c_str());

			string line;

			PTFileFormat::ImportableStopPoints linkedStopPoints;
			PTFileFormat::ImportableStopPoints nonLinkedStopPoints;
			ImportableTableSync::ObjectBySource<StopPointTableSync> stopPoints(_dataSource, _env);

			while(getline(inFile, line))
			{
				if(!_dataSource.getCharset().empty())
				{
					line = ImpExModule::ConvertChar(line, _dataSource.getCharset(), "UTF-8");
				}

				string id(boost::algorithm::trim_copy(line.substr(0, 4)));
				string name(boost::algorithm::trim_copy(line.substr(5, 50)));

				PTFileFormat::ImportableStopPoint isp;
				isp.operatorCode = id;
				isp.name = name;
				isp.linkedStopPoints = stopPoints.get(id);
				
				if(isp.linkedStopPoints.empty())
				{
					nonLinkedStopPoints.push_back(isp);
				}
				else
				{
					linkedStopPoints.push_back(isp);
				}
			}
			inFile.close();

			PTFileFormat::DisplayStopPointImportScreen(
				nonLinkedStopPoints,
				*request,
				_env,
				_dataSource,
				stream
			);
			PTFileFormat::DisplayStopPointImportScreen(
				linkedStopPoints,
				*request,
				_env,
				_dataSource,
				stream
			);
		
			return false;
		}



		void HeuresStopsFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
		) const	{
			stream << "<h1>Fichier</h1>";

			AdminFunctionRequest<DataSourceAdmin> reloadRequest(request);
			PropertiesHTMLTable t(reloadRequest.getHTMLForm());
			stream << t.open();
			stream << t.cell("Fichier pointsarrets.tmp", t.getForm().getTextInput(PARAMETER_PATH, _pathsSet.empty() ? string() : _pathsSet.begin()->file_string()));
			stream << t.close();
		}



		db::SQLiteTransaction HeuresStopsFileFormat::Importer_::_save() const
		{
			return SQLiteTransaction();
		}
}	}
