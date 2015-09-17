
//////////////////////////////////////////////////////////////////////////
/// FareAdmin class implementation.
///	@file FareAdmin.cpp
///	@author Gaël Sauvanet
///	@date 2011
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

#include "FareAdmin.hpp"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTModule.h"
#include "User.h"
#include "Fare.hpp"
#include "FareType.hpp"
#include "FareUpdateAction.hpp"
#include "PropertiesHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"
#include "AjaxVectorFieldEditor.hpp"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace fare;
	using namespace html;


	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, FareAdmin>::FACTORY_KEY("FareAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<FareAdmin>::ICON("database_edit.png");
		template<> const string AdminInterfaceElementTemplate<FareAdmin>::DEFAULT_TITLE("Tarification");
	}

	namespace fare
	{
		FareAdmin::FareAdmin()
			: AdminInterfaceElementTemplate<FareAdmin>()
		{ }



		void FareAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_fare = Env::GetOfficialEnv().get<Fare>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch(ObjectNotFoundException<Fare>&)
			{
				throw AdminParametersException("No such fare");
			}
		}



		ParametersMap FareAdmin::getParametersMap() const
		{
			ParametersMap m;

			if(_fare.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _fare->getKey());
			}

			return m;
		}



		bool FareAdmin::isAuthorized(
			const security::User& user
		) const	{
			return true; // TODO create fare right
		}



		void FareAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			{
				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<FareUpdateAction, FareAdmin> updateRequest(request, *this);
				updateRequest.getAction()->setFare(const_pointer_cast<Fare>(_fare));

				// General properties
				PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
				stream << t.open();
				stream << t.cell("ID", lexical_cast<string>(_fare->getKey()));
				stream << t.cell("Nom", t.getForm().GetTextInput(FareUpdateAction::PARAMETER_NAME, _fare->getName()));
				stream << t.cell("Type", _fare->getType() ? FareType::GetTypeName(_fare->getType()->getTypeNumber()) : string());
				stream << t.cell("Monnaie", t.getForm().GetTextInput(FareUpdateAction::PARAMETER_CURRENCY, _fare->get<Currency>()));
				stream << t.cell("Nombre de correspondances autorisées (vide : illimité)",
					t.getForm().getTextInput(
						FareUpdateAction::PARAMETER_PERMITTED_CONNECTIONS_NUMBER,
						(_fare->get<PermittedConnectionsNumber>() != 0) ? lexical_cast<string>(_fare->get<PermittedConnectionsNumber>()) : string()
				)	);
				stream << t.cell("Continuité obligatoire", t.getForm().getOuiNonRadioInput(FareUpdateAction::PARAMETER_REQUIRED_CONTINUITY, _fare->get<RequiredContinuity>()));
				stream << t.cell("Durée de validité (en minutes)", t.getForm().getTextInput(FareUpdateAction::PARAMETER_VALIDITY_PERIOD, lexical_cast<string>(_fare->get<ValidityPeriod>())));

				// Specific properties
				if(_fare->getType())
				{
					if(_fare->getType()->getTypeNumber() == FareType::FARE_TYPE_FLAT_RATE)
					{
						stream << t.title("Propriétés de la tarification forfaitaire");
						stream << t.cell("Prix du billet",
								t.getForm().getTextInput(
									FareUpdateAction::PARAMETER_ACCESS_PRICE,
									lexical_cast<string>(_fare->getType()->getAccessPrice())
								));
					}
					else if(_fare->getType()->getTypeNumber() == FareType::FARE_TYPE_DISTANCE)
					{
						stream << t.title("Propriétés de la tarification kilométrique");
						stream << t.cell("Prix d'accès",
								t.getForm().getTextInput(
									FareUpdateAction::PARAMETER_ACCESS_PRICE,
									lexical_cast<string>(_fare->getType()->getAccessPrice())
								));
					}
				}
				stream << t.close();

				StaticActionRequest<FareUpdateAction> updateOnlyRequest(request);
				updateOnlyRequest.getAction()->setFare(const_pointer_cast<Fare>(_fare));

				// Slices
				if(_fare->getType())
				{
					if(_fare->getType()->getTypeNumber() == FareType::FARE_TYPE_DISTANCE)
					{
						stream << "<h1>Définition des tranches</h1>";

						AjaxVectorFieldEditor::Fields fields;
						fields.push_back(boost::shared_ptr<AjaxVectorFieldEditor::Field>(new AjaxVectorFieldEditor::TextInputField("Seuil bas tranche (km)")));
						fields.push_back(boost::shared_ptr<AjaxVectorFieldEditor::Field>(new AjaxVectorFieldEditor::TextInputField("Seuil haut tranche (km)")));
						fields.push_back(boost::shared_ptr<AjaxVectorFieldEditor::Field>(new AjaxVectorFieldEditor::TextInputField("Prix par km")));

						// Creation of the editor
						AjaxVectorFieldEditor editor(
							FareUpdateAction::PARAMETER_SLICES,
							updateOnlyRequest.getURL(),
							fields,
							false
						);

						// Insertion of existing values
						BOOST_FOREACH(const FareType::Slice& slice, _fare->getType()->getSlices())
						{
							AjaxVectorFieldEditor::Row row;
							row.push_back(vector<string>(1,lexical_cast<string>(slice.min)));
							row.push_back(vector<string>(1,lexical_cast<string>(slice.max)));
							row.push_back(vector<string>(1,lexical_cast<string>(slice.price)));
							editor.addRow(row);
						}

						editor.display(stream);
					}
				}
			}
		}



		std::string FareAdmin::getTitle() const
		{
			return _fare.get() ? _fare->getName() : DEFAULT_TITLE;
		}



		bool FareAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _fare->getKey() == static_cast<const FareAdmin&>(other)._fare->getKey();
		}
}	}
