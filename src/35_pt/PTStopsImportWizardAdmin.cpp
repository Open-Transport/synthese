
//////////////////////////////////////////////////////////////////////////
/// PTStopsImportWizardAdmin class implementation.
///	@file PTStopsImportWizardAdmin.cpp
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "PTStopsImportWizardAdmin.hpp"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "PropertiesHTMLTable.h"
#include "GeoPoint.h"
#include "CoordinatesSystem.hpp"
#include "AdminFunctionRequest.hpp"
#include "StopPointTableSync.hpp"
#include "PTPlaceAdmin.h"
#include "HTMLModule.h"
#include "StopPointAdmin.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "StopAreaAddAction.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "DataSourceTableSync.h"
#include "ImportFunction.h"
#include "FileFormat.h"
#include "TridentFileFormat.h"
#include "CarPostalFileFormat.hpp"

#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace html;
	using namespace geography;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTStopsImportWizardAdmin>::FACTORY_KEY("PTStopsImportWizardAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTStopsImportWizardAdmin>::ICON("database_copy.png");
		template<> const string AdminInterfaceElementTemplate<PTStopsImportWizardAdmin>::DEFAULT_TITLE("Import d'arrêts");
	}

	namespace pt
	{
		const string PTStopsImportWizardAdmin::PARAM_BAHNHOF_FILE_NAME("bf");
		const string PTStopsImportWizardAdmin::PARAM_KOORDS_FILE_NAME("kf");
		const string PTStopsImportWizardAdmin::TAB_IMPORT("im");
		const string PTStopsImportWizardAdmin::TAB_IMPORT_STOPS("is");
		const string PTStopsImportWizardAdmin::TAB_PROPERTIES("pr");



		PTStopsImportWizardAdmin::PTStopsImportWizardAdmin()
			: AdminInterfaceElementTemplate<PTStopsImportWizardAdmin>()
		{ }


		
		void PTStopsImportWizardAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_dataSource = DataSourceTableSync::Get(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID), *_env);
			}
			catch (ObjectNotFoundException<DataSource>&)
			{
				throw AdminParametersException("No such data source");
			}
			_bahnhofFileName = map.getDefault<string>(PARAM_BAHNHOF_FILE_NAME);
			_koordsFileName = map.getDefault<string>(PARAM_KOORDS_FILE_NAME);
		}



		ParametersMap PTStopsImportWizardAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_dataSource.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _dataSource->getKey());
			}
			m.insert(PARAM_BAHNHOF_FILE_NAME, _bahnhofFileName);
			m.insert(PARAM_KOORDS_FILE_NAME, _koordsFileName);
			return m;
		}


		
		bool PTStopsImportWizardAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}



		void PTStopsImportWizardAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const admin::AdminRequest& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_PROPERTIES))
			{
			}

			////////////////////////////////////////////////////////////////////
			// TAB IMPORT
			if (openTabContent(stream, TAB_IMPORT))
			{
/*				if(_dataSource->getFormat() == NavteqWithProjectionFileFormat::FACTORY_KEY)
				{
					StaticFunctionRequest<ImportFunction> importRequest(request, true);
					importRequest.getFunction()->setDataSource(_dataSource);

					PropertiesHTMLTable t(importRequest.getHTMLForm());
					stream << t.open();
					stream << t.title("Propriétés");
					stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(ImportFunction::PARAMETER_DO_IMPORT, false));
					stream << t.title("Données");
					stream << t.cell("Rues (streets)", t.getForm().getTextInput(ImportFunction::PARAMETER_PATH + NavteqWithProjectionFileFormat::FILE_STREETS, string()));
					stream << t.cell("Noeuds (nodes)", t.getForm().getTextInput(ImportFunction::PARAMETER_PATH + NavteqWithProjectionFileFormat::FILE_NODES, string()));
					stream << t.cell("Zones administratives (mtdarea)", t.getForm().getTextInput(ImportFunction::PARAMETER_PATH + NavteqWithProjectionFileFormat::FILE_MTDAREA, string()));
					stream << t.close();
				}
				else*/ if(_dataSource->getFormat() == TridentFileFormat::FACTORY_KEY)
				{
					StaticFunctionRequest<ImportFunction> importRequest(request, true);
					importRequest.getFunction()->setDataSource(_dataSource);

					PropertiesHTMLTable t(importRequest.getHTMLForm());
					stream << t.open();
					stream << t.title("Propriétés");
					stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(ImportFunction::PARAMETER_DO_IMPORT, false));
					stream << t.cell("Import arrêts", t.getForm().getOuiNonRadioInput(TridentFileFormat::PARAMETER_IMPORT_STOPS, false));
					stream << t.cell("Import transferts", t.getForm().getOuiNonRadioInput(TridentFileFormat::PARAMETER_IMPORT_JUNCTIONS, false));
					stream << t.cell("Importer dates passées (nombre de jours)", t.getForm().getTextInput(TridentFileFormat::PARAMETER_WITH_OLD_DATES, "0"));
					stream << t.cell("Temps de correspondance par défaut (minutes)", t.getForm().getTextInput(TridentFileFormat::PARAMETER_DEFAULT_TRANSFER_DURATION, "8"));
					stream << t.title("Données");
					stream << t.cell("Ligne", t.getForm().getTextInput(ImportFunction::PARAMETER_PATH, string()));
					stream << t.close();
				}
				else if(_dataSource->getFormat() == CarPostalFileFormat::FACTORY_KEY)
				{
					StaticFunctionRequest<ImportFunction> importRequest(request, true);
					importRequest.getFunction()->setDataSource(_dataSource);

					PropertiesHTMLTable t(importRequest.getHTMLForm());
					stream << t.open();
					stream << t.title("Propriétés");
					stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(ImportFunction::PARAMETER_DO_IMPORT, false));
					stream << t.title("Données");
					stream << t.cell("Eckdaten", t.getForm().getTextInput(ImportFunction::PARAMETER_PATH + CarPostalFileFormat::FILE_ECKDATEN, string()));
					stream << t.cell("Bitfeld", t.getForm().getTextInput(ImportFunction::PARAMETER_PATH + CarPostalFileFormat::FILE_BITFELD, string()));
					stream << t.cell("Zugdat", t.getForm().getTextInput(ImportFunction::PARAMETER_PATH + CarPostalFileFormat::FILE_ZUGUDAT, string()));
					stream << t.close();
				}
				else
				{
					stream << "<p class=\"info\">Format de données d'entrées inconnu.</p>";
				}
			}

			////////////////////////////////////////////////////////////////////
			// TAB STOPS IMPORT
			if (openTabContent(stream, TAB_IMPORT_STOPS))
			{
				stream << "<h1>Fichiers</h1>";

				AdminFunctionRequest<PTStopsImportWizardAdmin> reloadRequest(request);
				PropertiesHTMLTable t(reloadRequest.getHTMLForm());
				stream << t.open();
				stream << t.cell("Fichier bahnhof", t.getForm().getTextInput(PARAM_BAHNHOF_FILE_NAME, _bahnhofFileName));
				stream << t.cell("Fichier koords", t.getForm().getTextInput(PARAM_KOORDS_FILE_NAME, _koordsFileName));
				stream << t.close();

				if(!_bahnhofFileName.empty() && !_koordsFileName.empty())
				{
					Bahnhofs nonLinkedBahnhofs;
					Bahnhofs linkedBahnhofs;

					{
						ifstream inFile;
						inFile.open(_bahnhofFileName.c_str());
						string line;

						while(getline(inFile, line))
						{
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
								linkedBahnhofs[bahnhof.operatorCode] = bahnhof;
							}
							else
							{
								nonLinkedBahnhofs[bahnhof.operatorCode] = bahnhof;
							}
						}

						inFile.close();
					}

					{
						ifstream inFile;
						inFile.open(_koordsFileName.c_str());
						string line;

						while(getline(inFile, line))
						{
							string operatorCode(line.substr(0, 7));
							bool ok(true);
							Bahnhofs::iterator it(linkedBahnhofs.find(operatorCode));
							if(it == linkedBahnhofs.end())
							{
								it = nonLinkedBahnhofs.find(operatorCode);
								if(it == nonLinkedBahnhofs.end())
								{
									ok = false;
								}
							}
							if(ok)
							{
								it->second.coords = GeoPoint(
									Coordinate(
										1000 * lexical_cast<double>(line.substr(10,7)),
										1000 * lexical_cast<double>(line.substr(20,7))
									), CoordinatesSystem::GetCoordinatesSystem("EPSG:21781")
								);
							}
						}

						inFile.close();
					}

					{
						stream << "<h1>Arrêts non liés à SYNTHESE</h1>";

						HTMLTable::ColsVector c;
						c.push_back("Code");
						c.push_back("Localité");
						c.push_back("Nom");
						c.push_back("Coords fichier");
						c.push_back("Coords fichier");
						c.push_back("Actions");

						HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
						stream << t.open();
						BOOST_FOREACH(const Bahnhofs::value_type& bahnhof, nonLinkedBahnhofs)
						{
							stream << t.row();
							stream << t.col();
							stream << bahnhof.first;

							stream << t.col();
							stream << bahnhof.second.cityName;

							stream << t.col();
							stream << bahnhof.second.name;

							stream << t.col();
							stream << bahnhof.second.coords.x;

							stream << t.col();
							stream << bahnhof.second.coords.y;

							stream << t.col();
							AdminActionFunctionRequest<StopAreaAddAction, PTStopsImportWizardAdmin> addRequest(request);
							addRequest.getAction()->setCreateCityIfNecessary(true);
							addRequest.getAction()->setCreatePhysicalStop(true);
							addRequest.getAction()->setName(bahnhof.second.name);
							addRequest.getAction()->setCityName(bahnhof.second.cityName);
							addRequest.getAction()->setOperatorCode(bahnhof.first);
							addRequest.getAction()->setPoint(bahnhof.second.coords);
							stream << HTMLModule::getLinkButton(addRequest.getURL(), "Ajouter");
						}
						stream << t.close();
					}

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
						c.push_back("Actions");

						AdminFunctionRequest<PTPlaceAdmin> openRequest(request);
						AdminFunctionRequest<StopPointAdmin> openPhysicalRequest(request);

						HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
						stream << t.open();
						BOOST_FOREACH(const Bahnhofs::value_type& bahnhof, linkedBahnhofs)
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

							stream << t.col();
							stream << bahnhof.second.stop->x;

							stream << t.col();
							stream << bahnhof.second.stop->y;

							stream << t.col();
							stream << bahnhof.second.coords.x;

							stream << t.col();
							stream << bahnhof.second.coords.y;

							stream << t.col();
							stream << "Mettre à jour coordonnées";
						}
						stream << t.close();
					}
				}
			}

			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}



		std::string PTStopsImportWizardAdmin::getTitle() const
		{
			return
				_dataSource.get() ?
				DEFAULT_TITLE :
				(	_dataSource->getName() + (_bahnhofFileName.empty() ?
						string() :
						(" " + _bahnhofFileName)
				)	)
			;
		}



		void PTStopsImportWizardAdmin::_buildTabs( const security::Profile& profile ) const
		{
			_tabs.clear();
			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, profile.isAuthorized<TransportNetworkRight>(WRITE)));
			if(	_dataSource.get() &&
				!_dataSource->getFormat().empty() &&
				Factory<FileFormat>::contains(_dataSource->getFormat()) &&
				profile.isAuthorized<TransportNetworkRight>(WRITE)
			){
				_tabs.push_back(Tab("Import", TAB_IMPORT, true));
			
				if(_dataSource->getFormat() == CarPostalFileFormat::FACTORY_KEY)
				{
					_tabs.push_back(Tab("Import manuel arrêts", TAB_IMPORT_STOPS, true));
				}
			}
			_tabBuilded = true;
		}
	}
}
