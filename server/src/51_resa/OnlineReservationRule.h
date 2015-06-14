
/** OnlineReservationRule class header.
	@file OnlineReservationRule.h

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

#ifndef SYNTHESE_CMODALITERESERVATIONENLIGNE_H
#define SYNTHESE_CMODALITERESERVATIONENLIGNE_H

#include "Registrable.h"
#include "Registry.h"

#include <map>
#include <set>
#include <boost/logic/tribool.hpp>
#include <boost/optional.hpp>

namespace synthese
{
	namespace pt
	{
		class ReservationContact;
	}

	namespace security
	{
		class User;
	}

	namespace cms
	{
		class Webpage;
	}

	namespace resa
	{
		class ReservationTransaction;

		/**	Online reservation rule class.
			@author Hugues Romain
			@date 2007
			@ingroup m51
		*/
		class OnlineReservationRule
		:	public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<OnlineReservationRule>	Registry;

			typedef std::set<size_t> CapacityThresholds;
			typedef std::map<util::RegistryKeyType, const OnlineReservationRule*> OnlineReservationRuleMap;

			static const std::string DATA_SUBJECT_OR_CONTENT;
			static const std::string DATA_DEPARTURE_DATE;
			static const std::string DATA_DEPARTURE_CITY_NAME;
			static const std::string DATA_DEPARTURE_PLACE_NAME;
			static const std::string DATA_ARRIVAL_CITY_NAME;
			static const std::string DATA_ARRIVAL_PLACE_NAME;
			static const std::string DATA_DEPARTURE_TIME;
            static const std::string DATA_IS_RESERVATION_POSSIBLE;
			static const std::string DATA_ARRIVAL_TIME;
			static const std::string DATA_LINE_CODE;
			static const std::string DATA_SEATS_NUMBER;
			static const std::string DATA_ROAD_RESAS;
			static const std::string DATA_ROAD_RESA;
			static const std::string DATA_KEY_RESA;
			static const std::string DATA_CUSTOMER_ID;
			static const std::string DATA_RESERVATION_DEAD_LINE_DATE;
			static const std::string DATA_RESERVATION_DEAD_LINE_TIME;
			static const std::string DATA_CUSTOMER_NAME;
			static const std::string DATA_CUSTOMER_PHONE;
			static const std::string DATA_USER_LOGIN;
			static const std::string DATA_USER_SURNAME;
			static const std::string DATA_USER_NAME;
			static const std::string DATA_USER_KEY;
			static const std::string DATA_USER_PHONE;
			static const std::string DATA_USER_EMAIL;
			static const std::string DATA_USER_PASSWORD;
			static const std::string DATA_CANCELLATION_BECAUSE_OF_ABSENCE;
			static const std::string DATA_RESERVATIONS_NUMBER;
			static const std::string DATA_SERVICES_NUMBER;
			static const std::string DATA_RESERVATIONS;
			static const std::string DATA_TRANSACTION_RESERVATION;
			static const std::string DATA_TOKEN_CANCELLATION;


			static const std::string TYPE_SUBJECT;
			static const std::string TYPE_CONTENT;
			static const std::string TYPE_UNCHANGED_PASSWORD;


		private:
			//! \name Link with env reservation rules
			//@{
				const pt::ReservationContact* _reservationRule;
				static OnlineReservationRuleMap _onlineReservationRuleMap;
			//@}

			//! \name Addresses for reports sending
			//@{
				std::string _eMail;		//!< Adresse e-mail du destinataire des mails d'états de réservations (ex: le transporteur)
				std::string	_copyEMail;	//!< Adresse e-mail de copie des mails d'états de réservations (ex: l'organisme autoritaire)
			//@}

			//! \name Customer e-mail sending options
			//@{
				std::string _senderEMail;
				std::string _senderName;
				boost::shared_ptr<const cms::Webpage> _cmsConfirmationEMail;
				boost::shared_ptr<const cms::Webpage> _cmsMultiReservationsEMail;
				boost::shared_ptr<const cms::Webpage> _cmsCancellationEMail;
				boost::shared_ptr<const cms::Webpage> _cmsPasswordEMail;
			//@}

			//! \name Optional form fields
			//@{
				boost::logic::tribool	_needsSurname;			//!< Prénom du client (Indifferent = champ affiché, remplissage facultatif)
				boost::logic::tribool	_needsAddress;			//!< Adresse du client (Indifferent = champ affiché, remplissage facultatif)
				boost::logic::tribool	_needsPhone;			//!< Numéro de téléphone du client (Indifferent = champ affiché, remplissage facultatif)
				boost::logic::tribool	_needsCustomerNumber;	//!< Numéro d'abonné du client (Indifferent = champ affiché, remplissage facultatif)
				boost::logic::tribool	_needsEMail;			//!< Adresse e-mail du client (Indifferent = champ affiché, remplissage facultatif)
			//@}

			//! \name Capacity
			//@{
				boost::optional<size_t>	_maxSeats;				//!< Nombre maximal de réservations par service (undefined = unlimited capacity)
				CapacityThresholds		_thresholds;			//!< Paliers de nombre de réservations générant un envoi de mail d'alerte
			//@}

		public:

			static const OnlineReservationRule* GetOnlineReservationRule(const pt::ReservationContact* rule);

			//! \name Getters
			//@{
				const pt::ReservationContact*	getReservationContact()		const { return _reservationRule; }
				const std::string&				getEMail()					const;
				const std::string&				getCopyEMail()				const;
				boost::logic::tribool			getNeedsSurname()			const;
				boost::logic::tribool			getNeedsAddress()			const;
				boost::logic::tribool			getNeedsPhone()				const;
				boost::logic::tribool			getNeedsCustomerNumber()	const;
				boost::logic::tribool			getNeedsEMail()				const;
				const boost::optional<size_t>&	getMaxSeats()				const { return _maxSeats; }
				const CapacityThresholds&		getThresholds()				const;
				const std::string&				getSenderEMail()			const;
				const std::string&				getSenderName()				const;
				boost::shared_ptr<const cms::Webpage> getConfirmationEMailCMS() const { return _cmsConfirmationEMail; }
				boost::shared_ptr<const cms::Webpage> getMultiReservationsEMailCMS() const{ return _cmsMultiReservationsEMail; }
				boost::shared_ptr<const cms::Webpage> getCancellationEMailCMS() const { return _cmsCancellationEMail; }
				boost::shared_ptr<const cms::Webpage> getPasswordEMailCMS() const{ return _cmsPasswordEMail; }

			//@}

			//! \name Setters
			//@{
				void	setReservationContact(const pt::ReservationContact* value);
				void	setEMail(const std::string& email);
				void	setCopyEMail(const std::string& email);
				void	setNeedsSurname(boost::logic::tribool value);
				void	setNeedsAddress(boost::logic::tribool value);
				void	setNeedsPhone(boost::logic::tribool value);
				void	setNeedsCustomerNumber(boost::logic::tribool value);
				void	setNeedsEMail(boost::logic::tribool value);
				void	setMaxSeats(const boost::optional<size_t>& value){ _maxSeats = value; }
				void	setThresholds(const CapacityThresholds& thresholds);
				void	setSenderEMail(const std::string& value);
				void	setSenderName(const std::string& value);
				void	setConfirmationEMailCMS(boost::shared_ptr<const cms::Webpage> value);
				void	setMultiReservationsEMailCMS(boost::shared_ptr<const cms::Webpage> value);
				void	setCancellationEMailCMS(boost::shared_ptr<const cms::Webpage> value);
				void	setPasswordEMailCMS(boost::shared_ptr<const cms::Webpage> value);
			//@}

			//! \name Queries
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Sends resume e-mail to the customer after booking a new reservation.
				/// @param resa Reservation transaction
				/// @return true if the e-mail is sent.
				/// @author Hugues Romain
				/// @date 2009
				bool	sendCustomerEMail(
					const ReservationTransaction& resa
				) const;


				//////////////////////////////////////////////////////////////////////////
				/// Sends resume e-mail to the customer after booking a new reservation.
				/// @param resa Reservation transaction
				/// @return true if the e-mail is sent.
				/// @author Hugues Romain
				/// @date 2009
				bool	sendCustomerEMail(
					const std::vector<ReservationTransaction>& resas
				) const;


				//////////////////////////////////////////////////////////////////////////
				/// Sends e-mail to the customer containing his password.
				/// @param customer Reservation transaction
				/// @return true if the e-mail is sent.
				/// @author Hugues Romain
				/// @date 2009
				bool	sendCustomerEMail(
					const security::User& customer
				) const;


				bool sendCustomerCancellationEMail(
					const ReservationTransaction& resa,
					bool isBecauseOfAbsence = false
				) const;
			//@}

			//! \name Constructeurs et destructeurs
			//@{
				OnlineReservationRule(util::RegistryKeyType key = 0);
				~OnlineReservationRule();
			//@}

		};
	}
}

#endif
