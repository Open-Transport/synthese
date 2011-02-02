
//////////////////////////////////////////////////////////////////////////
/// VehicleAdmin class implementation.
///	@file VehicleAdmin.cpp
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

#include "VehicleAdmin.hpp"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTOperationModule.hpp"
#include "PropertiesHTMLTable.h"
#include "Vehicle.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "VehicleUpdateAction.hpp"
#include "VehicleTableSync.hpp"

using namespace std;
using namespace boost;


namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt_operation;
	using namespace html;
	

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, VehicleAdmin>::FACTORY_KEY("VehicleAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<VehicleAdmin>::ICON("car.png");
		template<> const string AdminInterfaceElementTemplate<VehicleAdmin>::DEFAULT_TITLE("Vehicule");
	}

	namespace pt_operation
	{
		VehicleAdmin::VehicleAdmin()
			: AdminInterfaceElementTemplate<VehicleAdmin>()
		{ }


		
		void VehicleAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_vehicle = Env::GetOfficialEnv().get<Vehicle>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch(ObjectNotFoundException<Vehicle>&)
			{
				throw AdminParametersException("No such vehicle");
			}
		}



		ParametersMap VehicleAdmin::getParametersMap() const
		{
			ParametersMap m;

			if(_vehicle.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _vehicle->getKey());
			}

			return m;
		}


		
		bool VehicleAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<GlobalRight>(READ);
		}



		void VehicleAdmin::display(
			ostream& stream,
			const admin::AdminRequest& request
		) const	{

			stream << "<h2>Propriétés</h2>";

			AdminActionFunctionRequest<VehicleUpdateAction, VehicleAdmin> updateRequest(request);
			updateRequest.getAction()->setVehicle(const_pointer_cast<Vehicle>(_vehicle));

			PropertiesHTMLTable t(updateRequest.getHTMLForm("update_form"));
			stream << t.open();
			stream << t.cell("Nom", t.getForm().getTextInput(VehicleUpdateAction::PARAMETER_NAME, _vehicle->getName()));
			stream << t.cell("Numéro", t.getForm().getTextInput(VehicleUpdateAction::PARAMETER_NUMBER, _vehicle->getNumber()));
			stream << t.cell("Image", t.getForm().getTextInput(VehicleUpdateAction::PARAMETER_PICTURE, _vehicle->getPicture()));
			stream << t.cell("Places", t.getForm().getTextInput(VehicleUpdateAction::PARAMETER_SEATS, VehicleTableSync::SerializeSeats(_vehicle->getSeats())));
			stream << t.cell("Lignes autorisées", t.getForm().getTextInput(VehicleUpdateAction::PARAMETER_ALLOWED_LINES, VehicleTableSync::SerializeAllowedLines(_vehicle->getAllowedLines())));
			stream << t.close();
		}



		std::string VehicleAdmin::getTitle() const
		{
			return _vehicle.get() ? (_vehicle->getName() + " ("+ _vehicle->getNumber() + ")") : DEFAULT_TITLE;
		}



		bool VehicleAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _vehicle->getKey()  == static_cast<const VehicleAdmin&>(other)._vehicle->getKey();
		}
	}
}


