
/** OnlineReservationRule class implementation.
	@file OnlineReservationRule.cpp

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

#include "OnlineReservationRule.h"

#include "EMail.h"
#include "LoginToken.hpp"
#include "PTModule.h"
#include "Profile.h"
#include "Registry.h"
#include "ResaModule.h"
#include "ResaRight.h"
#include "Reservation.h"
#include "ReservationContact.h"
#include "ReservationTransaction.h"
#include "ServerModule.h"
#include "User.h"
#include "UserTableSync.h"
#include "Webpage.h"

#include "UtilConstants.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace cms;
	using namespace pt;
	using namespace resa;
	using namespace security;
	using namespace server;
	using namespace util;

	CLASS_DEFINITION(OnlineReservationRule, "t047_online_reservation_rules", 47)
	FIELD_DEFINITION_OF_OBJECT(OnlineReservationRule, "online_reservation_rule_id", "online_reservation_rule_ids")

	FIELD_DEFINITION_OF_TYPE(resa::Contact, "reservation_rule_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(resa::Email, "email", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(CopyEmail, "copy_email", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(NeedsSurname, "needs_surname", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(NeedsAddress, "needs_address", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(NeedsPhone, "needs_phone", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(NeedsEmail, "needs_email", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(NeedsCustomerNumber, "needs_customer_number", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MaxSeat, "max_seat", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Thresholds, "thresholds", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(SenderEmail, "sender_email", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(SenderName, "sender_name", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ConfirmationEmailCMS, "confirmation_email_cms_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(CancellationEmailCMS, "cancellation_email_cms_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(PasswordEmailCMS, "password_email_cms_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MultiReservationsEmailCMS, "multi_reservations_email_cms_id", SQL_INTEGER)

	namespace resa
	{
		OnlineReservationRule::OnlineReservationRuleMap OnlineReservationRule::_onlineReservationRuleMap;

		const std::string OnlineReservationRule::DATA_SUBJECT_OR_CONTENT("subject_or_content");
		const std::string OnlineReservationRule::DATA_DEPARTURE_DATE("departure_date");
		const std::string OnlineReservationRule::DATA_DEPARTURE_CITY_NAME("departure_city_name");
		const std::string OnlineReservationRule::DATA_DEPARTURE_PLACE_NAME("departure_place_name");
		const std::string OnlineReservationRule::DATA_ARRIVAL_CITY_NAME("arrival_city_name");
		const std::string OnlineReservationRule::DATA_ARRIVAL_PLACE_NAME("arrival_place_name");
		const std::string OnlineReservationRule::DATA_DEPARTURE_TIME("departure_time");
		const std::string OnlineReservationRule::DATA_IS_RESERVATION_POSSIBLE("is_reservation_possible");
		const std::string OnlineReservationRule::DATA_ARRIVAL_TIME("arrival_time");
		const std::string OnlineReservationRule::DATA_LINE_CODE("line_code");
		const std::string OnlineReservationRule::DATA_SEATS_NUMBER("seats_number");
		const std::string OnlineReservationRule::DATA_ROAD_RESAS("road_resas");
		const std::string OnlineReservationRule::DATA_ROAD_RESA("road_resa");
		const std::string OnlineReservationRule::DATA_KEY_RESA("key_resa");
		const std::string OnlineReservationRule::DATA_CUSTOMER_ID("customer_id");
		const std::string OnlineReservationRule::DATA_RESERVATION_DEAD_LINE_DATE("reservation_dead_line_date");
		const std::string OnlineReservationRule::DATA_RESERVATION_DEAD_LINE_TIME("reservation_dead_line_time");
		const std::string OnlineReservationRule::DATA_CUSTOMER_NAME("customer_name");
		const std::string OnlineReservationRule::DATA_CUSTOMER_PHONE("customer_phone");
		const std::string OnlineReservationRule::DATA_USER_LOGIN("user_login");
		const std::string OnlineReservationRule::DATA_USER_SURNAME("user_surname");
		const std::string OnlineReservationRule::DATA_USER_NAME("user_name");
		const std::string OnlineReservationRule::DATA_USER_KEY("user_key");
		const std::string OnlineReservationRule::DATA_USER_PHONE("user_phone");
		const std::string OnlineReservationRule::DATA_USER_EMAIL("user_email");
		const std::string OnlineReservationRule::DATA_USER_PASSWORD("user_password");
		const std::string OnlineReservationRule::DATA_CANCELLATION_BECAUSE_OF_ABSENCE("cancellation_because_of_absence");
		const std::string OnlineReservationRule::DATA_RESERVATIONS_NUMBER("reservations_number");
		const std::string OnlineReservationRule::DATA_SERVICES_NUMBER("services_number");
		const std::string OnlineReservationRule::DATA_RESERVATIONS("reservations");
		const std::string OnlineReservationRule::DATA_TRANSACTION_RESERVATION("reservation_transaction");
		const std::string OnlineReservationRule::DATA_TOKEN_CANCELLATION("token_cancel");

		const std::string OnlineReservationRule::TYPE_SUBJECT("subject");
		const std::string OnlineReservationRule::TYPE_CONTENT("content");
		const std::string OnlineReservationRule::TYPE_UNCHANGED_PASSWORD("Mot de passe inchangé");

		OnlineReservationRule::OnlineReservationRule(
			RegistryKeyType key
		):	Registrable(key),
			Object<OnlineReservationRule, OnlineReservationRuleSchema> (
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(resa::Contact),
					FIELD_DEFAULT_CONSTRUCTOR(resa::Email),
					FIELD_DEFAULT_CONSTRUCTOR(CopyEmail),
					FIELD_DEFAULT_CONSTRUCTOR(NeedsSurname),
					FIELD_DEFAULT_CONSTRUCTOR(NeedsAddress),
					FIELD_DEFAULT_CONSTRUCTOR(NeedsPhone),
					FIELD_DEFAULT_CONSTRUCTOR(NeedsEmail),
					FIELD_DEFAULT_CONSTRUCTOR(NeedsCustomerNumber),
					FIELD_DEFAULT_CONSTRUCTOR(MaxSeat),
					FIELD_DEFAULT_CONSTRUCTOR(Thresholds),
					FIELD_DEFAULT_CONSTRUCTOR(SenderEmail),
					FIELD_DEFAULT_CONSTRUCTOR(SenderName),
					FIELD_DEFAULT_CONSTRUCTOR(ConfirmationEmailCMS),
					FIELD_DEFAULT_CONSTRUCTOR(CancellationEmailCMS),
					FIELD_DEFAULT_CONSTRUCTOR(PasswordEmailCMS),
					FIELD_DEFAULT_CONSTRUCTOR(MultiReservationsEmailCMS)
			)	)
		{
		}

		const std::string& OnlineReservationRule::getEMail() const
		{
			return get<resa::Email>();
		}

		const std::string& OnlineReservationRule::getCopyEMail() const
		{
			return get<CopyEmail>();
		}

		boost::logic::tribool OnlineReservationRule::getNeedsSurname() const
		{
			return get<NeedsSurname>();
		}

		boost::logic::tribool OnlineReservationRule::getNeedsAddress() const
		{
			return get<NeedsAddress>();
		}

		boost::logic::tribool OnlineReservationRule::getNeedsPhone() const
		{
			return get<NeedsPhone>();
		}

		boost::logic::tribool OnlineReservationRule::getNeedsCustomerNumber() const
		{
			return get<NeedsCustomerNumber>();
		}

		boost::logic::tribool OnlineReservationRule::getNeedsEMail() const
		{
			return get<NeedsEmail>();
		}



		const OnlineReservationRule::CapacityThresholds& OnlineReservationRule::getThresholds() const
		{
			return _thresholds;
		}

		void OnlineReservationRule::setReservationContact(const ReservationContact* rule)
		{
			if (get<resa::Contact>())
			{
				OnlineReservationRuleMap::iterator it(_onlineReservationRuleMap.find(get<resa::Contact>()->getKey()));
				assert(it != _onlineReservationRuleMap.end());
				_onlineReservationRuleMap.erase(it);
			}
			set<resa::Contact>(rule
				? boost::optional<ReservationContact&>(*const_cast<ReservationContact*>(rule))
				: boost::none);
			_onlineReservationRuleMap.insert(make_pair(rule->getKey(), this));
		}

		void OnlineReservationRule::setEMail( const std::string& email )
		{
			set<resa::Email>(email);
		}

		void OnlineReservationRule::setCopyEMail( const std::string& email )
		{
			set<CopyEmail>(email);
		}

		void OnlineReservationRule::setNeedsSurname( boost::logic::tribool value )
		{
			set<NeedsSurname>(value);
		}

		void OnlineReservationRule::setNeedsAddress( boost::logic::tribool value )
		{
			set<NeedsAddress>(value);
		}

		void OnlineReservationRule::setNeedsPhone( boost::logic::tribool value )
		{
			set<NeedsPhone>(value);
		}

		void OnlineReservationRule::setNeedsCustomerNumber( boost::logic::tribool value )
		{
			set<NeedsCustomerNumber>(value);
		}

		void OnlineReservationRule::setNeedsEMail( boost::logic::tribool value )
		{
			set<NeedsEmail>(value);
		}



		void OnlineReservationRule::setThresholds( const CapacityThresholds& thresholds )
		{
			_thresholds = thresholds;
		}

		const OnlineReservationRule* OnlineReservationRule::GetOnlineReservationRule(
			const ReservationContact* rule
		){
			if(rule == NULL)
			{
				return NULL;
			}
			OnlineReservationRuleMap::const_iterator it(_onlineReservationRuleMap.find(rule->getKey()));
			return (it == _onlineReservationRuleMap.end()) ? NULL : it->second;
		}



		OnlineReservationRule::~OnlineReservationRule()
		{

		}



		bool OnlineReservationRule::sendCustomerEMail(
			const ReservationTransaction& resa
		) const {

			if(	!get<ConfirmationEmailCMS>()
			){
				return false;
			}

			// Use CMS to display EMail
			try
			{
				EMail email(ServerModule::GetEMailSender());

				// MIME type
				string mimeType = get<ConfirmationEmailCMS>()->getMimeType();
				if (mimeType == "text/html") {
					email.setFormat(EMail::EMAIL_HTML);
				}
				else
				{
					email.setFormat(EMail::EMAIL_TEXT);
				}
				email.setSender(get<SenderEmail>());
				email.setSenderName(get<SenderName>());

				stringstream subject;
				ParametersMap subjectMap;

				subjectMap.insert(DATA_SUBJECT_OR_CONTENT, TYPE_SUBJECT);
				subjectMap.insert(DATA_DEPARTURE_DATE, to_simple_string((*resa.getReservations().begin())->get<DepartureTime>().date()));
				subjectMap.insert(DATA_DEPARTURE_CITY_NAME, (*resa.getReservations().begin())->get<DepartureCityName>());
				subjectMap.insert(DATA_DEPARTURE_PLACE_NAME, (*resa.getReservations().begin())->get<DeparturePlaceNameNoCity>());
				subjectMap.insert(DATA_ARRIVAL_CITY_NAME, (*resa.getReservations().rbegin())->get<ArrivalCityName>());
				subjectMap.insert(DATA_ARRIVAL_PLACE_NAME, (*resa.getReservations().rbegin())->get<ArrivalPlaceNameNoCity>());

				get<ConfirmationEmailCMS>()->display(subject, subjectMap);

				email.setSubject(subject.str());

				email.addRecipient(resa.get<CustomerEmail>(), resa.get<CustomerName>());

				stringstream content;
				ParametersMap contentMap;

				contentMap.insert(DATA_SUBJECT_OR_CONTENT, TYPE_CONTENT);
				boost::shared_ptr<ParametersMap> roadResaMap(new ParametersMap);
				BOOST_FOREACH(const Reservation* r, resa.getReservations())
				{
					boost::shared_ptr<ParametersMap> resaMap(new ParametersMap);
					resaMap->insert(DATA_DEPARTURE_TIME, to_simple_string(r->get<DepartureTime>().time_of_day()));
					resaMap->insert(DATA_DEPARTURE_CITY_NAME, r->get<DepartureCityName>());
					resaMap->insert(DATA_DEPARTURE_PLACE_NAME, r->get<DeparturePlaceNameNoCity>());
					resaMap->insert(DATA_ARRIVAL_TIME, to_simple_string(r->get<ArrivalTime>().time_of_day()));
					resaMap->insert(DATA_ARRIVAL_CITY_NAME, r->get<ArrivalCityName>());
					resaMap->insert(DATA_ARRIVAL_PLACE_NAME, r->get<ArrivalPlaceNameNoCity>());
					resaMap->insert(DATA_LINE_CODE, r->get<LineCode>());
					resaMap->insert(DATA_IS_RESERVATION_POSSIBLE, r->get<IsReservationPossible>());
					roadResaMap->insert(DATA_ROAD_RESA, resaMap);
				}

				contentMap.insert(DATA_ROAD_RESAS, roadResaMap);
				contentMap.insert(DATA_KEY_RESA, lexical_cast<string>(resa.getKey()));
				contentMap.insert(DATA_CUSTOMER_ID, lexical_cast<string>(resa.get<Customer>() ? resa.get<Customer>()->getKey() : util::RegistryKeyType(0)));
				contentMap.insert(DATA_RESERVATION_DEAD_LINE_DATE, to_simple_string(resa.getReservationDeadLine().date()));
				contentMap.insert(DATA_RESERVATION_DEAD_LINE_TIME, to_simple_string(resa.getReservationDeadLine().time_of_day()));
				contentMap.insert(DATA_DEPARTURE_CITY_NAME, (*resa.getReservations().begin())->get<DepartureCityName>());
				contentMap.insert(DATA_DEPARTURE_PLACE_NAME, (*resa.getReservations().begin())->get<DeparturePlaceNameNoCity>());
				contentMap.insert(DATA_ARRIVAL_CITY_NAME, (*resa.getReservations().rbegin())->get<ArrivalCityName>());
				contentMap.insert(DATA_ARRIVAL_PLACE_NAME, (*resa.getReservations().rbegin())->get<ArrivalPlaceNameNoCity>());
				contentMap.insert(DATA_DEPARTURE_DATE, to_simple_string((*resa.getReservations().begin())->get<DepartureTime>().date()));
				contentMap.insert(DATA_CUSTOMER_PHONE, resa.get<CustomerPhone>());
				contentMap.insert(DATA_SEATS_NUMBER, lexical_cast<string>(resa.get<Seats>()));

				if (resa.get<Customer>())
				{
					contentMap.insert(DATA_USER_NAME, resa.get<Customer>()->getName());
					contentMap.insert(DATA_USER_SURNAME, resa.get<Customer>()->getSurname());
					contentMap.insert(DATA_USER_KEY, lexical_cast<string>(resa.get<Customer>()->getKey()));
					contentMap.insert(DATA_USER_PHONE, resa.get<Customer>()->getPhone());
					contentMap.insert(DATA_USER_EMAIL, resa.get<Customer>()->getEMail());
					contentMap.insert(DATA_USER_LOGIN, resa.get<Customer>()->getLogin());

					if(!resa.get<Customer>()->getPassword().empty())
					{
						contentMap.insert(DATA_USER_PASSWORD, resa.get<Customer>()->getPassword());
					}
					else
					{
						contentMap.insert(DATA_USER_PASSWORD, TYPE_UNCHANGED_PASSWORD);
					}

					LoginToken token(LoginToken(resa.get<Customer>()->getLogin(), resa.get<Customer>()->getPasswordHash(), resa.getKey()));
					contentMap.insert(DATA_TOKEN_CANCELLATION, token.toString());
				}

				get<ConfirmationEmailCMS>()->display(content, contentMap);

				email.setContent(content.str());

				email.send();
				return true;
			}
			catch(boost::system::system_error)
			{
				return false;
			}
		}



		bool OnlineReservationRule::sendCustomerEMail(
			const std::vector<ReservationTransaction>& resas
		) const {

			if( !get<MultiReservationsEmailCMS>()
			){
				return false;
			}
			
			try
			{
				EMail email(ServerModule::GetEMailSender());

				// MIME type
				string mimeType = get<MultiReservationsEmailCMS>()->getMimeType();
				if (mimeType == "text/html") {
					email.setFormat(EMail::EMAIL_HTML);
				}
				else
				{
					email.setFormat(EMail::EMAIL_TEXT);
				}
				email.setSender(get<SenderEmail>());
				email.setSenderName(get<SenderName>());

				stringstream subject;
				ParametersMap subjectMap;

				subjectMap.insert(DATA_SUBJECT_OR_CONTENT, TYPE_SUBJECT);
				subjectMap.insert(DATA_DEPARTURE_DATE, to_simple_string((*resas.front().getReservations().begin())->get<DepartureTime>().date()));
				subjectMap.insert(DATA_DEPARTURE_CITY_NAME, (*resas.front().getReservations().begin())->get<DepartureCityName>());
				subjectMap.insert(DATA_DEPARTURE_PLACE_NAME, (*resas.front().getReservations().begin())->get<DeparturePlaceNameNoCity>());
				subjectMap.insert(DATA_ARRIVAL_CITY_NAME, (*resas.front().getReservations().rbegin())->get<ArrivalCityName>());
				subjectMap.insert(DATA_ARRIVAL_PLACE_NAME, (*resas.front().getReservations().rbegin())->get<ArrivalPlaceNameNoCity>());
				subjectMap.insert(DATA_RESERVATIONS_NUMBER, resas.size());

				get<MultiReservationsEmailCMS>()->display(subject, subjectMap);

				email.setSubject(subject.str());

				email.addRecipient(resas.front().get<CustomerEmail>(), resas.front().get<CustomerName>());

				stringstream content;
				ParametersMap contentMap;

				contentMap.insert(DATA_SUBJECT_OR_CONTENT, TYPE_CONTENT);
	
				int servicesNumber = 0;
				boost::shared_ptr<ParametersMap> resasMap(new ParametersMap);
				BOOST_FOREACH(const ReservationTransaction resa, resas)
				{
					boost::shared_ptr<ParametersMap> resaTransactionMap(new ParametersMap);
					boost::shared_ptr<ParametersMap> resaRoadMap(new ParametersMap);

					servicesNumber = 0;
					BOOST_FOREACH(const Reservation* r, resa.getReservations())
					{
						boost::shared_ptr<ParametersMap> resaMap(new ParametersMap);
						resaMap->insert(DATA_DEPARTURE_TIME, to_simple_string(r->get<DepartureTime>().time_of_day()));
						resaMap->insert(DATA_DEPARTURE_CITY_NAME, r->get<DepartureCityName>());
						resaMap->insert(DATA_DEPARTURE_PLACE_NAME, r->get<DeparturePlaceNameNoCity>());
						resaMap->insert(DATA_ARRIVAL_TIME, to_simple_string(r->get<ArrivalTime>().time_of_day()));
						resaMap->insert(DATA_ARRIVAL_CITY_NAME, r->get<ArrivalCityName>());
						resaMap->insert(DATA_ARRIVAL_PLACE_NAME, r->get<ArrivalPlaceNameNoCity>());
						resaMap->insert(DATA_LINE_CODE, r->get<LineCode>());
						resaMap->insert(DATA_IS_RESERVATION_POSSIBLE, r->get<IsReservationPossible>());
						resaRoadMap->insert(DATA_ROAD_RESA, resaMap);

						if (r->get<IsReservationPossible>())
							servicesNumber++;
					}

					LoginToken token(LoginToken(resas.front().get<Customer>()->getLogin(),resas.front().get<Customer>()->getPasswordHash(),resa.getKey()));

					resaTransactionMap->insert(DATA_DEPARTURE_DATE, to_simple_string((*resa.getReservations().begin())->get<DepartureTime>().date()));
					resaTransactionMap->insert(DATA_KEY_RESA, lexical_cast<string>(resa.getKey()));
					resaTransactionMap->insert(DATA_TOKEN_CANCELLATION, token.toString());
					resaTransactionMap->insert(DATA_SEATS_NUMBER, lexical_cast<string>(resa.get<Seats>()));
					resaTransactionMap->insert(DATA_SERVICES_NUMBER, servicesNumber);
					resaTransactionMap->insert(DATA_ROAD_RESAS, resaRoadMap);

					resasMap->insert(DATA_TRANSACTION_RESERVATION, resaTransactionMap);
				}
			
				contentMap.insert(DATA_RESERVATIONS, resasMap);
				contentMap.insert(DATA_RESERVATIONS_NUMBER, resas.size());
				contentMap.insert(DATA_CUSTOMER_ID, lexical_cast<string>(resas.front().get<Customer>()->getKey()));
				contentMap.insert(DATA_RESERVATION_DEAD_LINE_DATE, to_simple_string(resas.front().getReservationDeadLine().date()));
				contentMap.insert(DATA_RESERVATION_DEAD_LINE_TIME, to_simple_string(resas.front().getReservationDeadLine().time_of_day()));
				contentMap.insert(DATA_DEPARTURE_CITY_NAME, (*resas.front().getReservations().begin())->get<DepartureCityName>());
				contentMap.insert(DATA_DEPARTURE_PLACE_NAME, (*resas.front().getReservations().begin())->get<DeparturePlaceNameNoCity>());
				contentMap.insert(DATA_ARRIVAL_CITY_NAME, (*resas.front().getReservations().rbegin())->get<ArrivalCityName>());
				contentMap.insert(DATA_ARRIVAL_PLACE_NAME, (*resas.front().getReservations().rbegin())->get<ArrivalPlaceNameNoCity>());
				contentMap.insert(DATA_CUSTOMER_PHONE, resas.front().get<CustomerPhone>());
				
				if (resas.front().get<Customer>())
				{
					contentMap.insert(DATA_USER_NAME, resas.front().get<Customer>()->getName());
					contentMap.insert(DATA_USER_SURNAME, resas.front().get<Customer>()->getSurname());
					contentMap.insert(DATA_USER_KEY, lexical_cast<string>(resas.front().get<Customer>()->getKey()));
					contentMap.insert(DATA_USER_PHONE, resas.front().get<Customer>()->getPhone());
					contentMap.insert(DATA_USER_EMAIL, resas.front().get<Customer>()->getEMail());
					contentMap.insert(DATA_USER_LOGIN, resas.front().get<Customer>()->getLogin());

					if(!resas.front().get<Customer>()->getPassword().empty())
					{
						contentMap.insert(DATA_USER_PASSWORD, resas.front().get<Customer>()->getPassword());
					}
					else
					{
						contentMap.insert(DATA_USER_PASSWORD, TYPE_UNCHANGED_PASSWORD);
					}
				}

				get<MultiReservationsEmailCMS>()->display(content, contentMap);

				email.setContent(content.str());

				email.send();
				return true;
			}
			catch(boost::system::system_error)
			{
				return false;
			}
		}



		bool OnlineReservationRule::sendCustomerEMail( const security::User& customer ) const
		{
			if(	!get<PasswordEmailCMS>() ||
				customer.getEMail().empty()
			){
				return false;
			}

			// Use CMS to display EMail
			try
			{
				EMail email(ServerModule::GetEMailSender());
				string mimeType = get<PasswordEmailCMS>()->getMimeType();
				if (mimeType == "text/html") {
					email.setFormat(EMail::EMAIL_HTML);
				}
				else
				{
					email.setFormat(EMail::EMAIL_TEXT);
				}
				email.setSender(get<SenderEmail>());
				email.setSenderName(get<SenderName>());

				stringstream subject;
				ParametersMap subjectMap;

				subjectMap.insert(DATA_SUBJECT_OR_CONTENT, TYPE_SUBJECT);
				subjectMap.insert(DATA_USER_LOGIN, customer.getLogin());

				get<PasswordEmailCMS>()->display(subject, subjectMap);

				email.setSubject(subject.str());
				email.addRecipient(customer.getEMail(), customer.getFullName());

				stringstream content;
				ParametersMap contentMap;

				contentMap.insert(DATA_SUBJECT_OR_CONTENT, TYPE_CONTENT);
				contentMap.insert(DATA_USER_SURNAME, customer.getSurname());
				contentMap.insert(DATA_USER_NAME, customer.getName());
				contentMap.insert(DATA_USER_KEY, lexical_cast<string>(customer.getKey()));
				contentMap.insert(DATA_USER_PHONE, customer.getPhone());
				contentMap.insert(DATA_USER_EMAIL, customer.getEMail());
				contentMap.insert(DATA_USER_LOGIN, customer.getLogin());
				if(!customer.getPassword().empty())
				{
					contentMap.insert(DATA_USER_PASSWORD, customer.getPassword());
				}
				else
				{
					contentMap.insert(DATA_USER_PASSWORD, TYPE_UNCHANGED_PASSWORD);
				}

				get<PasswordEmailCMS>()->display(content, contentMap);

				email.setContent(content.str());

				email.send();
				return true;
			}
			catch(boost::system::system_error)
			{
				return false;
			}
		}



		void OnlineReservationRule::setSenderEMail( const std::string& value )
		{
			set<SenderEmail>(value);
		}



		void OnlineReservationRule::setSenderName( const std::string& value )
		{
			set<SenderName>(value);
		}



		const std::string& OnlineReservationRule::getSenderEMail() const
		{
			return get<SenderEmail>();
		}



		const std::string& OnlineReservationRule::getSenderName() const
		{
			return get<SenderName>();
		}



		bool OnlineReservationRule::sendCustomerCancellationEMail(
			const ReservationTransaction& resa,
			bool isBecauseOfAbsence
		) const	{
			if(	!get<CancellationEmailCMS>()
			){
				return false;
			}

			// Use CMS to display EMail
			try
			{
				EMail email(ServerModule::GetEMailSender());
				string mimeType = get<CancellationEmailCMS>()->getMimeType();
				if (mimeType == "text/html") {
					email.setFormat(EMail::EMAIL_HTML);
				}
				else
				{
					email.setFormat(EMail::EMAIL_TEXT);
				}
				email.setSender(get<SenderEmail>());
				email.setSenderName(get<SenderName>());

				stringstream subject;
				ParametersMap subjectMap;

				subjectMap.insert(DATA_SUBJECT_OR_CONTENT, TYPE_SUBJECT);
				subjectMap.insert(DATA_DEPARTURE_DATE, to_simple_string((*resa.getReservations().begin())->get<DepartureTime>().date()));
				subjectMap.insert(DATA_DEPARTURE_CITY_NAME, (*resa.getReservations().begin())->get<DepartureCityName>());
				subjectMap.insert(DATA_DEPARTURE_PLACE_NAME, (*resa.getReservations().begin())->get<DeparturePlaceNameNoCity>());
				subjectMap.insert(DATA_ARRIVAL_CITY_NAME, (*resa.getReservations().rbegin())->get<ArrivalCityName>());
				subjectMap.insert(DATA_ARRIVAL_PLACE_NAME, (*resa.getReservations().rbegin())->get<ArrivalPlaceNameNoCity>());
				subjectMap.insert(DATA_CANCELLATION_BECAUSE_OF_ABSENCE, isBecauseOfAbsence);

				get<CancellationEmailCMS>()->display(subject, subjectMap);
					
				email.setSubject(subject.str());
				email.addRecipient(resa.get<CustomerEmail>(), resa.get<CustomerName>());

				stringstream content;
				ParametersMap contentMap;

				contentMap.insert(DATA_SUBJECT_OR_CONTENT, TYPE_CONTENT);
				boost::shared_ptr<ParametersMap> roadResaMap(new ParametersMap);
				BOOST_FOREACH(const Reservation* r, resa.getReservations())
				{
					boost::shared_ptr<ParametersMap> resaMap(new ParametersMap);
					resaMap->insert(DATA_DEPARTURE_TIME, to_simple_string(r->get<DepartureTime>().time_of_day()));
					resaMap->insert(DATA_DEPARTURE_CITY_NAME, r->get<DepartureCityName>());
					resaMap->insert(DATA_DEPARTURE_PLACE_NAME, r->get<DeparturePlaceNameNoCity>());
					resaMap->insert(DATA_ARRIVAL_TIME, to_simple_string(r->get<ArrivalTime>().time_of_day()));
					resaMap->insert(DATA_ARRIVAL_CITY_NAME, r->get<ArrivalCityName>());
					resaMap->insert(DATA_ARRIVAL_PLACE_NAME, r->get<ArrivalPlaceNameNoCity>());
					resaMap->insert(DATA_LINE_CODE, r->get<LineCode>());
					resaMap->insert(DATA_IS_RESERVATION_POSSIBLE, r->get<IsReservationPossible>());
					roadResaMap->insert(DATA_ROAD_RESA, resaMap);
				}
				contentMap.insert(DATA_ROAD_RESAS, roadResaMap);
				contentMap.insert(DATA_KEY_RESA, lexical_cast<string>(resa.getKey()));
				contentMap.insert(DATA_CUSTOMER_ID, lexical_cast<string>(resa.get<Customer>()->getKey()));
				contentMap.insert(DATA_RESERVATION_DEAD_LINE_DATE, to_simple_string(resa.getReservationDeadLine().date()));
				contentMap.insert(DATA_RESERVATION_DEAD_LINE_TIME, to_simple_string(resa.getReservationDeadLine().time_of_day()));
				contentMap.insert(DATA_DEPARTURE_CITY_NAME, (*resa.getReservations().begin())->get<DepartureCityName>());
				contentMap.insert(DATA_DEPARTURE_PLACE_NAME, (*resa.getReservations().begin())->get<DeparturePlaceNameNoCity>());
				contentMap.insert(DATA_ARRIVAL_CITY_NAME, (*resa.getReservations().rbegin())->get<ArrivalCityName>());
				contentMap.insert(DATA_ARRIVAL_PLACE_NAME, (*resa.getReservations().rbegin())->get<ArrivalPlaceNameNoCity>());
				contentMap.insert(DATA_DEPARTURE_DATE, to_simple_string((*resa.getReservations().begin())->get<DepartureTime>().date()));
				contentMap.insert(DATA_SEATS_NUMBER, lexical_cast<string>(resa.get<Seats>()));

				if (resa.get<Customer>())
				{
					contentMap.insert(DATA_USER_NAME, resa.get<Customer>()->getName());
					contentMap.insert(DATA_USER_SURNAME, resa.get<Customer>()->getSurname());
					contentMap.insert(DATA_USER_KEY, lexical_cast<string>(resa.get<Customer>()->getKey()));
					contentMap.insert(DATA_USER_PHONE, resa.get<Customer>()->getPhone());
					contentMap.insert(DATA_USER_EMAIL, resa.get<Customer>()->getEMail());
					contentMap.insert(DATA_USER_LOGIN, resa.get<Customer>()->getLogin());

					if(!resa.get<Customer>()->getPassword().empty())
					{
						contentMap.insert(DATA_USER_PASSWORD, resa.get<Customer>()->getPassword());
					}
					else
					{
						contentMap.insert(DATA_USER_PASSWORD, TYPE_UNCHANGED_PASSWORD);
					}
				}

				contentMap.insert(DATA_CANCELLATION_BECAUSE_OF_ABSENCE, isBecauseOfAbsence);

				get<CancellationEmailCMS>()->display(content, contentMap);
				email.setContent(content.str());

				email.send();
				return true;
			}
			catch(boost::system::system_error)
			{
				return false;
			}
		}

		bool OnlineReservationRule::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(security::WRITE);
		}

		bool OnlineReservationRule::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(security::WRITE);
		}

		bool OnlineReservationRule::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(security::DELETE_RIGHT);
		}
	}
}
