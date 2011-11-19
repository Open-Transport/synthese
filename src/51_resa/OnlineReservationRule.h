
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

#include <boost/logic/tribool.hpp>

#include <set>
#include <map>

namespace synthese
{
	namespace interfaces
	{
		class Interface;
	}

	namespace pt
	{
		class ReservationContact;
	}

	namespace security
	{
		class User;
	}

	namespace resa
	{
		class ReservationTransaction;

		/**	Online reservation rule class.
			@author Hugues Romain
			@date 2007
			@ingroup m31
		*/
		class OnlineReservationRule
		:	public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<OnlineReservationRule>	Registry;

			typedef std::set<int> CapacityThresholds;
			typedef std::map<util::RegistryKeyType, const OnlineReservationRule*> OnlineReservationRuleMap;

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
				interfaces::Interface*	_eMailInterface;
			//@}

			//! \name Optional form fields
			//@{
				boost::logic::tribool	_needsSurname;			//!< Prénom du client (Indifferent = champ affiché, remplissage facultatif)
				boost::logic::tribool	_needsAddress;			//!< Adresse du client (Indifferent = champ affiché, remplissage facultatif)
				boost::logic::tribool	_needsPhone;			//!< Numéro de téléphone du client (Indifferent = champ affiché, remplissage facultatif)
				boost::logic::tribool	_needsCustomerNumber;		//!< Numéro d'abonné du client (Indifferent = champ affiché, remplissage facultatif)
				boost::logic::tribool	_needsEMail;			//!< Adresse e-mail du client (Indifferent = champ affiché, remplissage facultatif)
			//@}

			//! \name Capacity
			//@{
				int					_maxSeats;				//!< Nombre maximal de réservations par service (UNKNOWN_VALUE = unlimited capacity)
				CapacityThresholds	_thresholds;				//!< Paliers de nombre de réservations générant un envoi de mail d'alerte
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
				int								getMaxSeats()				const;
				const CapacityThresholds&		getThresholds()				const;
				const std::string&				getSenderEMail()			const;
				const std::string&				getSenderName()				const;
				interfaces::Interface*			getEMailInterface()			const;
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
				void	setMaxSeats(int value);
				void	setThresholds(const CapacityThresholds& thresholds);
				void	setSenderEMail(const std::string& value);
				void	setSenderName(const std::string& value);
				void	setEMailInterface(interfaces::Interface* value);
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
				/// Sends e-mail to the customer containing his password.
				/// @param customer Reservation transaction
				/// @return true if the e-mail is sent.
				/// @author Hugues Romain
				/// @date 2009
				bool	sendCustomerEMail(
					const security::User& customer
				) const;


				bool sendCustomerCancellationEMail(
					const ReservationTransaction& resa
				) const;
			//@}

			//! \name Constructeurs et destructeurs
			//@{
				OnlineReservationRule(util::RegistryKeyType key);
				~OnlineReservationRule();
			//@}

		};
	}
}

#endif
