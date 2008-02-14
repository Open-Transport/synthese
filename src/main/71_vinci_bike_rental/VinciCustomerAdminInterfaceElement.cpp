
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
#include "30_server/QueryString.h"

#include "32_admin/AdminRequest.h"

#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"
#include "57_accounting/AccountingModule.h"
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
#include "71_vinci_bike_rental/RemoveCustomerAction.h"
#include "71_vinci_bike_rental/VinciCustomerSearchAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciAddFinancialAction.h"

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
	using namespace vinci;
	using namespace util;

	namespace util
	{
	    template <> const string FactorableTemplate<AdminInterfaceElement,VinciCustomerAdminInterfaceElement>::FACTORY_KEY("vincicustomer");
	}

	namespace admin
	{
		template <> const string AdminInterfaceElementTemplate<VinciCustomerAdminInterfaceElement>::ICON("vcard.png");
		template <> const string AdminInterfaceElementTemplate<VinciCustomerAdminInterfaceElement>::DEFAULT_TITLE("Client inconnu");
	}

	namespace vinci
	{
		VinciCustomerAdminInterfaceElement::VinciCustomerAdminInterfaceElement()
			: AdminInterfaceElementTemplate<VinciCustomerAdminInterfaceElement>() 
		{}



		void VinciCustomerAdminInterfaceElement::display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request /*= NULL*/ ) const
		{
			// Update user request
			ActionFunctionRequest<VinciUpdateCustomerAction,AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage<VinciCustomerAdminInterfaceElement>();
			updateRequest.setObjectId(request->getObjectId());

			// Add guarantee request
			ActionFunctionRequest<VinciAddGuaranteeAction,AdminRequest> addGuaranteeRequest(request);
			addGuaranteeRequest.getFunction()->setPage<VinciCustomerAdminInterfaceElement>();
			addGuaranteeRequest.setObjectId(request->getObjectId());

			// Return guarantee request
			ActionFunctionRequest<VinciReturnGuaranteeAction, AdminRequest> returnGuaranteeRequest(request);
			returnGuaranteeRequest.getFunction()->setPage<VinciCustomerAdminInterfaceElement>();
			returnGuaranteeRequest.setObjectId(request->getObjectId());

			// Add Rent request
			ActionFunctionRequest<RentABikeAction, AdminRequest> addRentRequest(request);
			addRentRequest.getFunction()->setPage<VinciCustomerAdminInterfaceElement>();
			addRentRequest.setObjectId(request->getObjectId());

			// Add Financial request
			ActionFunctionRequest<VinciAddFinancialAction, AdminRequest> addFinancialRequest(request);
			addFinancialRequest.getAction()->setUser(_contract->getUser());
			addFinancialRequest.getFunction()->setPage<VinciCustomerAdminInterfaceElement>();
			addFinancialRequest.setObjectId(request->getObjectId());

			// Return Rent request
			ActionFunctionRequest<ReturnABikeAction,AdminRequest> returnRentRequest(request);
			returnRentRequest.getFunction()->setPage<VinciCustomerAdminInterfaceElement>();
			returnRentRequest.setObjectId(request->getObjectId());

			// Print request
			FunctionRequest<VinciContractPrintRequest> printRequest(request);
			printRequest.getFunction()->setContract(_contract);

			// Remove request
			ActionFunctionRequest<RemoveCustomerAction,AdminRequest> removeRequest(request);
			removeRequest.getFunction()->setPage<VinciCustomerSearchAdminInterfaceElement>();
			removeRequest.getAction()->setContract(_contract);


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
			shared_ptr<Account> checkAccount = VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE);
			shared_ptr<Account> cardAccount = VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE);
			vector<pair<uid, string> > paymentModesMap;
			DateTime oneMonthBefore(TIME_CURRENT);
			oneMonthBefore -= 31;
			paymentModesMap.push_back(make_pair(checkAccount->getKey(), "Chèque"));
			paymentModesMap.push_back(make_pair(cardAccount->getKey(), "Carte"));

			vector<shared_ptr<TransactionPart> > guarantees = TransactionPartTableSync::search((shared_ptr<const Account>) VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE), _user);
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
			stream << ct.col() << addGuaranteeForm.getCalendarInput(VinciAddGuaranteeAction::PARAMETER_DATE, DateTime(TIME_CURRENT));
			stream << ct.col() << addGuaranteeForm.getTextInput(VinciAddGuaranteeAction::PARAMETER_AMOUNT, "260");
			stream << ct.col() << addGuaranteeForm.getSelectInput(VinciAddGuaranteeAction::PARAMETER_ACCOUNT_ID, paymentModesMap, uid());
			stream << ct.col() << addGuaranteeForm.getSubmitButton("Nouvelle caution");

			for (vector<shared_ptr<TransactionPart> >::iterator it = guarantees.begin(); it != guarantees.end(); ++it)
			{
				shared_ptr<const Transaction> transaction = TransactionTableSync::Get((*it)->getTransactionId());
				vector<shared_ptr<TransactionPart> > payments = TransactionPartTableSync::search(transaction);

				stream << ct.row();
				stream << ct.col() << transaction->getStartDateTime().toString();
				stream << ct.col() << Conversion::ToString((*it)->getRightCurrencyAmount());
				stream << ct.col();
				for (vector<shared_ptr<TransactionPart> >::iterator it2 = payments.begin(); it2 != payments.end(); ++it2)
				{
					if ((*it2)->getKey() != (*it)->getKey())
					{
						shared_ptr<const Account> account = AccountTableSync::Get((*it2)->getAccountId());
						if (account->getRightClassNumber() == VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE)
							stream << "Chèque";
						else
							stream << "Carte";
					}
				}
				stream << ct.col();
				if (transaction->getEndDateTime().isUnknown())
				{
					HTMLForm returnForm(returnGuaranteeRequest.getHTMLForm("return" + Conversion::ToString(transaction->getKey())));
					returnForm.addHiddenField(VinciReturnGuaranteeAction::PARAMETER_GUARANTEE_ID, Conversion::ToString(transaction->getKey()));
					stream << returnForm.getLinkButton("Rendre la caution");
					if (transaction->getStartDateTime() <= oneMonthBefore)
						stream << HTMLModule::GetHTMLJavascriptOpen() << "alert(\"Attention caution périmée\");" << HTMLModule::GetHTMLJavascriptClose();
				}
				else
					stream << "Caution rendue le " << transaction->getEndDateTime().toString();
			}

			stream << ct.close() << addGuaranteeForm.close() << "</div>";

			// Rents
			stream << "<h1>Locations</h1>";
			vector<shared_ptr<TransactionPart> > rents = TransactionPartTableSync::search((shared_ptr<const Account>) VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE), _user);
			vector<shared_ptr<VinciRate> > rates = VinciRateTableSync::search();
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
			stream << rt.col() << addRentForm.getCalendarInput(RentABikeAction::PARAMETER_DATE, DateTime(TIME_CURRENT));
			stream << rt.col() << addRentForm.getTextInput(RentABikeAction::PARAMETER_BIKE_ID, "");
			stream << rt.col() << addRentForm.getTextInput(RentABikeAction::PARAMETER_LOCK_ID, "");
			stream << rt.col() << "<select name=\"" << RentABikeAction::PARAMETER_RATE_ID << "\">";
			
			for (vector<shared_ptr<VinciRate> >::iterator it = rates.begin(); it != rates.end(); ++it)
				stream << "<option value=\"" << Conversion::ToString((*it)->getKey()) << "\">" << (*it)->getName() << "</option>";
			stream << "</select>";
			stream << rt.col() << addRentForm.getSubmitButton("Nouvelle location");

			for (vector<shared_ptr<TransactionPart> >::iterator it = rents.begin(); it != rents.end(); ++it)
			{
				shared_ptr<const Transaction> transaction = TransactionTableSync::Get((*it)->getTransactionId());
								
				shared_ptr<VinciRate> rate;
				if ((*it)->getRateId() > 0)
					rate = VinciRateTableSync::GetUpdateable((*it)->getRateId());

				shared_ptr<VinciBike> bike;
				if ((*it)->getTradedObjectId() != "")
					bike = VinciBikeTableSync::GetUpdateable(Conversion::ToLongLong((*it)->getTradedObjectId()));

				shared_ptr<VinciAntivol> lock;
				vector<shared_ptr<TransactionPart> > vtp = TransactionPartTableSync::search(transaction, VinciBikeRentalModule::getFreeLockRentServiceAccount(), 0, 1);
				if (!vtp.empty())
				{
					shared_ptr<TransactionPart> tp = vtp.front();
					lock = VinciAntivolTableSync::GetUpdateable(Conversion::ToLongLong(tp->getTradedObjectId()));
				}

				stream << rt.row();
				stream << rt.col() << transaction->getStartDateTime().toString();
				stream << rt.col() << (bike.get() ? bike->getNumber() : "Non renseign&eacute;");
				stream << rt.col() << (lock.get() ? lock->getMarkedNumber() : "Non renseign&eacute;");
				stream << rt.col() << (rate.get() ? rate->getName() : "Non renseign&eacute;");
				stream << rt.col();
				if (transaction->getEndDateTime().isUnknown())
				{
					DateTime now(TIME_CURRENT);
					if (now > rate->getEndDate(transaction->getStartDateTime()))
					{
						stream << "Retard : ";
						const DurationInterfacePage* page = request->getFunction()->getInterface()->getPage<DurationInterfacePage>();
						page->display(
							stream
							, now - rate->getEndDate(transaction->getStartDateTime())
							, variables
							, NULL, request);
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
			}

			stream << rt.close() << addRentForm.close() << "</div>";


			// Change
			stream << "<h1>Compte client</h1>";
			shared_ptr<const Account> fa((shared_ptr<const Account>) VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE));
			
			HTMLForm addFinancialForm(addFinancialRequest.getHTMLForm("addfinancial"));
			std::vector<std::pair<uid, std::string> > financialAccounts(AccountingModule::getAccountsName(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHANGE_CODE, true));
			stream << "<p>" << addFinancialForm.open();
			stream << "Réglement : " << addFinancialForm.getSelectInput(VinciAddFinancialAction::PARAMETER_ACCOUNT, financialAccounts, static_cast<uid>(UNKNOWN_VALUE));
			stream << addFinancialForm.getTextInput(VinciAddFinancialAction::PARAMETER_AMOUNT, "");
			stream << addFinancialForm.getSubmitButton("Ajouter");
			stream << addFinancialForm.close() << "</p>";

			HTMLForm addSaleForm(addFinancialRequest.getHTMLForm("addfinancial"));
			financialAccounts = AccountingModule::getAccountsName(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_SERVICES_SALES_CODE + "%", true);
			stream << "<p>" << addSaleForm.open();
			stream << "Vente de produit : " << addSaleForm.getSelectInput(VinciAddFinancialAction::PARAMETER_ACCOUNT, financialAccounts, static_cast<uid>(UNKNOWN_VALUE));
			stream << addSaleForm.getSelectNumberInput(VinciAddFinancialAction::PARAMETER_PIECES, 1, 99, 1);
			stream << addSaleForm.getSubmitButton("Ajouter");
			stream << addSaleForm.close() << "</p>";

			vector<shared_ptr<TransactionPart> > ftps = TransactionPartTableSync::search(fa, _user);
			double solde(TransactionPartTableSync::sum(fa, _user));
			HTMLTable::ColsVector fv;
			fv.push_back("Date");
			fv.push_back("Libellé");
			fv.push_back("Montant");
			HTMLTable ft(fv);
			stream << "<div class=\"" << VinciBikeRentalModule::CSS_LIMITED_HEIGHT << "\">";
			stream << ft.open();
			stream << ft.row();
			stream << ft.col();
			stream << ft.col() << "SOLDE";
			stream << ft.col() << solde;

			for (vector<shared_ptr<TransactionPart> >::iterator it = ftps.begin(); it != ftps.end(); ++it)
			{
				shared_ptr<const Transaction> transaction = TransactionTableSync::Get((*it)->getTransactionId());
				stream << ft.row();
				stream << ft.col() << transaction->getStartDateTime().toString();
				stream << ft.col() << transaction->getName();
				stream << ft.col() << (*it)->getLeftCurrencyAmount();
			}

			stream << ft.close();

			// Admin
			stream << "<h1>Opérations avancées</h1>";

			if (removeRequest.isActionFunctionAuthorized())
				stream << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le contrat ?", "vcard_delete.png") << " ";
		}

		void VinciCustomerAdminInterfaceElement::setFromParametersMap(const ParametersMap& map)
		{
			// Current contract
			uid id = map.getUid(QueryString::PARAMETER_OBJECT_ID, false, FACTORY_KEY);
			if (id != UNKNOWN_VALUE)
			{
				if (id == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
					return;
				_contract = VinciContractTableSync::Get(id);
				_user = UserTableSync::Get(_contract->getUserId());
				_pageLink.name = "Client " + _user->getSurname() + " " + _user->getName();
				_pageLink.parameterName = QueryString::PARAMETER_OBJECT_ID;
				_pageLink.parameterValue = Conversion::ToString(id);
			}
		}

		bool VinciCustomerAdminInterfaceElement::isAuthorized( const server::FunctionRequest<AdminRequest>* request ) const
		{
			return true;
		}

		AdminInterfaceElement::PageLinks VinciCustomerAdminInterfaceElement::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage ) const
		{
			AdminInterfaceElement::PageLinks links;
			if (parentLink.factoryKey == VinciCustomerSearchAdminInterfaceElement::FACTORY_KEY && currentPage.getFactoryKey() == FACTORY_KEY)
			{
				links.push_back(currentPage.getPageLink());
			}
			return links;
		}
	}
}
