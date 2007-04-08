
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

#include "05_html/HTMLForm.h"
#include "05_html/HTMLTable.h"

#include "11_interfaces/DurationInterfacePage.h"
#include "11_interfaces/Interface.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "30_server/ActionFunctionRequest.h"

#include "32_admin/AdminRequest.h"

#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"
#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"

#include "71_vinci_bike_rental/RentABikeAction.h"
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"
#include "71_vinci_bike_rental/VinciAntivol.h"
#include "71_vinci_bike_rental/VinciAntivolTableSync.h"
#include "71_vinci_bike_rental/VinciRate.h"
#include "71_vinci_bike_rental/VinciRateTableSync.h"
#include "71_vinci_bike_rental/VinciBike.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"
#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
#include "71_vinci_bike_rental/VinciUpdateCustomerAction.h"
#include "71_vinci_bike_rental/VinciAddGuaranteeAction.h"
#include "71_vinci_bike_rental/VinciCustomerAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciContractPrintRequest.h"
#include "71_vinci_bike_rental/VinciReturnGuaranteeAction.h"
#include "71_vinci_bike_rental/ReturnABikeAction.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace security;
	using namespace accounts;
	using namespace time;
	using namespace html;
	using namespace interfaces;

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

		void VinciCustomerAdminInterfaceElement::display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request /*= NULL*/ ) const
		{
			// Update user request
			ActionFunctionRequest<VinciUpdateCustomerAction,AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerAdminInterfaceElement>());
			updateRequest.setObjectId(request->getObjectId());

			// Add guarantee request
			ActionFunctionRequest<VinciAddGuaranteeAction,AdminRequest> addGuaranteeRequest(request);
			addGuaranteeRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerAdminInterfaceElement>());
			addGuaranteeRequest.setObjectId(request->getObjectId());

			// Return guarantee request
			ActionFunctionRequest<VinciReturnGuaranteeAction, AdminRequest> returnGuaranteeRequest(request);
			returnGuaranteeRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerAdminInterfaceElement>());
			returnGuaranteeRequest.setObjectId(request->getObjectId());

			// Add Rent request
			ActionFunctionRequest<RentABikeAction, AdminRequest> addRentRequest(request);
			addRentRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerAdminInterfaceElement>());
			addRentRequest.setObjectId(request->getObjectId());

			// Return Rent request
			ActionFunctionRequest<ReturnABikeAction,AdminRequest> returnRentRequest(request);
			returnRentRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerAdminInterfaceElement>());
			returnRentRequest.setObjectId(request->getObjectId());

			// Print request
			FunctionRequest<VinciContractPrintRequest> printRequest(request);
			printRequest.getFunction()->setContract(_contract);

			// Personal Data
			stream << "<h1>Coordonnées</h1>";

			HTMLForm form(updateRequest.getHTMLForm("update"));
			HTMLTable t;
			
			stream << form.open() << t.open();
			stream << t.row();
			stream << t.col() << "Nom";
			stream << t.col() << form.getTextInput(VinciUpdateCustomerAction::PARAMETER_NAME, _user->getName());
			stream << t.col() << "E-mail";
			stream << t.col() << form.getTextInput(VinciUpdateCustomerAction::PARAMETER_EMAIL, _user->getEMail());
			stream << t.row();
			stream << t.col() << "Prénom";
			stream << t.col() << form.getTextInput(VinciUpdateCustomerAction::PARAMETER_SURNAME, _user->getSurname());
			stream << t.col() << "Téléphone";
			stream << t.col() << form.getTextInput(VinciUpdateCustomerAction::PARAMETER_PHONE, _user->getPhone());
			stream << t.row();
			stream << t.col() << "Adresse";
			stream << t.col() << form.getTextInput(VinciUpdateCustomerAction::PARAMETER_ADDRESS, _user->getAddress());
			stream << t.col() << "Date de naissance";
			stream << t.col() << form.getCalendarInput(VinciUpdateCustomerAction::PARAMETER_BIRTH_DATE, _user->getBirthDate());
			stream << t.row();
			stream << t.col() << "Code postal";
			stream << t.col() << form.getTextInput(VinciUpdateCustomerAction::PARAMETER_POST_CODE, _user->getPostCode());
			stream << t.col() << "Pièce d'identité";
			stream << t.col() << form.getTextInput(VinciUpdateCustomerAction::PARAMETER_PASSPORT, _contract->getPassport());
			stream << t.row();
			stream << t.col() << "Commune";
			stream << t.col() << form.getTextInput(VinciUpdateCustomerAction::PARAMETER_CITY, _user->getCityText());
			stream << t.row();
			stream << t.col() << "Pays";
			stream << t.col() << form.getTextInput(VinciUpdateCustomerAction::PARAMETER_COUNTRY, _user->getCountry());
			stream << t.row();
			stream << t.col(2) << form.getSubmitButton("Enregistrer");
			stream << t.col(2) << printRequest.getHTMLForm().getLinkButton("Imprimer");
			stream << t.close() << form.close();			

			// Guarantees
			stream << "<h1>Cautions</h1>";
			Account* checkAccount = VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE);
			Account* cardAccount = VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE);
			vector<pair<uid, string> > paymentModesMap;
			paymentModesMap.push_back(make_pair(checkAccount->getKey(), "Chèque"));
			paymentModesMap.push_back(make_pair(cardAccount->getKey(), "Carte"));

			vector<TransactionPart*> guarantees = TransactionPartTableSync::search(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE), _user);
			HTMLForm addGuaranteeForm(addGuaranteeRequest.getHTMLForm("addguarantee"));
			addGuaranteeForm.addHiddenField(VinciAddGuaranteeAction::PARAMETER_CONTRACT_ID, Conversion::ToString(_contract->getKey()));
			
			HTMLTable::ColsVector cv;
			cv.push_back("Date");
			cv.push_back("Montant");
			cv.push_back("Nature");
			cv.push_back("Actions");
			HTMLTable ct(cv);
			
			stream
				<< "<div class=\"" << VinciBikeRentalModule::CSS_LIMITED_HEIGHT << "\">"
				<< addGuaranteeForm.open() << ct.open();

			stream << ct.row();
			stream << ct.col() << addGuaranteeForm.getCalendarInput(VinciAddGuaranteeAction::PARAMETER_DATE, DateTime());
			stream << ct.col() << addGuaranteeForm.getTextInput(VinciAddGuaranteeAction::PARAMETER_AMOUNT, "260");
			stream << ct.col() << addGuaranteeForm.getSelectInput(VinciAddGuaranteeAction::PARAMETER_ACCOUNT_ID, paymentModesMap, uid());
			stream << ct.col() << addGuaranteeForm.getSubmitButton("Nouvelle caution");

			for (vector<TransactionPart*>::iterator it = guarantees.begin(); it != guarantees.end(); ++it)
			{
				Transaction* transaction = TransactionTableSync::get((*it)->getTransactionId());
				vector<TransactionPart*> payments = TransactionPartTableSync::search(transaction);

				stream << ct.row();
				stream << ct.col() << transaction->getStartDateTime().toString();
				stream << ct.col() << Conversion::ToString((*it)->getRightCurrencyAmount());
				stream << ct.col();
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
				stream << ct.col();
				if (transaction->getEndDateTime().isUnknown())
				{
					HTMLForm returnForm(returnGuaranteeRequest.getHTMLForm("return" + Conversion::ToString(transaction->getKey())));
					returnForm.addHiddenField(VinciReturnGuaranteeAction::PARAMETER_GUARANTEE_ID, Conversion::ToString(transaction->getKey()));
					stream << returnForm.getLinkButton("Rendre la caution");
				}
				else
					stream << "Caution rendue le " << transaction->getEndDateTime().toString();

				delete transaction;
				delete *it;
			}

			stream << ct.close() << addGuaranteeForm.close() << "</div>";

			// Rents
			stream << "<h1>Locations</h1>";
			vector<TransactionPart*> rents = TransactionPartTableSync::search(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE), _user);
			vector<VinciRate*> rates = VinciRateTableSync::search();
			HTMLForm addRentForm(addRentRequest.getHTMLForm("addrent"));
			addRentForm.addHiddenField(RentABikeAction::PARAMETER_CONTRACT_ID, Conversion::ToString(_contract->getKey()));

			HTMLTable::ColsVector rv;
			rv.push_back("Date");
			rv.push_back("Vélo");
			rv.push_back("Antivol");
			rv.push_back("Tarif");
			rv.push_back("Actions");
			HTMLTable rt(rv);
			stream
				<< "<div class=\"" << VinciBikeRentalModule::CSS_LIMITED_HEIGHT << "\">"
				<< addRentForm.open() << rt.open();

			stream << rt.row();
			stream << rt.col() << addRentForm.getCalendarInput(RentABikeAction::PARAMETER_DATE, DateTime());
			stream << rt.col() << addRentForm.getTextInput(RentABikeAction::PARAMETER_BIKE_ID, "");
			stream << rt.col() << addRentForm.getTextInput(RentABikeAction::PARAMETER_LOCK_ID, "");
			stream << rt.col() << "<select name=\"" << RentABikeAction::PARAMETER_RATE_ID << "\">";
			
			for (vector<VinciRate*>::iterator it = rates.begin(); it != rates.end(); ++it)
				stream << "<option value=\"" << Conversion::ToString((*it)->getKey()) << "\">" << (*it)->getName() << "</option>";
			stream << "</select>";
			stream << rt.col() << addRentForm.getSubmitButton("Nouvelle location");

			for (vector<TransactionPart*>::iterator it = rents.begin(); it != rents.end(); ++it)
			{
				Transaction* transaction = TransactionTableSync::get((*it)->getTransactionId());
								
				VinciRate* rate = NULL;
				if ((*it)->getRateId() > 0)
					rate = VinciRateTableSync::get((*it)->getRateId());

				VinciBike* bike = NULL;
				if ((*it)->getTradedObjectId() != "")
					bike = VinciBikeTableSync::get(Conversion::ToLongLong((*it)->getTradedObjectId()));

				VinciAntivol* lock = NULL;
				vector<TransactionPart*> vtp = TransactionPartTableSync::search(transaction, VinciBikeRentalModule::getFreeLockRentServiceAccount(), 0, 1);
				if (!vtp.empty())
				{
					TransactionPart* tp = vtp.front();
					lock = VinciAntivolTableSync::get(Conversion::ToLongLong(tp->getTradedObjectId()));
					delete tp;
				}

				stream << rt.row();
				stream << rt.col() << transaction->getStartDateTime().toString();
				stream << rt.col() << ((bike == NULL) ? "Non renseign&eacute;" : bike->getNumber());
				stream << rt.col() << (lock ? lock->getMarkedNumber() : "Non renseign&eacute;");
				stream << rt.col() << ((rate == NULL) ? "Non renseign&eacute;" : rate->getName());
				stream << rt.col();
				if (transaction->getEndDateTime().isUnknown())
				{
					DateTime now;
					if (now > rate->getEndDate(transaction->getStartDateTime()))
					{
						stream << "Retard : ";
						const DurationInterfacePage* page = request->getFunction()->getInterface()->getPage<DurationInterfacePage>();
						page->display(stream, now - rate->getEndDate(transaction->getStartDateTime()), variables, NULL, request);
						stream << " - " << rate->getAdditionalAmountToPay(transaction->getStartDateTime()) << " EUR<br />";
					}
					HTMLForm returnForm(returnRentRequest.getHTMLForm("return" + Conversion::ToString((*it)->getKey())));
					returnForm.addHiddenField(ReturnABikeAction::PARAMETER_TRANSACTION_PART_ID, Conversion::ToString((*it)->getKey()));
					stream << returnForm.getLinkButton("Retour du vélo");
				}
				else
				{
					stream
						<< "Retour le " << transaction->getEndDateTime().toString();
				}

				delete bike;
				delete rate;
				delete lock;
			}

			stream << rt.close() << addRentForm.close() << "</div>";


			// Change
			stream << "<h1>Compte client</h1>";
		}

		void VinciCustomerAdminInterfaceElement::setFromParametersMap(const ParametersMap& map)
		{
			// Current contract
			const ParametersMap::const_iterator it = map.find(Request::PARAMETER_OBJECT_ID);
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

		bool VinciCustomerAdminInterfaceElement::isAuthorized( const server::FunctionRequest<AdminRequest>* request ) const
		{
			return true;
		}
	}
}
