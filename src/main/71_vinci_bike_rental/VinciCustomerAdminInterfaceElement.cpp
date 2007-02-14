
/** VinciCustomerAdminInterfaceElement class implementation.
	@file VinciCustomerAdminInterfaceElement.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "32_admin/AdminRequest.h"

#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"
#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"

#include "71_vinci_bike_rental/RentABikeAction.h"
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"
#include "71_vinci_bike_rental/VinciRate.h"
#include "71_vinci_bike_rental/VinciRateTableSync.h"
#include "71_vinci_bike_rental/VinciBike.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"
#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
#include "71_vinci_bike_rental/VinciUpdateCustomerAction.h"
#include "71_vinci_bike_rental/VinciAddGuaranteeAction.h"
#include "71_vinci_bike_rental/VinciCustomerAdminInterfaceElement.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace security;
	using namespace accounts;

	namespace vinci
	{
		VinciCustomerAdminInterfaceElement::VinciCustomerAdminInterfaceElement()
			: AdminInterfaceElement("vincicustomers", AdminInterfaceElement::DISPLAYED_IF_CURRENT) 
			, _contract(NULL), _user(NULL)		
		{}


		std::string VinciCustomerAdminInterfaceElement::getTitle() const
		{
			return "Client " + _user->getSurname() + " " + _user->getName();
		}

		void VinciCustomerAdminInterfaceElement::display(std::ostream& stream, const AdminRequest* request /*= NULL*/ ) const
		{
			// Update user request
			AdminRequest* updateRequest = Factory<Request>::create<AdminRequest>();
			updateRequest->copy(request);
			updateRequest->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerAdminInterfaceElement>());
			updateRequest->setAction(Factory<Action>::create<VinciUpdateCustomerAction>());

			// Add guarantee request
			AdminRequest* addGuaranteeRequest = Factory<Request>::create<AdminRequest>();
			addGuaranteeRequest->copy(request);
			addGuaranteeRequest->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerAdminInterfaceElement>());
			addGuaranteeRequest->setAction(Factory<Action>::create<VinciAddGuaranteeAction>());

			// Add Rent request
			AdminRequest* addRentRequest = Factory<Request>::create<AdminRequest>();
			addRentRequest->copy(request);
			addRentRequest->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerAdminInterfaceElement>());
			addRentRequest->setAction(Factory<Action>::create<RentABikeAction>());

			// Personal Data
			stream
				<< "<h1>Coordonnées</h1>"
				<< updateRequest->getHTMLFormHeader("update")
				<< "<input type=\"hidden\" name=\"" << Request::PARAMETER_OBJECT_ID << "\" value=\"" << Conversion::ToString(_contract->getKey()) << "\" />"
				<< "<input type=\"hidden\" name=\"" << VinciUpdateCustomerAction::PARAMETER_ID << "\" value=\"" << Conversion::ToString(_contract->getKey()) << "\" />"
				<< "<table>"
				<< "<tr><td>Nom</td><td><input name=\"" << VinciUpdateCustomerAction::PARAMETER_NAME << "\" value=\"" << _user->getName() << "\" /></td></tr>"
				<< "<tr><td>Prénom</td><td><input name=\"" << VinciUpdateCustomerAction::PARAMETER_SURNAME << "\" value=\"" << _user->getSurname() << "\" /></td></tr>"
				<< "<tr><td>Adresse</td><td><input name=\"" << VinciUpdateCustomerAction::PARAMETER_ADDRESS << "\" value=\"" << _user->getAddress() << "\" /></td></tr>"
				<< "<tr><td>Code postal</td><td><input name=\"" << VinciUpdateCustomerAction::PARAMETER_POST_CODE << "\" value=\"" << _user->getPostCode() << "\" /></td></tr>"
				<< "<tr><td>Commune</td><td><input name=\"" << VinciUpdateCustomerAction::PARAMETER_CITY << "\" value=\"" << _user->getCityText() << "\" /></td></tr>"
				<< "<tr><td>Pays</td><td><input name=\"" << VinciUpdateCustomerAction::PARAMETER_COUNTRY << "\" value=\"" << _user->getCountry() << "\" /></td></tr>"
				<< "<tr><td>E-mail</td><td><input name=\"" << VinciUpdateCustomerAction::PARAMETER_EMAIL << "\" value=\"" << _user->getEMail() << "\" /></td></tr>"
				<< "<tr><td>Téléphone</td><td><input name=\"" << VinciUpdateCustomerAction::PARAMETER_PHONE << "\" value=\"" << _user->getPhone() << "\" /></td></tr>"
				<< "<tr><td colspan=\"2\"><input type=\"submit\" value=\"Modifier\" /></td>"
				<< "</table></form>"
				;

			// Guarantees
			vector<TransactionPart*> guarantees = TransactionPartTableSync::search(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE), _user);

			stream
				<< "<h1>Cautions</h1>"
				<< addGuaranteeRequest->getHTMLFormHeader("addguarantee")
				<< "<input type=\"hidden\" name=\"" << AdminRequest::PARAMETER_OBJECT_ID << "\" value=\"" << _contract->getKey() << "\" />"
				<< "<input type=\"hidden\" name=\"" << VinciAddGuaranteeAction::PARAMETER_CONTRACT_ID << "\" value=\"" << _contract->getKey() << "\" />"
				<< "<table>"
				<< "<tr><th>Date</th><th>Montant</th><th>Nature</th><th>Actions</th></tr>"
				;
			for (vector<TransactionPart*>::iterator it = guarantees.begin(); it != guarantees.end(); ++it)
			{
				Transaction* transaction = TransactionTableSync::get((*it)->getTransactionId());
				vector<TransactionPart*> payments = TransactionPartTableSync::search(transaction);

				stream
					<< "<tr>"
					<< "<td>" << transaction->getStartDateTime().toString() << "</td>"
					<< "<td>" << Conversion::ToString((*it)->getRightCurrencyAmount()) << "</td>"
					<< "<td>";
				for (vector<TransactionPart*>::iterator it2 = payments.begin(); it2 != payments.end(); ++it2)
				{
					if ((*it2)->getKey() != (*it)->getKey())
					{
						Account* account = AccountTableSync::get((*it2)->getAccountId());
						if (account->getRightClassNumber() == VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE)
							stream << "Chèque";
						else
							stream << "Carte";
						delete account;
					}
					delete (*it2);
				}
				stream
					<< "</td>"
					<< "<td>"
					;
				if (transaction->getEndDateTime().isUnknown())
					stream << "Rendre la caution";
				else
					stream << "Caution rendue le " << transaction->getEndDateTime().toSQLString(false);

				stream
					<< "</td>"
					<< "</tr>";

				delete transaction;
				delete *it;
			}

			Account* checkAccount = VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE);
			Account* cardAccount = VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE);
			stream
				<< "<tr><td><input type=\"submit\" value=\"Nouvelle caution\" /></td>"
				<< "<td><input name=\"" << VinciAddGuaranteeAction::PARAMETER_AMOUNT << "\" value=\"260\" /></td>"
				<< "<td><select name=\"" << VinciAddGuaranteeAction::PARAMETER_ACCOUNT_ID << "\">"
				<< "<option value=\"" << checkAccount->getKey() << "\">Chèque</option>"
				<< "<option value=\"" << cardAccount->getKey() << "\">Carte</option>"
				<< "</select></td>"
				<< "<td></td></tr>"
				<< "</table></form>"
				;

			// Rents
			vector<TransactionPart*> rents = TransactionPartTableSync::search(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE), _user);
			vector<VinciRate*> rates = VinciRateTableSync::search();
			stream
				<< "<h1>Locations</h1>"
				<< addRentRequest->getHTMLFormHeader("addrent")
				<< "<input type=\"hidden\" name=\"" << AdminRequest::PARAMETER_OBJECT_ID << "\" value=\"" << _contract->getKey() << "\" />"
				<< "<input type=\"hidden\" name=\"" << RentABikeAction::PARAMETER_CONTRACT_ID << "\" value=\"" << _contract->getKey() << "\" />"
				<< "<table>"
				<< "<tr><th>Date</th><th>Vélo</th><th>Tarif</th></tr>"
				;
			for (vector<TransactionPart*>::iterator it = rents.begin(); it != rents.end(); ++it)
			{
				Transaction* transaction = TransactionTableSync::get((*it)->getTransactionId());
								
				VinciRate* rate = NULL;
				if ((*it)->getRateId() > 0)
					rate = VinciRateTableSync::get((*it)->getRateId());

				VinciBike* bike = NULL;
				if ((*it)->getTradedObjectId() != "")
					bike = VinciBikeTableSync::get(Conversion::ToLongLong((*it)->getTradedObjectId()));

				stream
					<< "<tr>"
					<< "<td>" << transaction->getStartDateTime().toString() << "</td>"
					<< "<td>" << ((bike == NULL) ? "Non renseign&eacute;" : bike->getNumber()) << "</td>"
					<< "<td>" << ((rate == NULL) ? "Non renseign&eacute;" : rate->getName())  << "</td>"
					<< "</tr>"
					;

				delete bike;
				delete rate;
			}
			stream
				<< "<tr>"
				<< "<td><input type=\"submit\" value=\"Nouvelle location\" /></td>"
				<< "<td><input size=\"10\" name=\"" << RentABikeAction::PARAMETER_BIKE_ID << "\" /></td>"
				<< "<td><select name=\"" << RentABikeAction::PARAMETER_RATE_ID << "\">"
				;
			for (vector<VinciRate*>::iterator it = rates.begin(); it != rates.end(); ++it)
				stream << "<option value=\"" << Conversion::ToString((*it)->getKey()) << "\">" << (*it)->getName() << "</option>";
			stream
				<< "</select></td>"
				<< "</tr>";
				;



			stream
				<< "</table></form>"
				;


			// Change
			stream
				<< "<h1>Compte client</h1>"
				;

			// Cleaning
			delete updateRequest;

		}

		void VinciCustomerAdminInterfaceElement::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			// Current contract
			const server::Request::ParametersMap::const_iterator it = map.find(Request::PARAMETER_OBJECT_ID);
			if (it != map.end())
			{
				_contract = VinciContractTableSync::get(Conversion::ToLongLong(it->second));
				_user = UserTableSync::get(_contract->getUserId());
			}
		}

		VinciCustomerAdminInterfaceElement::~VinciCustomerAdminInterfaceElement()
		{
			delete _contract;
			delete _user;
		}
	}
}
