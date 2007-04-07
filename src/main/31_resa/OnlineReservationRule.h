
/** OnlineReservationRule class header.
	@file OnlineReservationRule.h

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

#ifndef SYNTHESE_CMODALITERESERVATIONENLIGNE_H
#define SYNTHESE_CMODALITERESERVATIONENLIGNE_H

#include <boost/logic/tribool.hpp>

#include "15_env/ReservationRule.h"

namespace synthese
{
	namespace resa
	{

		/**	Online reservation rule class.
			@author Hugues Romain
			@date 2007
		*/
		class OnlineReservationRule : public env::ReservationRule
		{
			//! \name Adresses d'envoi des états
			//@{
			std::string _eMail;		//!< Adresse e-mail du destinataire des mails d'états de réservations (ex: le transporteur)
			std::string	_copyEMail;	//!< Adresse e-mail de copie des mails d'états de réservations (ex: l'organisme autoritaire)
			//@}

			//! \name Champs optionnels affichés dans le formulaire
			//@{
			boost::logic::tribool	_needsSurname;			//!< Prénom du client (Indifferent = champ affiché, remplissage facultatif)
			boost::logic::tribool	_neetsaddress;			//!< Adresse du client (Indifferent = champ affiché, remplissage facultatif)
			boost::logic::tribool	_needsPhone;			//!< Numéro de téléphone du client (Indifferent = champ affiché, remplissage facultatif)
			boost::logic::tribool	_needsCustomerNumber;		//!< Numéro d'abonné du client (Indifferent = champ affiché, remplissage facultatif)
			boost::logic::tribool	_needsEMail;			//!< Adresse e-mail du client (Indifferent = champ affiché, remplissage facultatif)
			//@}

			//! \name champs de gestion des places
			//@{
			int				_maxSeats;				//!< Nombre maximal de réservations par service
			std::set<int>	_thresholds;				//!< Paliers de nombre de réservations générant un envoi de mail d'alerte
			//@}

		public:
			//! \name Accesseurs
			//@{
			//@}

			//! \name Calculateurs
			//@{
			/*	size_t		Disponibilite(const cTrain* Circulation, const cMoment&, cDatabaseManager* tBaseManager)	const;
			tSeuilResa	getSeuil(const int ancienneValeur, const int nouvelleValeur) 						const;
			bool		Reserver(const cTrain* Service	, const cGare* tPADepart, const cGare* tPAArrivee
			, const cMoment& DateDepart, const cTexteSQL& tNom, const cTexteSQL& tNomBrut
			, const cTexteSQL& tPrenom, const cTexteSQL& tAdresse, const cTexteSQL& tEmail
			, const cTexteSQL& tTelephone, const cTexteSQL& tNumAbonne
			, const cTexteSQL& tAdressePAArrivee, const cTexteSQL& tAdressePADepart
			, const size_t iNombrePlaces, cDatabaseManager* baseManager)				const;
			bool		Annuler(const cTexte& CodeReservation, cDatabaseManager* tBaseManager)				const;
			*/	//@}

			//! \name Modificateurs
			//@{
			//@}

			//! \name Constructeurs et destructeurs
			//@{
			//@}

		};
	}
}

#endif