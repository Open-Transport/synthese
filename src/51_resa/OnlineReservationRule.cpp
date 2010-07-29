
/** OnlineReservationRule class implementation.
	@file OnlineReservationRule.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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
#include "ReservationConfirmationEMailInterfacePage.h"
#include "ReservationConfirmationEMailSubjectInterfacePage.h"
#include "Interface.h"
#include "CustomerPasswordEMailContentInterfacePage.h"
#include "CustomerPasswordEMailSubjectInterfacePage.h"
#include "User.h"
#include "ReservationCancellationEMailContentInterfacePage.h"
#include "ReservationCancellationEMailSubjectInterfacePage.h"
#include "ReservationContact.h"

#include "01_util/Constants.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace pt;
	using namespace util;
	using namespace server;
	using namespace interfaces;
	using namespace security;
	using namespace pt;

	namespace util
	{
		template<> const string Registry<resa::OnlineReservationRule>::KEY("OnlineReservationRule");
	}

	namespace resa
	{
		OnlineReservationRule::OnlineReservationRuleMap OnlineReservationRule::_onlineReservationRuleMap;

		OnlineReservationRule::OnlineReservationRule(
			RegistryKeyType key
		):	Registrable(key),
			_maxSeats(UNKNOWN_VALUE),
			_reservationRule(NULL),
			_eMailInterface(NULL)
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

		int OnlineReservationRule::getMaxSeats() const
		{
			return _maxSeats;
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

		void OnlineReservationRule::setMaxSeats( int value )
		{
			_maxSeats = value;
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

			if(	_eMailInterface == NULL ||
				_eMailInterface->getPage<ReservationConfirmationEMailInterfacePage>() == NULL ||
				_eMailInterface->getPage<ReservationConfirmationEMailSubjectInterfacePage>() == NULL
			){
				return false;
			}

			try
			{
				EMail email(ServerModule::GetEMailSender());
				email.setFormat(EMail::EMAIL_HTML);
				email.setSender(_senderEMail);
				email.setSenderName(_senderName);

				stringstream subject;
				VariablesMap v;
				_eMailInterface->getPage<ReservationConfirmationEMailSubjectInterfacePage>()->display(
					subject,
					resa,
					v
				);
				email.setSubject(subject.str());
				email.addRecipient(resa.getCustomerEMail(), resa.getCustomerName());
				
				stringstream content;
				_eMailInterface->getPage<ReservationConfirmationEMailInterfacePage>()->display(
					content,
					resa,
					v
				);
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
			if(	_eMailInterface == NULL ||
				_eMailInterface->getPage<CustomerPasswordEMailContentInterfacePage>() == NULL ||
				_eMailInterface->getPage<CustomerPasswordEMailSubjectInterfacePage>() == NULL ||
				customer.getEMail().empty()
			){
				return false;
			}

			try
			{
				EMail email(ServerModule::GetEMailSender());
				email.setFormat(EMail::EMAIL_HTML);
				email.setSender(_senderEMail);
				email.setSenderName(_senderName);

				VariablesMap v;
				stringstream subject;
				_eMailInterface->getPage<CustomerPasswordEMailSubjectInterfacePage>()->display(
					subject,
					customer,
					v
				);
				email.setSubject(subject.str());
				email.addRecipient(customer.getEMail(), customer.getFullName());

				stringstream content;
				_eMailInterface->getPage<CustomerPasswordEMailContentInterfacePage>()->display(
					content,
					customer,
					v
				);
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



		void OnlineReservationRule::setEMailInterface( interfaces::Interface* value )
		{
			_eMailInterface = value;
		}



		interfaces::Interface* OnlineReservationRule::getEMailInterface() const
		{
			return _eMailInterface;
		}



		bool OnlineReservationRule::sendCustomerCancellationEMail(
			const ReservationTransaction& resa
		) const	{
			if(	_eMailInterface == NULL ||
				_eMailInterface->getPage<ReservationCancellationEMailSubjectInterfacePage>() == NULL ||
				_eMailInterface->getPage<ReservationCancellationEMailContentInterfacePage>() == NULL
			){
				return false;
			}

			try
			{
				EMail email(ServerModule::GetEMailSender());
				email.setFormat(EMail::EMAIL_HTML);
				email.setSender(_senderEMail);
				email.setSenderName(_senderName);

				stringstream subject;
				VariablesMap v;
				_eMailInterface->getPage<ReservationCancellationEMailSubjectInterfacePage>()->display(
					subject,
					resa,
					v
					);
				email.setSubject(subject.str());
				email.addRecipient(resa.getCustomerEMail(), resa.getCustomerName());

				stringstream content;
				_eMailInterface->getPage<ReservationCancellationEMailContentInterfacePage>()->display(
					content,
					resa,
					v
					);
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
