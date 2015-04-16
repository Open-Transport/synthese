
//////////////////////////////////////////////////////////////////////////
/// DataSourceAdmin class implementation.
///	@file DataSourceAdmin.cpp
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#include "DataSourceAdmin.h"

#include "AdminParametersException.h"
#include "ObjectUpdateAction.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "User.h"
#include "ImpExModule.h"
#include "GlobalRight.h"
#include "PropertiesHTMLTable.h"
#include "DataSource.h"
#include "AdminActionFunctionRequest.hpp"
#include "FileFormat.h"
#include "Importer.hpp"
#include "DataSourceTableSync.h"
#include "CleanObsoleteDataAction.hpp"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	using namespace html;
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, DataSourceAdmin>::FACTORY_KEY("DataSourceAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DataSourceAdmin>::ICON("database.png");
		template<> const string AdminInterfaceElementTemplate<DataSourceAdmin>::DEFAULT_TITLE("Source de données");
	}

	namespace impex
	{
		const string DataSourceAdmin::TAB_IMPORT("ti");
		const string DataSourceAdmin::TAB_PROPERTIES("tp");
		const string DataSourceAdmin::TAB_MAINTENANCE("tm");

		const string DataSourceAdmin::PARAMETER_DO_IMPORT("di");



		DataSourceAdmin::DataSourceAdmin():
			AdminInterfaceElementTemplate<DataSourceAdmin>(),
			_doImport(false)
		{ }



		void DataSourceAdmin::setFromParametersMap(
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

//			if(Factory<FileFormat>::contains(_dataSource->getFormat()))
//			{
//				_doImport = map.getDefault<bool>(PARAMETER_DO_IMPORT, false);
//				_importer = _dataSource->getImporter(*_env);
//				_importer->setFromParametersMap(map, _doImport);
//			}
		}



		ParametersMap DataSourceAdmin::getParametersMap() const
		{
			ParametersMap m(_importer.get() ? _importer->getParametersMap() : ParametersMap());
			if(_dataSource.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _dataSource->getKey());
			}

			return m;
		}



		bool DataSourceAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<GlobalRight>(WRITE);
		}



		void DataSourceAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// PROPERTIES TAB
			if(openTabContent(stream, TAB_PROPERTIES))
			{
				AdminActionFunctionRequest<ObjectUpdateAction, DataSourceAdmin> updateRequest(request, *this);
				updateRequest.getAction()->setObject(const_cast<DataSource&>(*_dataSource));

				PropertiesHTMLTable t(updateRequest.getHTMLForm("updateds"));
				stream << t.open();
				stream << t.cell("ID", lexical_cast<string>(_dataSource->getKey()));
				stream << t.cell(
					"Nom",
					t.getForm().getTextInput(ObjectUpdateAction::GetInputName<Name>(), _dataSource->get<Name>())
				);
				stream << t.cell(
					"Icone",
					t.getForm().getTextInput(
						ObjectUpdateAction::GetInputName<Icon>(),
						_dataSource->get<Icon>()
					) + " " + HTMLModule::getHTMLImage("/admin/img/"+ (_dataSource->get<Icon>().empty() ? "note.png" : _dataSource->get<Icon>()), _dataSource->get<Name>())
				);
				stream << t.cell(
					"Jeu de caractères (défaut = auto-détection)",
					t.getForm().getTextInput(ObjectUpdateAction::GetInputName<Charset>(), _dataSource->get<Charset>())
				);
				stream << t.cell(
					"SRID",
					t.getForm().getTextInput(ObjectUpdateAction::GetInputName<CoordinatesSystem>(), _dataSource->get<CoordinatesSystem>() ? lexical_cast<string>(_dataSource->get<CoordinatesSystem>()->getSRID()) : string())
				);
				stream << t.close();
			}


			////////////////////////////////////////////////////////////////////
			// MAINTENANCE TAB
/*			if(openTabContent(stream, TAB_MAINTENANCE))
			{
				AdminActionFunctionRequest<CleanObsoleteDataAction, DataSourceAdmin> cleanRequest(request, *this);
				cleanRequest.getAction()->setEnv(_env);
				cleanRequest.getAction()->setObject(*_dataSource);
				PropertiesHTMLTable cleanForm(cleanRequest.getHTMLForm("clean"));
				date yesterday(gregorian::day_clock::local_day());
				yesterday -= days(1);
				stream << cleanForm.open();
				stream << cleanForm.title("Suppression des données obsolètes");
				stream << cleanForm.cell("Premier jour à conserver", cleanForm.getForm().getCalendarInput(CleanObsoleteDataAction::PARAMETER_FIRST_DATE, yesterday));
				stream << cleanForm.close();
			}

			////////////////////////////////////////////////////////////////////
			// IMPORT TAB
			if(openTabContent(stream, TAB_IMPORT))
			{
				_importer->displayAdmin(stream, request);

				stream << "<h1>Requête</h1><p>";
				_importer->getParametersMap().outputURI(stream);
				stream << "</p><h1>Log import</h1>";

				bool doImport(_doImport);
				doImport &= _importer->beforeParsing();
				doImport &= _importer->parseFiles(stream, request);
				_importer->getLogger().output(stream);
				doImport &= _importer->afterParsing();
				if(doImport)
				{
					_importer->save().run();
					// WARNING : Do not write code after this line, because the environment is cleaned by Importer::save.
				}

				stream << "Return code : " << (doImport ? "0" : "1");
			}
*/
			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		std::string DataSourceAdmin::getTitle() const
		{
			return _dataSource.get() ? _dataSource->get<Name>() : DEFAULT_TITLE;
		}



		bool DataSourceAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _dataSource->getKey() == static_cast<const DataSourceAdmin&>(other)._dataSource->getKey();
		}



		void DataSourceAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, true));
/*			if(_dataSource.get() && _dataSource->canImport())
			{
				_tabs.push_back(Tab("Import", TAB_IMPORT, true));
			}
			_tabs.push_back(Tab("Maintenance", TAB_MAINTENANCE, true));
*/
			_tabBuilded = true;
		}
	}
}
