
/** BaseReservationActionAdmin class implementation.
	@file BaseReservationActionAdmin.cpp

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

#include "BaseReservationActionAdmin.hpp"

#include "AdminParametersException.h"
#include "BookReservationAction.h"
#include "HTMLTable.h"
#include "Profile.h"
#include "ResaCustomerHtmlOptionListFunction.h"
#include "ReservationTransaction.h"
#include "ReservationTransactionTableSync.h"
#include "Reservation.h"
#include "ReservationTableSync.h"
#include "StaticFunctionRequest.h"
#include "User.h"
#include "UserTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace html;
	using namespace security;
	using namespace server;
	using namespace util;

	namespace resa
	{
		const std::string BaseReservationActionAdmin::PARAMETER_CUSTOMER_ID = "cu";
		const std::string BaseReservationActionAdmin::PARAMETER_SEATS_NUMBER = "sn";



		BaseReservationActionAdmin::BaseReservationActionAdmin(
		):	_seatsNumber(1)
		{}



		void BaseReservationActionAdmin::setBaseReservationFromParametersMap(
			Env& env,
			const util::ParametersMap& map
		){

			if(	map.getOptional<RegistryKeyType>(Request::PARAMETER_OBJECT_ID) &&
				decodeTableId(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)) == ReservationTransactionTableSync::TABLE.ID
			){
				try
				{
					_confirmedTransaction = ReservationTransactionTableSync::GetEditable(
						map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
						env
					);
					ReservationTableSync::SearchResult reservations(
						ReservationTableSync::Search(env, _confirmedTransaction->getKey())
					);
				}
				catch (...)
				{
					throw AdminParametersException("Reservation load error");
				}
			}

			if(map.getOptional<RegistryKeyType>(PARAMETER_CUSTOMER_ID))
			{
				try
				{
					_customer = UserTableSync::Get(
						map.get<RegistryKeyType>(PARAMETER_CUSTOMER_ID),
						env
					);
				}
				catch (...)
				{
					throw AdminParametersException("No such User");
				}
			}

		}



		util::ParametersMap BaseReservationActionAdmin::getBaseReservationParametersMap() const
		{
			ParametersMap m;

			if(_customer.get())
			{
				m.insert(PARAMETER_CUSTOMER_ID, _customer->getKey());
			}
			m.insert(PARAMETER_SEATS_NUMBER, _seatsNumber);

			return m;
		}



		void BaseReservationActionAdmin::displayReservationForm(
			std::ostream& stream,
			html::HTMLForm& rf,
			const server::Request& request
		) const	{
			HTMLTable rt(2,"propertysheet");

			StaticFunctionRequest<ResaCustomerHtmlOptionListFunction> customerSearchRequest(request, true);
			customerSearchRequest.getFunction()->setNumber(20);

			stream << HTMLModule::GetHTMLJavascriptOpen("resa.js");

			stream << rt.open();

			if (_customer.get())
			{
				stream << rt.row();
				stream << rt.col() << "Client";
				stream << rt.col() << _customer->getFullName() << " (" << _customer->getPhone() << ")";
				rf.addHiddenField(
					BookReservationAction::PARAMETER_CUSTOMER_ID,
					lexical_cast<string>(_customer->getKey())
				);

				ParametersMap withoutClientPM(request.getParametersMap());
				withoutClientPM.remove(PARAMETER_CUSTOMER_ID);

				stringstream os;
				os << request.getClientURL() + Request::PARAMETER_STARTER;
				withoutClientPM.outputURI(os);
				stream << HTMLModule::getLinkButton(os.str(), "Autre client");
			}
			else
			{
				stream << rt.row();
				stream << rt.col() << "Recherche client";
				stream << rt.col() << "Nom : " << rf.getTextInput(BookReservationAction::PARAMETER_CUSTOMER_NAME, string());
				stream << "	Prénom : " << rf.getTextInput(BookReservationAction::PARAMETER_CUSTOMER_SURNAME, string());
				stream << " Téléphone : " << rf.getTextInput(BookReservationAction::PARAMETER_CUSTOMER_PHONE, string());

				stream << HTMLModule::GetHTMLJavascriptOpen();
				stream
					<< "document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_NAME) << "').onkeyup = "
					<< "function(){ programCustomerUpdate("
					<< "'" << rf.getName() << "'"
					<< ",'" << BookReservationAction::PARAMETER_CREATE_CUSTOMER << "'"
					<< ",'ie_bug_curstomer_div'"
					<< ",'" << BookReservationAction::PARAMETER_CUSTOMER_ID << "'"
					<< ",'" << customerSearchRequest.getURL()
					<< "&" << ResaCustomerHtmlOptionListFunction::PARAMETER_NAME <<"='+document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_NAME) << "').value"
					<< "+'&" << ResaCustomerHtmlOptionListFunction::PARAMETER_SURNAME <<"='+document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_SURNAME) << "').value"
					<< "+'&" << ResaCustomerHtmlOptionListFunction::PARAMETER_PHONE <<"='+document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_PHONE) << "').value"
					<< "); };";
				stream << "document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_SURNAME) << "').onkeyup = "
					<< "function(){ programCustomerUpdate("
					<< "'" << rf.getName() << "'"
					<< ",'" << BookReservationAction::PARAMETER_CREATE_CUSTOMER << "'"
					<< ",'ie_bug_curstomer_div'"
					<< ",'" << BookReservationAction::PARAMETER_CUSTOMER_ID << "'"
					<< ",'" << customerSearchRequest.getURL()
					<< "&" << ResaCustomerHtmlOptionListFunction::PARAMETER_NAME <<"='+document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_NAME) << "').value"
					<< "+'&" << ResaCustomerHtmlOptionListFunction::PARAMETER_SURNAME <<"='+document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_SURNAME) << "').value"
					<< "+'&" << ResaCustomerHtmlOptionListFunction::PARAMETER_PHONE <<"='+document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_PHONE) << "').value"
					<< "); };";
				stream << "document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_PHONE) << "').onkeyup = "
					<< "function(){ programCustomerUpdate("
					<< "'" << rf.getName() << "'"
					<< ",'" << BookReservationAction::PARAMETER_CREATE_CUSTOMER << "'"
					<< ",'ie_bug_curstomer_div'"
					<< ",'" << BookReservationAction::PARAMETER_CUSTOMER_ID << "'"
					<< ",'" << customerSearchRequest.getURL()
					<< "&" << ResaCustomerHtmlOptionListFunction::PARAMETER_NAME <<"='+document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_NAME) << "').value"
					<< "+'&" << ResaCustomerHtmlOptionListFunction::PARAMETER_SURNAME <<"='+document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_SURNAME) << "').value"
					<< "+'&" << ResaCustomerHtmlOptionListFunction::PARAMETER_PHONE <<"='+document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_PHONE) << "').value"
					<< "); };";
				stream <<
					"document.forms." << rf.getName() << ".onsubmit = " <<
					"function(){" <<
					"document.getElementById('" << rf.getFieldId(PARAMETER_SEATS_NUMBER) << "').value=" <<
					"document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_SEATS_NUMBER) << "').value;" <<
					"document.getElementById('" << rf.getFieldId(PARAMETER_CUSTOMER_ID) << "').value=" <<
					"document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_ID) << "').value;" <<
					"};"
				;

				stream << HTMLModule::GetHTMLJavascriptClose();

				stream << rt.row();
				stream << rt.col() << "Client";
				stream << rt.col() << rf.getRadioInput(
					BookReservationAction::PARAMETER_CREATE_CUSTOMER,
					optional<bool>(true),
					optional<bool>(true),
					"Nouveau client"
				);
				stream << " : Téléphone : " << rf.getTextInput(BookReservationAction::PARAMETER_CUSTOMER_PHONE, string());
				stream << "	E-mail : " << rf.getTextInput(BookReservationAction::PARAMETER_CUSTOMER_EMAIL, string());
				stream << "<br />" << rf.getRadioInput(
					BookReservationAction::PARAMETER_CREATE_CUSTOMER,
					optional<bool>(false),
					optional<bool>(true),
					"Client existant",
					true
				);
				stream << "<span id=\"ie_bug_curstomer_div\"></span>";
			}


			stream << rt.row();
			stream << rt.col() << "Nombre de places";
			stream << rt.col() <<
				rf.getTextInput(
					BookReservationAction::PARAMETER_SEATS_NUMBER,
					lexical_cast<string>(_seatsNumber)
				)
			;

			stream << rt.row();
			stream << rt.col() << "Commentaire";
			stream << rt.col() << rf.getTextInput(BookReservationAction::PARAMETER_COMMENT, string());

			stream << rt.row();
			stream << rt.col(2, string(), true) << rf.getSubmitButton("Réserver");

			stream << rt.close();
		}
}	}
