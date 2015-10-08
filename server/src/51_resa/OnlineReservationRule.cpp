
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

#include "ServerModule.h"
#include "EMail.h"
#include "ReservationTransaction.h"
#include "OnlineReservationRule.h"
#include "Registry.h"
#include "ResaModule.h"
#include "PTModule.h"
#include "User.h"
#include "UserTableSync.h"
#include "LoginToken.hpp"
#include "ReservationContact.h"
#include "Webpage.h"
#include "Reservation.h"

#include "UtilConstants.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace pt;
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace pt;
	using namespace cms;

	namespace util
	{
		template<> const string Registry<resa::OnlineReservationRule>::KEY("OnlineReservationRule");
	}

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
			_reservationRule(NULL)
		{
		}

		const std::string& OnlineReservationRule::getEMail() const
		{
			return _eMail;
		}

		const std::string& OnlineReservationRule::getCopyEMail() const
		{
			return _copyEMail;
		}

		boost::logic::tribool OnlineReservationRule::getNeedsSurname() const
		{
			return _needsSurname;
		}

		boost::logic::tribool OnlineReservationRule::getNeedsAddress() const
		{
			return _needsAddress;
		}

		boost::logic::tribool OnlineReservationRule::getNeedsPhone() const
		{
			return _needsPhone;
		}

		boost::logic::tribool OnlineReservationRule::getNeedsCustomerNumber() const
		{
			return _needsCustomerNumber;
		}

		boost::logic::tribool OnlineReservationRule::getNeedsEMail() const
		{
			return _needsEMail;
		}



		const OnlineReservationRule::CapacityThresholds& OnlineReservationRule::getThresholds() const
		{
			return _thresholds;
		}

		void OnlineReservationRule::setReservationContact(const ReservationContact* rule)
		{
			if (_reservationRule != NULL)
			{
				OnlineReservationRuleMap::iterator it(_onlineReservationRuleMap.find(_reservationRule->getKey()));
				assert(it != _onlineReservationRuleMap.end());
				_onlineReservationRuleMap.erase(it);
			}
			_reservationRule = rule;
			_onlineReservationRuleMap.insert(make_pair(rule->getKey(), this));
		}

		void OnlineReservationRule::setEMail( const std::string& email )
		{
			_eMail = email;
		}

		void OnlineReservationRule::setCopyEMail( const std::string& email )
		{
			_copyEMail = email;
		}

		void OnlineReservationRule::setNeedsSurname( boost::logic::tribool value )
		{
			_needsSurname = value;
		}

		void OnlineReservationRule::setNeedsAddress( boost::logic::tribool value )
		{
			_needsAddress = value;
		}

		void OnlineReservationRule::setNeedsPhone( boost::logic::tribool value )
		{
			_needsPhone = value;
		}

		void OnlineReservationRule::setNeedsCustomerNumber( boost::logic::tribool value )
		{
			_needsCustomerNumber = value;
		}

		void OnlineReservationRule::setNeedsEMail( boost::logic::tribool value )
		{
			_needsEMail = value;
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

			if(	!_cmsConfirmationEMail.get()
			){
				return false;
			}

			// Use CMS to display EMail
			try
			{
				EMail email(ServerModule::GetEMailSender());

				// MIME type
				string mimeType = _cmsConfirmationEMail.get()->getMimeType();
				if (mimeType == "text/html") {
					email.setFormat(EMail::EMAIL_HTML);
				}
				else
				{
					email.setFormat(EMail::EMAIL_TEXT);
				}
				email.setSender(_senderEMail);
				email.setSenderName(_senderName);

				stringstream subject;
				ParametersMap subjectMap;

				subjectMap.insert(DATA_SUBJECT_OR_CONTENT, TYPE_SUBJECT);
				subjectMap.insert(DATA_DEPARTURE_DATE, to_simple_string((*resa.getReservations().begin())->get<DepartureTime>().date()));
				subjectMap.insert(DATA_DEPARTURE_CITY_NAME, (*resa.getReservations().begin())->get<DepartureCityName>());
				subjectMap.insert(DATA_DEPARTURE_PLACE_NAME, (*resa.getReservations().begin())->get<DeparturePlaceNameNoCity>());
				subjectMap.insert(DATA_ARRIVAL_CITY_NAME, (*resa.getReservations().rbegin())->get<ArrivalCityName>());
				subjectMap.insert(DATA_ARRIVAL_PLACE_NAME, (*resa.getReservations().rbegin())->get<ArrivalPlaceNameNoCity>());

				_cmsConfirmationEMail.get()->display(subject, subjectMap);

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

				_cmsConfirmationEMail.get()->display(content, contentMap);

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

			if( !_cmsMultiReservationsEMail.get()
			){
				return false;
			}
			
			try
			{
				EMail email(ServerModule::GetEMailSender());

				// MIME type
				string mimeType = _cmsMultiReservationsEMail.get()->getMimeType();
				if (mimeType == "text/html") {
					email.setFormat(EMail::EMAIL_HTML);
				}
				else
				{
					email.setFormat(EMail::EMAIL_TEXT);
				}
				email.setSender(_senderEMail);
				email.setSenderName(_senderName);

				stringstream subject;
				ParametersMap subjectMap;

				subjectMap.insert(DATA_SUBJECT_OR_CONTENT, TYPE_SUBJECT);
				subjectMap.insert(DATA_DEPARTURE_DATE, to_simple_string((*resas.front().getReservations().begin())->get<DepartureTime>().date()));
				subjectMap.insert(DATA_DEPARTURE_CITY_NAME, (*resas.front().getReservations().begin())->get<DepartureCityName>());
				subjectMap.insert(DATA_DEPARTURE_PLACE_NAME, (*resas.front().getReservations().begin())->get<DeparturePlaceNameNoCity>());
				subjectMap.insert(DATA_ARRIVAL_CITY_NAME, (*resas.front().getReservations().rbegin())->get<ArrivalCityName>());
				subjectMap.insert(DATA_ARRIVAL_PLACE_NAME, (*resas.front().getReservations().rbegin())->get<ArrivalPlaceNameNoCity>());
				subjectMap.insert(DATA_RESERVATIONS_NUMBER, resas.size());

				_cmsMultiReservationsEMail.get()->display(subject, subjectMap);

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

				_cmsMultiReservationsEMail.get()->display(content, contentMap);

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
			if(	!_cmsPasswordEMail.get() ||
				customer.getEMail().empty()
			){
				return false;
			}

			// Use CMS to display EMail
			try
			{
				EMail email(ServerModule::GetEMailSender());
				string mimeType = _cmsPasswordEMail.get()->getMimeType();
				if (mimeType == "text/html") {
					email.setFormat(EMail::EMAIL_HTML);
				}
				else
				{
					email.setFormat(EMail::EMAIL_TEXT);
				}
				email.setSender(_senderEMail);
				email.setSenderName(_senderName);

				stringstream subject;
				ParametersMap subjectMap;

				subjectMap.insert(DATA_SUBJECT_OR_CONTENT, TYPE_SUBJECT);
				subjectMap.insert(DATA_USER_LOGIN, customer.getLogin());

				_cmsPasswordEMail.get()->display(subject, subjectMap);

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

				_cmsPasswordEMail.get()->display(content, contentMap);

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
			_senderEMail = value;
		}



		void OnlineReservationRule::setSenderName( const std::string& value )
		{
			_senderName = value;
		}



		const std::string& OnlineReservationRule::getSenderEMail() const
		{
			return _senderEMail;
		}



		const std::string& OnlineReservationRule::getSenderName() const
		{
			return _senderName;
		}



		void OnlineReservationRule::setConfirmationEMailCMS( boost::shared_ptr<const cms::Webpage> value )
		{
			_cmsConfirmationEMail = value;
		}

		void OnlineReservationRule::setMultiReservationsEMailCMS( boost::shared_ptr<const cms::Webpage> value )
		{
			_cmsMultiReservationsEMail = value;
		}

		void OnlineReservationRule::setCancellationEMailCMS( boost::shared_ptr<const cms::Webpage> value )
		{
			_cmsCancellationEMail = value;
		}

		void OnlineReservationRule::setPasswordEMailCMS( boost::shared_ptr<const cms::Webpage> value )
		{
			_cmsPasswordEMail = value;
		}



		bool OnlineReservationRule::sendCustomerCancellationEMail(
			const ReservationTransaction& resa,
			bool isBecauseOfAbsence
		) const	{
			if(	!_cmsCancellationEMail.get()
			){
				return false;
			}

			// Use CMS to display EMail
			try
			{
				EMail email(ServerModule::GetEMailSender());
				string mimeType = _cmsCancellationEMail.get()->getMimeType();
				if (mimeType == "text/html") {
					email.setFormat(EMail::EMAIL_HTML);
				}
				else
				{
					email.setFormat(EMail::EMAIL_TEXT);
				}
				email.setSender(_senderEMail);
				email.setSenderName(_senderName);

				stringstream subject;
				ParametersMap subjectMap;

				subjectMap.insert(DATA_SUBJECT_OR_CONTENT, TYPE_SUBJECT);
				subjectMap.insert(DATA_DEPARTURE_DATE, to_simple_string((*resa.getReservations().begin())->get<DepartureTime>().date()));
				subjectMap.insert(DATA_DEPARTURE_CITY_NAME, (*resa.getReservations().begin())->get<DepartureCityName>());
				subjectMap.insert(DATA_DEPARTURE_PLACE_NAME, (*resa.getReservations().begin())->get<DeparturePlaceNameNoCity>());
				subjectMap.insert(DATA_ARRIVAL_CITY_NAME, (*resa.getReservations().rbegin())->get<ArrivalCityName>());
				subjectMap.insert(DATA_ARRIVAL_PLACE_NAME, (*resa.getReservations().rbegin())->get<ArrivalPlaceNameNoCity>());
				subjectMap.insert(DATA_CANCELLATION_BECAUSE_OF_ABSENCE, isBecauseOfAbsence);

				_cmsCancellationEMail.get()->display(subject, subjectMap);
					
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

				_cmsCancellationEMail.get()->display(content, contentMap);
				email.setContent(content.str());

				email.send();
				return true;
			}
			catch(boost::system::system_error)
			{
				return false;
			}
		}
	}
}
