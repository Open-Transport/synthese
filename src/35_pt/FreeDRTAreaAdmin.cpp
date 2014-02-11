
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
#include "Profile.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "StaticActionRequest.h"
#include "AdminActionFunctionRequest.hpp"
#include "AjaxVectorFieldEditor.hpp"
#include "RequestException.h"
#include "FreeDRTArea.hpp"
#include "PropertiesHTMLTable.h"
#include "FreeDRTAreaUpdateAction.hpp"
#include "CityListFunction.h"
#include "PlacesListFunction.h"
#include "AdminFunctionRequest.hpp"
#include "FreeDRTTimeSlotAdmin.hpp"
#include "FreeDRTTimeSlotUpdateAction.hpp"
#include "RemoveObjectAction.hpp"
#include "FreeDRTTimeSlotTableSync.hpp"
#include "CommercialLineAdmin.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace graph;
	using namespace db;
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, FreeDRTAreaAdmin>::FACTORY_KEY("FreeDRTArea");
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
			const server::Request& request
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

					// Cities field
					fields.push_back(boost::shared_ptr<AjaxVectorFieldEditor::Field>(
						new AjaxVectorFieldEditor::TextAutoCompleteInputField(
							"Commune",
							FreeDRTAreaUpdateAction::PARAMETER_CITIES,
							string(),
							string(),
							pt_website::CityListFunction::FACTORY_KEY,
							pt_website::CityListFunction::DATA_CITIES,
							pt_website::CityListFunction::DATA_CITY,
							string(),
							string(),
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

				{
					stream << "<h1>Arrêts isolés</h1>";
					StaticActionRequest<FreeDRTAreaUpdateAction> updateRequest(request);
					updateRequest.getAction()->setArea(const_pointer_cast<FreeDRTArea>(_area));

					AjaxVectorFieldEditor::Fields fields;

					// Stops field
					fields.push_back(boost::shared_ptr<AjaxVectorFieldEditor::Field>(
						new AjaxVectorFieldEditor::TextAutoCompleteInputField(
							"Arrêt",
							FreeDRTAreaUpdateAction::PARAMETER_STOP_AREAS,
							string(),
							string(),
							pt_website::PlacesListFunction::FACTORY_KEY,
							pt_website::PlacesListFunction::DATA_PLACES,
							pt_website::PlacesListFunction::DATA_PLACE,
							pt_website::PlacesListFunction::PARAMETER_CITY_TEXT,
							string(),
							false, false, true, false
						)
					));

					// Creation of the editor
					AjaxVectorFieldEditor editor(
						FreeDRTAreaUpdateAction::PARAMETER_STOP_AREAS,
						updateRequest.getURL(),
						fields,
						true
					);

					// Insertion of existing values
					BOOST_FOREACH(const FreeDRTArea::StopAreas::value_type& stopArea, _area->getStopAreas())
					{
						AjaxVectorFieldEditor::Row row;
						vector<string> field;
						field.push_back(lexical_cast<string>(stopArea->getKey()));
						field.push_back(stopArea->getFullName());
						row.push_back(field);
						editor.addRow(row);
					}

					editor.display(stream);
				}

				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<FreeDRTAreaUpdateAction, FreeDRTAreaAdmin> updateRequest(request, *this);
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
				// Declarations
				AdminFunctionRequest<FreeDRTTimeSlotAdmin> openRequest(request);
				AdminActionFunctionRequest<RemoveObjectAction, FreeDRTAreaAdmin> removeRequest(request, *this);

				// Search for services
				AdminFunctionRequest<FreeDRTAreaAdmin> searchRequest(request, *this);
				FreeDRTTimeSlotTableSync::SearchResult services(
					FreeDRTTimeSlotTableSync::Search(
						Env::GetOfficialEnv(),
						_area->getKey(),
						_requestParameters.first,
						_requestParameters.maxSize,
						true,
						_requestParameters.raisingOrder
				)	);

				// Add request and form
				AdminActionFunctionRequest<FreeDRTTimeSlotUpdateAction,FreeDRTTimeSlotAdmin> addRequest(request);
				addRequest.setActionWillCreateObject();
				addRequest.getAction()->setArea(const_pointer_cast<FreeDRTArea>(_area));
				HTMLForm addForm(addRequest.getHTMLForm("addDRTTimeslot"));
				stream << addForm.open();

				// Table initialization
				ResultHTMLTable::HeaderVector cols;
				cols.push_back(make_pair(string(), string()));
				cols.push_back(make_pair(string(), "Num"));
				cols.push_back(make_pair(string(), "Heure début"));
				cols.push_back(make_pair(string(), "Heure fin"));
				cols.push_back(make_pair(string(), string()));
				ResultHTMLTable t(
					cols,
					searchRequest.getHTMLForm(),
					_requestParameters,
					services
				);
				stream << t.open();
				BOOST_FOREACH(const Service* itService, _area->getServices())
				{
					// Declarations
					const FreeDRTTimeSlot& service(static_cast<const FreeDRTTimeSlot&>(*itService));
					openRequest.getPage()->setTimeSlot(Env::GetOfficialEnv().getEditableSPtr(const_cast<FreeDRTTimeSlot*>(&service)));
					removeRequest.getAction()->setObjectId(service.getKey());

					// New row
					stream << t.row();

					// Open button cell
					stream << t.col();
					stream << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + FreeDRTTimeSlotAdmin::ICON);

					// Name cell
					stream << t.col();
					stream << service.getServiceNumber();

					// First departure cell
					stream << t.col();
					stream << service.getFirstDeparture();

					// Last arrival cell
					stream << t.col();
					stream << service.getLastArrival();

					// Remove cell
					stream << t.col();
					stream << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer la zone "+ service.getServiceNumber() + " ?");
				}

				// New row
				stream << t.row();

				// Empty cell
				stream << t.col();

				// Name field
				stream << t.col() << addForm.getTextInput(FreeDRTTimeSlotUpdateAction::PARAMETER_SERVICE_NUMBER, string(), "(numéro)");

				// First departure field
				stream << t.col() << addForm.getTextInput(FreeDRTTimeSlotUpdateAction::PARAMETER_FIRST_DEPARTURE, string());

				// Last arrival field
				stream << t.col() << addForm.getTextInput(FreeDRTTimeSlotUpdateAction::PARAMETER_LAST_ARRIVAL, string());

				// Add button
				stream << t.col() << addForm.getSubmitButton("Ajouter");

				// Table closing
				stream << t.close() << addForm.close();
			}


			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		AdminInterfaceElement::PageLinks FreeDRTAreaAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	currentPage == *this ||
				currentPage.getCurrentTreeBranch().find(*this)
			){
				// Free DRT areas
				FreeDRTTimeSlotTableSync::SearchResult timeSlots(
					FreeDRTTimeSlotTableSync::Search(Env::GetOfficialEnv(), _area->getKey())
				);
				BOOST_FOREACH(const boost::shared_ptr<FreeDRTTimeSlot>& timeSlot, timeSlots)
				{
					boost::shared_ptr<FreeDRTTimeSlotAdmin> p(
						getNewPage<FreeDRTTimeSlotAdmin>()
					);
					p->setTimeSlot(timeSlot);
					links.push_back(p);
				}
			}

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



		AdminInterfaceElement::PageLinks FreeDRTAreaAdmin::_getCurrentTreeBranch() const
		{
			boost::shared_ptr<CommercialLineAdmin> p(
				getNewPage<CommercialLineAdmin>()
			);
			p->setCommercialLine(Env::GetOfficialEnv().getSPtr(_area->getLine()));

			PageLinks links(p->_getCurrentTreeBranch());
			links.push_back(getNewCopiedPage());
			return links;
		}
}	}
