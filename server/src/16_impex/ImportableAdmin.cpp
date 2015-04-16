
/** ImportableAdmin class implementation.
	@file ImportableAdmin.cpp

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

#include "ImportableAdmin.hpp"
#include "AjaxVectorFieldEditor.hpp"
#include "Importable.h"
#include "Env.h"
#include "DataSource.h"
#include "Request.h"
#include "Action.h"
#include "BaseImportableUpdateAction.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace html;
	using namespace util;

	namespace impex
	{
		const string ImportableAdmin::TAB_DATA_SOURCES = "tab_data_sources";



		void ImportableAdmin::DisplayDataSourcesTab(
			ostream& stream,
			const Importable& object,
			const Request& updateRequest
		){
			stream << "<h1>Sources de données</h1>";

			AjaxVectorFieldEditor::Fields fields;

			// Source selector field
			AjaxVectorFieldEditor::SelectField::Choices choices;
			choices.push_back(make_pair(RegistryKeyType(0), "(Toutes sources)"));
			BOOST_FOREACH(const Registry<DataSource>::value_type& source, Env::GetOfficialEnv().getRegistry<DataSource>())
			{
				choices.push_back(make_pair(source.first, source.second->get<Name>()));
			}
			fields.push_back(boost::shared_ptr<AjaxVectorFieldEditor::Field>(new AjaxVectorFieldEditor::SelectField("Source", false, choices)));

			// Code field
			fields.push_back(boost::shared_ptr<AjaxVectorFieldEditor::Field>(new AjaxVectorFieldEditor::TextInputField("Code")));

			// Creation of the editor
			AjaxVectorFieldEditor editor(
				BaseImportableUpdateAction::PARAMETER_DATA_SOURCE_LINKS,
				updateRequest.getURL(),
				fields,
				true
			);

			// Insertion of existing values
			BOOST_FOREACH(const Importable::DataSourceLinks::value_type& link, object.getDataSourceLinks())
			{
				AjaxVectorFieldEditor::Row row;
				row.push_back(vector<string>(1,(link.first ? lexical_cast<string>(link.first->getKey()) : string())));
				row.push_back(vector<string>(1,link.second));
				editor.addRow(row);
			}

			editor.display(stream);
		}
}	}
