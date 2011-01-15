
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
#include "StopPointAdmin.hpp"
#include "StopAreaAddAction.h"
#include "StopArea.hpp"
#include "DataSource.h"
#include "ImpExModule.h"
#include "Importer.hpp"
#include "StopPointMoveAction.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLModule.h"
#include "HTMLForm.h"
#include "DBModule.h"

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

			struct PointArret
			{
				std::string operatorCode;
				std::string name;
				boost::shared_ptr<StopPoint> stop;
			};

			typedef std::map<std::string, PointArret> PointsArrets;

			PointsArrets _nonLinkedPointsArrets;
			PointsArrets _linkedPointsArrets;

			while(getline(inFile, line))
			{
				if(!_dataSource.getCharset().empty())
				{
					line = ImpExModule::ConvertChar(line, _dataSource.getCharset(), "UTF-8");
				}

				PointArret pointArret;
				pointArret.operatorCode = line.substr(0, 4);
				pointArret.name = boost::algorithm::trim_copy(line.substr(5, 50));

				StopPointTableSync::SearchResult stops(
					StopPointTableSync::Search(
						Env::GetOfficialEnv(),
						optional<RegistryKeyType>(),
						pointArret.operatorCode,
						false,
						true,
						0,
						1
				)	);
				if(!stops.empty())
				{
					pointArret.stop = stops.front();
					_linkedPointsArrets[pointArret.operatorCode] = pointArret;
				}
				else
				{
					_nonLinkedPointsArrets[pointArret.operatorCode] = pointArret;
				}
			}
			inFile.close();

			if(!_nonLinkedPointsArrets.empty())
			{
				stream << "<h1>Arrêts non liés à SYNTHESE</h1>";

				HTMLTable::ColsVector c;
				c.push_back("Code");
				c.push_back("Nom");

				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				BOOST_FOREACH(const PointsArrets::value_type& pointArret, _nonLinkedPointsArrets)
				{
					stream << t.row();
					stream << t.col();
					stream << pointArret.first;

					stream << t.col();
					stream << pointArret.second.name;
				}
				stream << t.close();
			}

			if(!_linkedPointsArrets.empty())
			{
				stream << "<h1>Arrêts liés à SYNTHESE</h1>";

				HTMLTable::ColsVector c;
				c.push_back("Code source");
				c.push_back("Zone d'arrêt SYNTHESE");
				c.push_back("Arrêt physique SYNTHESE");
				c.push_back("Nom source");
				c.push_back("Coords SYNTHESE");
				c.push_back("Coords SYNTHESE");

				AdminFunctionRequest<PTPlaceAdmin> openRequest(*request);
				AdminFunctionRequest<StopPointAdmin> openPhysicalRequest(*request);

				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				stream.precision(0);
				BOOST_FOREACH(const PointsArrets::value_type& pointArret, _linkedPointsArrets)
				{
					stream << t.row();
					stream << t.col();
					stream << pointArret.first;

					stream << t.col();
					openRequest.getPage()->setConnectionPlace(Env::GetOfficialEnv().getSPtr(pointArret.second.stop->getConnectionPlace()));
					stream << HTMLModule::getHTMLLink(openRequest.getURL(), pointArret.second.stop->getConnectionPlace()->getFullName());

					stream << t.col();
					openPhysicalRequest.getPage()->setStop(pointArret.second.stop);
					stream << HTMLModule::getHTMLLink(openPhysicalRequest.getURL(), pointArret.second.stop->getName());

					stream << t.col();
					stream << pointArret.second.name;

					if(pointArret.second.stop->getGeometry().get())
					{
						stream << t.col() << std::fixed << pointArret.second.stop->getGeometry()->getX();
						stream << t.col() << std::fixed << pointArret.second.stop->getGeometry()->getY();
					}
					else
					{
						stream << t.col() << "(non localisé)";
						stream << t.col() << "(non localisé)";
					}
				}
				stream << t.close();
			}
		
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
