
/** BookReservationAction class header.
	@file BookReservationAction.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_BookReservationAction_H__
#define SYNTHESE_BookReservationAction_H__

#include "Action.h"
#include "Journey.h"
#include "FactorableTemplate.h"
#include "AccessParameters.h"

#include <boost/shared_ptr.hpp>
#include <string>

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace transportwebsite
	{
		class Site;
		class RollingStockFilter;
	}


	namespace resa
	{
		/** BookReservationAction action class.
			@ingroup m31Actions refActions

			The reservation is made upon a journey. Each journey leg allowing reservation
			is booked simultaneously.

			Two types of user can do a reservation :
			 - an operator (ResaRight public WRITE right) : can type a customer name or a customer id, must not type his password
			 - a customer (ResaRight private WRITE right) : can only modify the contact phone number and must retype his password even if it is already logged in

			The datetime must be precisely identical with the start time of the founded journey. If not, the reservation is cancelled and the route planning must be launched again.
			This case can occur if a hot update has been done while the customer asked for his reservation.

			Tip : XMLReservationFunction can be used to view the result of the action.

			<h3>Request</h3>

			<pre>
			a=bra
			sid=<id session> : ID de la session ouverte, issu du retour de la fonction de connexion d’utilisateur
			actionParamcuid=<id utilisateur> : ID de l’utilisateur courant, issu du retour de la fonction de connexion d’utilisateur
			actionParamsit=<site id> : ID of the site used by the route planner
			actionParamacc=35001|35002|35003 : type d’utilisateur pour filtrer sur les règles d’accessibilité et appliquer les règles de réservation adéquates. 35001 = piéton, 35002 = PMR, 35003 = vélo. Si non fourni, le calcul s’effectue pour un utilisateur piéton.
			actionParamdct=<commune de départ> : commune de départ sous forme de texte
			[actionParamdpt=<arrêt de départ>] : arrêt de départ sous forme de texte. Si non fourni, les arrêts principaux de la commune sont choisis.
			actionParamact=<commune d’arrivée> : commune d’arrivée sous forme de texte
			[actionParamapt=<arrêt d’arrivée>] : arrêt d’arrivée sous forme de texte. Si non fourni, les arrêts principaux de la commune sont choisis.
			actionParamda=YYYY-MM-DD HH:II : date et heure exacte du depart de la solution à réserver
			[actionParamtm =<id filtre mode de transport>] : identificateur de filtre sur les modes de transport (doit correspondre à un des filtres paramétrés sur le site)
			actionParamsenu=<nombre de places> : nombre de places à réserver
			</pre>
		*/
		class BookReservationAction
			: public util::FactorableTemplate<server::Action, BookReservationAction>
		{
		public:
		
			// Accessibility
			static const std::string PARAMETER_ACCESS_PARAMETERS;

			// Journey information
			static const std::string PARAMETER_ORIGIN_CITY;
			static const std::string PARAMETER_ORIGIN_PLACE;
			static const std::string PARAMETER_DESTINATION_CITY;
			static const std::string PARAMETER_DESTINATION_PLACE;
			static const std::string PARAMETER_DATE_TIME;

			// Customer information
			static const std::string PARAMETER_CREATE_CUSTOMER;
			static const std::string PARAMETER_CUSTOMER_ID;
			static const std::string PARAMETER_CUSTOMER_NAME;
			static const std::string PARAMETER_CUSTOMER_SURNAME;
			static const std::string PARAMETER_CUSTOMER_PHONE;
			static const std::string PARAMETER_CUSTOMER_EMAIL;

			// Requester information
			static const std::string PARAMETER_PASSWORD;

			// Reservation information
			static const std::string PARAMETER_SEATS_NUMBER;

			// Access parameters by site
			static const std::string PARAMETER_SITE;
			static const std::string PARAMETER_USER_CLASS_ID;
			static const std::string PARAMETER_ROLLING_STOCK_FILTER_ID;

		private:
			graph::Journey						_journey;
			boost::shared_ptr<security::User>	_customer;
			bool								_createCustomer;
			graph::AccessParameters				_accessParameters;
			int									_seatsNumber;
			boost::shared_ptr<const transportwebsite::Site>	_site;
			boost::shared_ptr<const transportwebsite::RollingStockFilter>	_rollingStockFilter;

		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			BookReservationAction();

			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			
			void setJourney(const graph::Journey& journey);
			void setAccessParameters(const graph::AccessParameters& value);

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_BookReservationAction_H__
