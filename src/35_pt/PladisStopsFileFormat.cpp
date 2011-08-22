
/** PladisStopsFileFormat class implementation.
	@file PladisStopsFileFormat.cpp

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

#include "PladisStopsFileFormat.hpp"
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
#include "IConv.hpp"
#include "Importer.hpp"
#include "StopPointAddAction.hpp"
#include "StopPointUpdateAction.hpp"
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
		template<> const string FactorableTemplate<FileFormat,PladisStopsFileFormat>::FACTORY_KEY("PladisStops");
	}

	namespace pt
	{
		const std::string PladisStopsFileFormat::Importer_::FILE_BAHNHOFS("bahnhofs");
		const std::string PladisStopsFileFormat::Importer_::FILE_KOORDS("koords");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<PladisStopsFileFormat>::Files MultipleFileTypesImporter<PladisStopsFileFormat>::FILES(
			PladisStopsFileFormat::Importer_::FILE_BAHNHOFS.c_str(),
			PladisStopsFileFormat::Importer_::FILE_KOORDS.c_str(),
		"");
	}

	namespace pt
	{
		bool PladisStopsFileFormat::Importer_::_checkPathsMap() const
		{
			// BAHNHOFS
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_BAHNHOFS));
			if(it == _pathsMap.end() || it->second.empty() || !exists(it->second))
			{
				return false;
			}

			// KOORDS
			it = _pathsMap.find(FILE_KOORDS);
			if(it == _pathsMap.end() || it->second.empty() || !exists(it->second))
			{
				return false;
			}

			// OK
			return true;
		}



		bool pt::PladisStopsFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& stream,
			const std::string& key,
			boost::optional<const admin::AdminRequest&> request
		) const {
			if(!request)
			{
				return false;
			}

			ifstream inFile;
			inFile.open(filePath.file_string().c_str());

			if(key == FILE_BAHNHOFS)
			{
				string line;

				while(getline(inFile, line))
				{
					if(!_dataSource.getCharset().empty())
					{
						line = IConv::IConv(_dataSource.getCharset(), "UTF-8").convert(line);
					}
					Bahnhof bahnhof;
					bahnhof.operatorCode = line.substr(0, 7);

					vector<string> cols;
					std::string times(line.substr(12));
					boost::algorithm::split( cols, times, boost::algorithm::is_any_of(","));
					bahnhof.cityName = cols[0];
					bahnhof.name = (cols.size() == 1) ? "Arrêt" : cols[1];

					StopPointTableSync::SearchResult stops(
						StopPointTableSync::Search(
							Env::GetOfficialEnv(),
							optional<RegistryKeyType>(),
							bahnhof.operatorCode,
							false,
							true,
							0,
							1
					)	);
					if(!stops.empty())
					{
						bahnhof.stop = stops.front();
						_linkedBahnhofs[bahnhof.operatorCode] = bahnhof;
					}
					else
					{
						_nonLinkedBahnhofs[bahnhof.operatorCode] = bahnhof;
					}
				}
			}

			if(key == FILE_KOORDS)
			{
				string line;

				while(getline(inFile, line))
				{
					string operatorCode(line.substr(0, 7));
					bool ok(true);
					Bahnhofs::iterator it(_linkedBahnhofs.find(operatorCode));
					if(it == _linkedBahnhofs.end())
					{
						it = _nonLinkedBahnhofs.find(operatorCode);
						if(it == _nonLinkedBahnhofs.end())
						{
							ok = false;
						}
					}
					if(ok)
					{
						it->second.coords = CoordinatesSystem::GetCoordinatesSystem(21781).createPoint(
							1000 * lexical_cast<double>(line.substr(10,7)),
							1000 * lexical_cast<double>(line.substr(20,7))
						);

						it->second.projected = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*it->second.coords);
					}
				}

				if(!_nonLinkedBahnhofs.empty())
				{
					stream << "<h1>Arrêts non liés à SYNTHESE</h1>";

					HTMLTable::ColsVector c;
					c.push_back("Code");
					c.push_back("Localité");
					c.push_back("Nom");
					c.push_back("Coords fichier");
					c.push_back("Coords fichier");
					c.push_back("Coords fichier (origine)");
					c.push_back("Coords fichier (origine)");
					c.push_back("Actions");

					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();
					stream.precision(0);
					BOOST_FOREACH(const Bahnhofs::value_type& bahnhof, _nonLinkedBahnhofs)
					{
						stream << t.row();
						stream << t.col();
						stream << bahnhof.first;

						stream << t.col();
						stream << bahnhof.second.cityName;

						stream << t.col();
						stream << bahnhof.second.name;

						if(bahnhof.second.projected.get())
						{
						    stream << t.col();
						    stream << fixed << bahnhof.second.projected->getX();

						    stream << t.col();
						    stream << fixed << bahnhof.second.projected->getY();
						}
						else
						{
						    stream << t.col();
						    stream << t.col();
						}

						if(bahnhof.second.coords.get())
						{
							stream << t.col();
							stream << fixed << bahnhof.second.coords->getX();

							stream << t.col();
							stream << fixed << bahnhof.second.coords->getY();

							stream << t.col();
							AdminActionFunctionRequest<StopPointAddAction, DataSourceAdmin> addRequest(*request);
							addRequest.getAction()->setName(bahnhof.second.name);
							addRequest.getAction()->setCityName(bahnhof.second.cityName);
							addRequest.getAction()->setCreateCityIfNecessary(true);
							Importable::DataSourceLinks links;
							links.insert(make_pair(&_dataSource, bahnhof.first));
							addRequest.getAction()->setDataSourceLinks(links);
							addRequest.getAction()->setPoint(DBModule::GetStorageCoordinatesSystem().convertPoint(*bahnhof.second.coords));
							stream << HTMLModule::getLinkButton(addRequest.getURL(), "Ajouter");
						}
						else
						{
							stream << t.col();
							stream << t.col();
							stream << t.col();
						}

					}
					stream << t.close();
				}

				if(!_linkedBahnhofs.empty())
				{
					stream << "<h1>Arrêts liés à SYNTHESE</h1>";

					HTMLTable::ColsVector c;
					c.push_back("Code");
					c.push_back("Zone d'arrêt SYNTHESE");
					c.push_back("Arrêt physique");
					c.push_back("Localité");
					c.push_back("Nom");
					c.push_back("Coords SYNTHESE");
					c.push_back("Coords SYNTHESE");
					c.push_back("Coords fichier");
					c.push_back("Coords fichier");
					c.push_back("Coords fichier (origine)");
					c.push_back("Coords fichier (origine)");
					c.push_back("Distance");
					c.push_back("Actions");

					AdminFunctionRequest<PTPlaceAdmin> openRequest(*request);
					AdminFunctionRequest<StopPointAdmin> openPhysicalRequest(*request);

					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();
					stream.precision(0);
					BOOST_FOREACH(const Bahnhofs::value_type& bahnhof, _linkedBahnhofs)
					{
						stream << t.row();
						stream << t.col();
						stream << bahnhof.first;

						stream << t.col();
						openRequest.getPage()->setConnectionPlace(Env::GetOfficialEnv().getSPtr(bahnhof.second.stop->getConnectionPlace()));
						stream << HTMLModule::getHTMLLink(openRequest.getURL(), bahnhof.second.stop->getConnectionPlace()->getFullName());

						stream << t.col();
						openPhysicalRequest.getPage()->setStop(bahnhof.second.stop);
						stream << HTMLModule::getHTMLLink(openPhysicalRequest.getURL(), bahnhof.second.stop->getName());

						stream << t.col();
						stream << bahnhof.second.cityName;

						stream << t.col();
						stream << bahnhof.second.name;

						if(bahnhof.second.stop->getGeometry().get())
						{
							stream << t.col() << std::fixed << bahnhof.second.stop->getGeometry()->getX();
							stream << t.col() << std::fixed << bahnhof.second.stop->getGeometry()->getY();
						}
						else
						{
							stream << t.col() << "(non localisé)";
							stream << t.col() << "(non localisé)";
						}

						if(bahnhof.second.coords.get())
						{
							double distance(-1);
							if (bahnhof.second.stop->getGeometry().get())
							{
								distance = geos::operation::distance::DistanceOp::distance(*bahnhof.second.projected, *bahnhof.second.stop->getGeometry());
							}

							if(bahnhof.second.projected.get())
							{
								stream << t.col();
								stream << fixed << bahnhof.second.projected->getX();

								stream << t.col();
								stream << fixed << bahnhof.second.projected->getY();
							}
							else
							{
								stream << t.col();
								stream << t.col();
							}

							if(bahnhof.second.coords.get())
							{
								stream << t.col();
								stream << fixed << bahnhof.second.coords->getX();

								stream << t.col();
								stream << fixed << bahnhof.second.coords->getY();
							}
							else
							{
								stream << t.col();
								stream << t.col();
							}

							stream << t.col();
							if(distance == 0)
							{
								stream << "identiques";
							}
							if(distance > 0)
							{
								stream << distance << " m";
							}

							stream << t.col();
							if(distance != 0)
							{
								AdminActionFunctionRequest<StopPointUpdateAction, DataSourceAdmin> moveRequest(*request);
								moveRequest.getAction()->setStop(bahnhof.second.stop);
								moveRequest.getAction()->setPoint(DBModule::GetStorageCoordinatesSystem().convertPoint(*bahnhof.second.coords));
								stream << HTMLModule::getLinkButton(moveRequest.getHTMLForm().getURL(), "Mettre à jour coordonnées");
							}
						}
						else
						{
							stream << t.col();
							stream << t.col();
							stream << t.col();
							stream << t.col();
							stream << t.col();
							stream << t.col();
						}
					}
					stream << t.close();
				}
			}

			inFile.close();

			return false;
		}



		void PladisStopsFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
		) const	{
			stream << "<h1>Fichiers</h1>";

			AdminFunctionRequest<DataSourceAdmin> reloadRequest(request);
			PropertiesHTMLTable t(reloadRequest.getHTMLForm());
			stream << t.open();
			stream << t.cell("Fichier bahnhof", t.getForm().getTextInput(_getFileParameterName(FILE_BAHNHOFS), _pathsMap[FILE_BAHNHOFS].file_string()));
			stream << t.cell("Fichier koord", t.getForm().getTextInput(_getFileParameterName(FILE_KOORDS), _pathsMap[FILE_KOORDS].file_string()));
			stream << t.close();
		}



		db::DBTransaction PladisStopsFileFormat::Importer_::_save() const
		{
			return DBTransaction();
		}
}	}
