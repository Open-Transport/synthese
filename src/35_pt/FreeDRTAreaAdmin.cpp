
//////////////////////////////////////////////////////////////////////////
/// FreeDRTAreaAdmin class implementation.
///	@file FreeDRTAreaAdmin.cpp
///	@author Hugues Romain
///	@date 2011
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

#include "FreeDRTAreaAdmin.hpp"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "StaticActionRequest.h"
#include "AdminActionFunctionRequest.hpp"
#include "AjaxVectorFieldEditor.hpp"
#include "RequestException.h"
#include "FreeDRTArea.hpp"
#include "PropertiesHTMLTable.h"
#include "FreeDRTAreaUpdateAction.hpp"
#include "CityListFunction.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, FreeDRTAreaAdmin>::FACTORY_KEY("FreeDRTAreaAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<FreeDRTAreaAdmin>::ICON("shape_handles.png");
		template<> const string AdminInterfaceElementTemplate<FreeDRTAreaAdmin>::DEFAULT_TITLE("Zone TAD liberalisé");
	}

	namespace pt
	{
		const string FreeDRTAreaAdmin::TAB_DEFINITION("definition");
		const string FreeDRTAreaAdmin::TAB_SERVICES("services");



		FreeDRTAreaAdmin::FreeDRTAreaAdmin()
			: AdminInterfaceElementTemplate<FreeDRTAreaAdmin>()
		{ }


		
		void FreeDRTAreaAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_area = Env::GetOfficialEnv().get<FreeDRTArea>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (ObjectNotFoundException<FreeDRTArea>&)
			{
				throw RequestException("No such DRT area");
			}
		}



		ParametersMap FreeDRTAreaAdmin::getParametersMap() const
		{
			ParametersMap m;

			if(_area.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _area->getKey());
			}

			return m;
		}


		
		bool FreeDRTAreaAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void FreeDRTAreaAdmin::display(
			ostream& stream,
			const admin::AdminRequest& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// DEFINITION TAB
			if (openTabContent(stream, TAB_DEFINITION))
			{
				{
					stream << "<h1>Communes entières</h1>";

					StaticActionRequest<FreeDRTAreaUpdateAction> updateRequest(request);
					updateRequest.getAction()->setArea(const_pointer_cast<FreeDRTArea>(_area));

					AjaxVectorFieldEditor::Fields fields;

					// Stop field
					fields.push_back(shared_ptr<AjaxVectorFieldEditor::Field>(
						new AjaxVectorFieldEditor::TextAutoCompleteInputField(
							"Commune",
							FreeDRTAreaUpdateAction::PARAMETER_CITIES,
							string(),
							string(),
							pt_website::CityListFunction::FACTORY_KEY,
							pt_website::CityListFunction::DATA_CITIES,
							pt_website::CityListFunction::DATA_CITY,
							"ct",string(),
							false, false, true, false
						)
					));

					// Creation of the editor
					AjaxVectorFieldEditor editor(
						FreeDRTAreaUpdateAction::PARAMETER_CITIES,
						updateRequest.getURL(),
						fields,
						true
					);

					// Insertion of existing values
					BOOST_FOREACH(const FreeDRTArea::Cities::value_type& city, _area->getCities())
					{
						AjaxVectorFieldEditor::Row row;
						vector<string> field;
						field.push_back(lexical_cast<string>(city->getKey()));
						field.push_back(city->getName());
						row.push_back(field);
						editor.addRow(row);
					}

					editor.display(stream);
				}				

				stream << "<h1>Arrêts isolés supplémentaires</h1>";

				stream << "<p class=\"info\">Non implémenté</p>";

				stream << "<h1>Arrêts isolés supplémentaires</h1>";

				stream << "<p class=\"info\">Non implémenté</p>";

				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<FreeDRTAreaUpdateAction, FreeDRTAreaAdmin> updateRequest(request);
				updateRequest.getAction()->setArea(const_pointer_cast<FreeDRTArea>(_area));

				PropertiesHTMLTable propertiesTable(
					updateRequest.getHTMLForm("properties")
				);
				stream << propertiesTable.open();
				stream << propertiesTable.cell("ID", lexical_cast<string>(_area->getKey()));
				stream << propertiesTable.cell("Nom", propertiesTable.getForm().getTextInput(FreeDRTAreaUpdateAction::PARAMETER_NAME, _area->getName()));
				stream << propertiesTable.close();
			}


			////////////////////////////////////////////////////////////////////
			// SERVICES TAB
			if (openTabContent(stream, TAB_SERVICES))
			{

			}


			////////////////////////////////////////////////////////////////////
			/// END TABS
			// closeTabContent(stream);
		}


		
		AdminInterfaceElement::PageLinks FreeDRTAreaAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			// const FreeDRTAreaAdmin* ua(
			//	dynamic_cast<const FreeDRTAreaAdmin*>(&currentPage)
			// );
			
			// if(ua)
			// {
			//	shared_ptr<FreeDRTAreaAdmin> p(getNewOtherPage<FreeDRTAreaAdmin>());
			//	links.push_back(p);
			// }
			
			return links;
		}


		std::string FreeDRTAreaAdmin::getTitle() const
		{
			return _area.get() ? _area->getName() : DEFAULT_TITLE;
		}



		bool FreeDRTAreaAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _area == static_cast<const FreeDRTAreaAdmin&>(other)._area;
		}



		void FreeDRTAreaAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			_tabs.push_back(Tab("Définition", TAB_DEFINITION, profile.isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));
			_tabs.push_back(Tab("Services", TAB_SERVICES, profile.isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));

			_tabBuilded = true;
		}
}	}
