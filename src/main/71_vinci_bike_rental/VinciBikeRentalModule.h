
#ifndef SYNTHESE_VinciBikeRentalModule_H__
#define SYNTHESE_VinciBikeRentalModule_H__

#include "01_util/ModuleClass.h"

namespace synthese
{
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

	namespace vinci
	{

		/** @defgroup m71 71 Vinci bike rent.
			
			Most of the events are treated as transaction parts :
				- customer giving a guarantee :
					- positive amount on the template guarantee account, tagged with the corresponding user
					- negative amount on the financial account used for payment (warning : the financial accounts for guarantee should be separated for the ones corresponding to the standard payments in several organizations)
				- destroying of a customer guarantee :
					- negative amount on the template guarantee account, tagged with the corresponding user
					- positive amount on the financial account used for the payment
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
			
			Remarks : the "ticket uses" and the payment accounts will never go down without 
				
			Bike rent is a special application of the accounting module. The French account numbers are used. The special accounts are :
				- Customers :
					- 4111 : Customer financial account (Currency = Euro or local currency)
					- 4117 : Guarantees (Currency = Euro or local currency)
					- 4119 : Customer tickets accounts (Currency = ticket punchings)
				- Stocks :
					- 371 : Bikes (Currency = bikes)
				- Services :
					*- 70831 : Bike rent (Currency = Euro or local currency)
					- 70832 : Bike rent (Currency = tickets punchings)
					*- 763 : Delayed payments (Currency = Euro or local currency)
					*- 707 : Unreturned bikes (sales) (Currency = Euro or local currency)
				- Change :
					- 5331 : guarantees checks (Currency = Euro or local currency)
					- 5332 : guarantees payment card (Currency = Euro or local currency)
					*- 5112 : checks (Currency = Euro or local currency)
					*- 5121 : payment card (Currency = Euro or local currency)
					*- 532 : cash (Currency = Euro or local currency)
					- 59 : tickets punching (Currency = ticket punchings)

			The module initialization consists in the creation of :
				- 3 currencies : Euro, Tickets, and Bikes
				- a special user "Bike Rent Accounts"
				- all the preceding accounts, with their right user equal to the created special user
				- a "Rental User Without Access" Profile

		@{
		*/

		class VinciBikeRentalModule : public util::ModuleClass
		{
		public:
			static const std::string VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE;
			static const std::string VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE;
			static const std::string VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE;
			static const std::string VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE;
			static const std::string VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE;
			static const std::string VINCI_STOCKS_BIKE_ACCOUNT_CODE;
			static const std::string VINCI_SERVICES_BIKE_RENT_EUROS_ACCOUNT_CODE;
			static const std::string VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE;
			static const std::string VINCI_SERVICES_DELAYED_PAYMENTS_ACCOUNT_CODE;
			static const std::string VINCI_SERVICES_UNRETURNED_BIKE_ACCOUNT_CODE;
			static const std::string VINCI_CHANGE_GUARANTEE_ACCOUNT_CODE;
			static const std::string VINCI_CHANGE_CHECKS_ACCOUNT_CODE;
			static const std::string VINCI_CHANGE_CREDIT_CARD_ACCOUNT_CODE;
			static const std::string VINCI_CHANGE_CASH_ACCOUNT_CODE;
			static const std::string VINCI_CHANGE_TICKETS_PUNCHING_ACCOUNT_CODE;

			static const std::string VINCI_ACCOUNTING_PROFILE;
			static const std::string VINCI_ACCOUNTING_PROFILE_RIGHTS;
			static const std::string VINCI_ACCOUNTING_USER;

			static const std::string VINCI_CURRENCY_EURO_NAME;
			static const std::string VINCI_CURRENCY_EURO;
			static const std::string VINCI_CURRENCY_BIKE_NAME;
			static const std::string VINCI_CURRENCY_BIKE;
			static const std::string VINCI_CURRENCY_TICKET_PUNCHING_NAME;
			static const std::string VINCI_CURRENCY_TICKET_PUNCHING;

			static const std::string VINCI_CUSTOMER_PROFILE;
			static const std::string VINCI_CUSTOMER_PROFILE_RIGHTS;

			void initialize();


			/** Customer profile getter.
				@warning the returned profile must be deallocated after use to avoid memory leaks
			*/
			static security::Profile* getCustomerProfile();
			static security::User* getVinciUser();
			static accounts::Currency* getEuroCurrency();
			static accounts::Account* getGuaranteeAccount();
			static accounts::Account* getCheckGuaranteeAccount();
			static accounts::Account* getCardGuaranteeAccount();
		};
		/** @} */
	}
}

#endif // SYNTHESE_VinciBikeRentalModule_H__

