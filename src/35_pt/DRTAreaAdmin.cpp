
//////////////////////////////////////////////////////////////////////////
/// DRTAreaAdmin class implementation.
///	@file DRTAreaAdmin.cpp
///	@author RCSobility
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

#include "DRTAreaAdmin.hpp"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "DRTArea.hpp"
#include "DRTAreaUpdateAction.hpp"
#include "PropertiesHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"
#include "AjaxVectorFieldEditor.hpp"
#include "StopArea.hpp"

#include <boost/lexical_cast.hpp>

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
		template<> const string FactorableTemplate<AdminInterfaceElement, DRTAreaAdmin>::FACTORY_KEY("DRTAreaAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DRTAreaAdmin>::ICON("shape_handles.png");
		template<> const string AdminInterfaceElementTemplate<DRTAreaAdmin>::DEFAULT_TITLE("Zone");
	}

	namespace pt
	{
		DRTAreaAdmin::DRTAreaAdmin()
			: AdminInterfaceElementTemplate<DRTAreaAdmin>()
		{ }



		void DRTAreaAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_area = Env::GetOfficialEnv().get<DRTArea>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch(ObjectNotFoundException<DRTArea>&)
			{
				throw AdminParametersException("No such area");
			}

		}



		ParametersMap DRTAreaAdmin::getParametersMap() const
		{
			ParametersMap m;

			if(_area.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _area->getKey());
			}

			return m;
		}



		bool DRTAreaAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void DRTAreaAdmin::display(
			ostream& stream,
			const admin::AdminRequest& request
		) const	{

			{
				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<DRTAreaUpdateAction, DRTAreaAdmin> updateRequest(request);
				updateRequest.getAction()->setArea(const_pointer_cast<DRTArea>(_area));

				PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
				stream << t.open();
				stream << t.cell("ID", lexical_cast<string>(_area->getKey()));
				stream << t.cell("Nom", t.getForm().GetTextInput(DRTAreaUpdateAction::PARAMETER_NAME, _area->getName()));
				stream << t.close();
			}

			{
				stream << "<h1>Arrêts</h1>";

				StaticActionRequest<DRTAreaUpdateAction> updateRequest(request);
				updateRequest.getAction()->setArea(const_pointer_cast<DRTArea>(_area));

				AjaxVectorFieldEditor::Fields fields;

				// Stop field
				fields.push_back(shared_ptr<AjaxVectorFieldEditor::Field>(new AjaxVectorFieldEditor::TextInputField("Arrêt")));

				// Creation of the editor
				AjaxVectorFieldEditor editor(
					DRTAreaUpdateAction::PARAMETER_STOPS,
					updateRequest.getURL(),
					fields,
					true
				);

				// Insertion of existing values
				BOOST_FOREACH(const DRTArea::Stops::value_type& stop, _area->getStops())
				{
					AjaxVectorFieldEditor::Row row;
					row.push_back(lexical_cast<string>(stop->getKey()));
					editor.addRow(row);
				}

				editor.display(stream);
			}
		}



		std::string DRTAreaAdmin::getTitle() const
		{
			return _area.get() ? _area->getName() : DEFAULT_TITLE;
		}



		bool DRTAreaAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _area->getKey() == static_cast<const DRTAreaAdmin&>(other)._area->getKey();
		}
}	}
