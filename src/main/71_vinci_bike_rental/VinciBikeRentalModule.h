
/** VinciBikeRentalModule class header.
	@file VinciBikeRentalModule.h

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

#ifndef SYNTHESE_VinciBikeRentalModule_H__
#define SYNTHESE_VinciBikeRentalModule_H__

#include <boost/shared_ptr.hpp>

#include <vector>
#include <map>
#include <utility>

#include "01_util/ModuleClass.h"
#include "01_util/FactorableTemplate.h"
#include "01_util/UId.h"

namespace synthese
{
	namespace server
	{
		class Session;
	}

	namespace security
	{
		class Profile;
		class User;
	}

	namespace accounts
	{
		class Account;
		class Currency;
	}

		/**	@defgroup m71Actions 71 Actions
			@ingroup m71

			@defgroup m71Pages 71 Pages
			@ingroup m71

			@defgroup m71Values 71 Values
			@ingroup m71

			@defgroup m71Functions 71 Functions
			@ingroup m71

			@defgroup m71LS 71 Table synchronizers
			@ingroup m71

			@defgroup m71Admin 71 Administration pages
			@ingroup m71

			@defgroup m71Rights 71 Rights
			@ingroup m71

			@defgroup m71Logs 71 DB Logs
			@ingroup m71

			@defgroup m71 71 Vinci bike rental
			@ingroup m7
						
			Most of the events are treated as transaction parts between accounts.

			@section vinciAccounts Accounts

			Remarks : the "ticket uses" and the payment accounts will never go down without 
			
			Bike rent is a special application of the accounting module. The French account numbers are used. The special accounts are :
				- Customers :
					- 4111 : Customer financial account (Currency = Euro or local currency)
					- 4117 : Guarantees (Currency = Euro or local currency)
					- 4119 : Customer tickets accounts (Currency = ticket punchings)
				- Stocks :
					- 371 : Bikes (Currency = bikes)
				- Services :
					- *70831 : Bike rent (Currency = Euro or local currency)
					- 70832 : Bike rent (Currency = tickets punchings)
					- *763 : Delayed payments (Currency = Euro or local currency)
					- *707 : Unreturned bikes (sales) (Currency = Euro or local currency)
				- Change :
					- 5331 : guarantees checks (Currency = Euro or local currency)
					- 5332 : guarantees payment card (Currency = Euro or local currency)
					- *5112 : checks (Currency = Euro or local currency)
					- *5121 : payment card (Currency = Euro or local currency)
					- *532 : cash (Currency = Euro or local currency)
					- 59 : tickets punching (Currency = ticket punchings)

			@section vinciGuarantees Guarantees
				- customer giving a guarantee :
					- positive amount on the template guarantee account, tagged with the corresponding user
					- negative amount on the financial account used for payment (warning : the financial accounts for guarantee should be separated for the ones corresponding to the standard payments in several organizations)
				- destroying of a customer guarantee :
					- negative amount on the template guarantee account, tagged with the corresponding user
					- positive amount on the financial account used for the payment

			@section vinciRent Bike rent

			@msc
				"371|Start pl.","371|Return pl." , "371|Outgoing", 4111, "4 VAT", "51*", 59, 70831, 70832, 763, "7 discount";
				--- [label="Start of the rent"];
				"371|Start pl." -> "371|Outgoing" [label = "1 bike"];
				4111 -> "4 VAT" [label = "VAT initial amount"];
				4111 -> 70831 [label = "rent initial amount"];
				59 -> 70832 [label = "1 ticket"];
				--- [label="Payment"];
				"51*" -> 4111 [label = "due amount"];
				...;
				--- [label="Return of the bike"];
				"371|Outgoing" -> "371|Return pl." [label = "1 bike"];
				4111 -> "4 VAT" [label = "VAT return amount"];
				4111 -> 70831 [label = "rent return amount"];
				59 -> 70832 [label = "1 ticket"];
				--- [label="If return on late"];
				4111 -> "4 VAT" [label = "VAT penalty amount"];
				4111 -> 763 [label = "penalty amount"];
				--- [label="If discount"];
				"4 VAT" -> 4111 [label = "VAT discount amount"];
				"7 discount" -> 4111 [label = "discount amount"];
				--- [label="Payment"];
				"51*" -> 4111 [label = "due amount"];
			@endmsc

			- starting a bike rental :
					- +1 on the bike stock of the used station, tagged with the number of the corresponding bike in the tradedObjectId field
					- -1 on the template outgoing bikes account, tagged :
						- with the corresponding user
						- with the number of the corresponding bike in the tradedObjectId field 
					- if the fare needs payment :
						- negative starting price on the template customer account, tagged with the corresponding user
						- positive starting price (deducted with the taxes) on the bike rental product account, tagged with the corresponding fare
						- positive starting tax on the tax account
					- if the fare needs ticket uses :
						- -1 on the template customer tickets account
						- +1 on the bike rental product payed with transport tickets account, tagged with the corresponding fare
				- ending a bike rental :
					- -1 on the bike stock of the used station, tagged with the number of the corresponding bike in the tradedObjectId field
					- +1 on the template outgoing bikes account, tagged :
						- with the corresponding user
						- with the number of the corresponding bike in the tradedObjectId field 
					- if the fare needs payment :
						- negative starting price on the template customer account, tagged with the corresponding user
						- positive starting price (deducted with the taxes) on the bike rental product account, tagged with the corresponding fare
						- positive starting tax on the tax account
					- if the fare needs ticket uses :
						- -1 on the template customer tickets account
						- +1 on the bike rental product payed with transport tickets account, tagged with the corresponding fare
				- payment at the starting of the bike rental :
					- positive amount on the template customer account, tagged with the corresponding user
					- negative amount on the account used for the payment
				- payment with transport tickets uses :
					- positive amount on the template customer tickets account, tagged with the corresponding user
					- negative amount on the "ticket uses" account
			
		@{
		*/

	/** 71 Vinci Bike Rental Module namespace. */
	namespace vinci
	{

		/** 71 Vinci Bike Rental Module class. */
		class VinciBikeRentalModule : public util::FactorableTemplate<util::ModuleClass, VinciBikeRentalModule>
		{
		private:
			static boost::shared_ptr<accounts::Account>		_freeLockRent;
			static boost::shared_ptr<security::User>		_vinciUser;
			static boost::shared_ptr<security::Profile>		_adminProfile;
			static boost::shared_ptr<security::Profile>		_operatorProfile;
			static boost::shared_ptr<security::Profile>		_vinciProfile;
			static boost::shared_ptr<security::Profile>		_vinciCustomerProfile;
			static boost::shared_ptr<accounts::Currency>	_euroCurrency;
			static boost::shared_ptr<accounts::Currency>	_objectCurrency;
			static boost::shared_ptr<accounts::Currency>	_ticketCurrency;
			static boost::shared_ptr<accounts::Account>		_stockChargeAccount;


			static const std::string VINCI_SERVICES_LOCK_RENT_FREE_ACCOUNT_CODE;

			typedef std::map<const server::Session*, uid> _SessionsSitesMap;
			static _SessionsSitesMap _sessionsSites;


		public:
			//! \name CSS Styles
			//@{
				static const std::string CSS_LIMITED_HEIGHT;
			//@}

			static const std::string VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE;
			static const std::string VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE;
			static const std::string VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE;
			static const std::string VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE;
			static const std::string VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE;
			static const std::string VINCI_AVAILABLE_BIKES_STOCKS_ACCOUNT_CODE;
			
			static const std::string VINCI_SERVICES_CODE;
			static const std::string VINCI_SERVICES_BIKE_RENT_EUROS_ACCOUNT_CODE;
			static const std::string VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE;
			static const std::string VINCI_SERVICES_DELAYED_PAYMENTS_ACCOUNT_CODE;
			static const std::string VINCI_SERVICES_UNRETURNED_BIKE_ACCOUNT_CODE;
			static const std::string VINCI_SERVICES_SALES_CODE;

			static const std::string VINCI_CHANGE_CODE;
			static const std::string VINCI_CHANGE_GUARANTEE_ACCOUNT_CODE;
			static const std::string VINCI_CHANGE_CHECKS_ACCOUNT_CODE;
			static const std::string VINCI_CHANGE_CREDIT_CARD_ACCOUNT_CODE;
			static const std::string VINCI_CHANGE_CASH_ACCOUNT_CODE;
			static const std::string VINCI_CHANGE_TICKETS_PUNCHING_ACCOUNT_CODE;
			static const std::string VINCI_CHANGE_CUSTOM_CODE;

			static const std::string VINCI_STOCK_CODE;

			static const std::string VINCI_CHARGE_STOCK_CHANGE_CODE;

			static const std::string VINCI_ACCOUNTING_PROFILE;
			static const std::string VINCI_ACCOUNTING_PROFILE_RIGHTS;
			static const std::string VINCI_ACCOUNTING_USER;

			static const std::string VINCI_ADMIN_PROFILE;
			static const std::string VINCI_OPERATOR_PROFILE;

			static const std::string VINCI_CURRENCY_EURO_NAME;
			static const std::string VINCI_CURRENCY_EURO;
			static const std::string VINCI_CURRENCY_OBJECT_NAME;
			static const std::string VINCI_CURRENCY_OBJECT;
			static const std::string VINCI_CURRENCY_TICKET_PUNCHING_NAME;
			static const std::string VINCI_CURRENCY_TICKET_PUNCHING;

			static const std::string VINCI_CUSTOMER_PROFILE;
			static const std::string VINCI_CUSTOMER_PROFILE_RIGHTS;

			/** Initialization of the module after the automatic database loads.
				@author Hugues Romain
				@date 2007
				
				The module initialization consists in the creation of :
				- 3 currencies : Euro, Tickets, and Bikes
					- a special user "Bike Rent Accounts"
					- all the preceding accounts, with their right user equal to the created special user
					- a "Rental User Without Access" Profile
			*/			
			void initialize();

			//! \name Account getters
			//@{
				/** Customer profile getter.
					@warning the returned profile must be deallocated after use to avoid memory leaks
				*/
				static boost::shared_ptr<const security::Profile>	getCustomerProfile();
				static boost::shared_ptr<const security::User>		getVinciUser();
				static boost::shared_ptr<const accounts::Account>	getFreeLockRentServiceAccount();
				static boost::shared_ptr<const accounts::Account>	getStockChargeAccount();
				static boost::shared_ptr<const accounts::Currency>	getEuroCurrency();
				static boost::shared_ptr<const accounts::Currency>	getObjectCurrency();
				
				static boost::shared_ptr<accounts::Account> getAccount(const std::string& code);

			//@}

			static void AddSessionSite(const server::Session* session, uid siteId);
			static uid GetSessionSite(const server::Session* session);

			static std::vector<std::pair<uid, std::string> > GetSitesName(uid differentValue);
		};
	}
	/** @} */
}

#endif // SYNTHESE_VinciBikeRentalModule_H__

