
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

#include "Object.hpp"

#include "ReservationContact.h"
#include "TriboolField.hpp"
#include "User.h"
#include "Webpage.h"

#include <map>
#include <set>
#include <boost/logic/tribool.hpp>
#include <boost/optional.hpp>

namespace synthese
{
	namespace resa
	{
		class ReservationTransaction;

		FIELD_POINTER(Contact, pt::ReservationContact)
		FIELD_STRING(Email)
		FIELD_STRING(CopyEmail)
		FIELD_TRIBOOL(NeedsSurname)
		FIELD_TRIBOOL(NeedsAddress)
		FIELD_TRIBOOL(NeedsPhone)
		FIELD_TRIBOOL(NeedsEmail)
		FIELD_TRIBOOL(NeedsCustomerNumber)
		FIELD_SIZE_T(MaxSeat)
		FIELD_STRING(Thresholds)
		FIELD_STRING(SenderEmail)
		FIELD_STRING(SenderName)
		FIELD_POINTER(ConfirmationEmailCMS, cms::Webpage)
		FIELD_POINTER(CancellationEmailCMS, cms::Webpage)
		FIELD_POINTER(PasswordEmailCMS, cms::Webpage)
		FIELD_POINTER(MultiReservationsEmailCMS, cms::Webpage)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Contact),
			FIELD(Email),										//!< Adresse e-mail du destinataire des mails d'états de réservations (ex: le transporteur)
			FIELD(CopyEmail),									//!< Adresse e-mail de copie des mails d'états de réservations (ex: l'organisme autoritaire)
			FIELD(NeedsSurname),								//!< Prénom du client (Indifferent = champ affiché, remplissage facultatif)
			FIELD(NeedsAddress),								//!< Adresse du client (Indifferent = champ affiché, remplissage facultatif)
			FIELD(NeedsPhone),									//!< Numéro de téléphone du client (Indifferent = champ affiché, remplissage facultatif)
			FIELD(NeedsEmail),									//!< Numéro d'abonné du client (Indifferent = champ affiché, remplissage facultatif)
			FIELD(NeedsCustomerNumber),							//!< Adresse e-mail du client (Indifferent = champ affiché, remplissage facultatif)
			FIELD(MaxSeat),
			FIELD(Thresholds),									//!< Paliers de nombre de réservations générant un envoi de mail d'alerte /!\ Not used
			FIELD(SenderEmail),
			FIELD(SenderName),
			FIELD(ConfirmationEmailCMS),
			FIELD(CancellationEmailCMS),
			FIELD(PasswordEmailCMS),
			FIELD(MultiReservationsEmailCMS)
		> OnlineReservationRuleSchema;

		/**	Online reservation rule class.
			@author Hugues Romain
			@date 2007
			@ingroup m51
		*/
		class OnlineReservationRule
		:	public virtual Object<OnlineReservationRule, OnlineReservationRuleSchema>
		{
		public:

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
				static OnlineReservationRuleMap _onlineReservationRuleMap;
			//@}

			//! \name Capacity
			//@{
				CapacityThresholds		_thresholds;			//!< Paliers de nombre de réservations générant un envoi de mail d'alerte
			//@}

		public:

			static const OnlineReservationRule* GetOnlineReservationRule(const pt::ReservationContact* rule);

			//! \name Getters
			//@{
				const pt::ReservationContact*	getReservationContact()		const { return get<Contact>() ? get<Contact>().get_ptr() : NULL; }
				const std::string&				getEMail()					const;
				const std::string&				getCopyEMail()				const;
				boost::logic::tribool			getNeedsSurname()			const;
				boost::logic::tribool			getNeedsAddress()			const;
				boost::logic::tribool			getNeedsPhone()				const;
				boost::logic::tribool			getNeedsCustomerNumber()	const;
				boost::logic::tribool			getNeedsEMail()				const;
				size_t							getMaxSeats()				const { return get<MaxSeat>(); }
				const CapacityThresholds&		getThresholds()				const;
				const std::string&				getSenderEMail()			const;
				const std::string&				getSenderName()				const;

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
				void	setMaxSeats(const size_t value){ set<MaxSeat>(value); }
				void	setThresholds(const CapacityThresholds& thresholds);
				void	setSenderEMail(const std::string& value);
				void	setSenderName(const std::string& value);
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

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;

		};
	}
}

#endif
