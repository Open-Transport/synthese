////////////////////////////////////////////////////////////////////////////////
/// RoutePlannerFunction class header.
///	@file RoutePlannerFunction.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_RoutePlannerFunction_H__
#define SYNTHESE_RoutePlannerFunction_H__

#include "FunctionWithSite.h"
#include "AccessParameters.h"
#include "DateTime.h"
#include "FactorableTemplate.h"
#include "Site.h"
#include "RoutePlannerTypes.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace transportwebsite
	{
		class HourPeriod;
	}

	namespace geography
	{
		class Place;
		class NamedPlace;
	}

	namespace env
	{
		class PublicTransportStopZoneConnectionPlace;
		class PhysicalStop;
	}

	namespace routeplanner
	{
		class RoutePlannerInterfacePage;

		////////////////////////////////////////////////////////////////////
		/// Route planning Function class.
		///	@ingroup m53Functions refFunctions
		///
		/// <h2>Requête</h2>
		/// <h3>Parameters de base</h3>
		///		- fonction=<b>rp</b>
		///		- si=<site id> : ID Site
		///		- [sid=<session id>] : ID de la session ouverte, issu du retour de la fonction de connexion
		///		  d’utilisateur. Nécessaire uniquement pour faire appel au trajet favori de l’utilisateur (voir
		///		  plus loin)
		///
		///	<h3>Paramètres de temps</h3>
		///		- Choix 1a : désignation de la plage de calcul par jour et période :
		///			- dy=YYYY-MM-DD : date du calcul. YYYY=année, MM=mois, DD=jour. Si non fournie, la fiche
		///			  horaire est calculée pour le jour même.
		///			- pi=<id période> : identificateur de la période dans la base de données. Doit correspondre à
		///			  une période appartenant au site
		///		- Choix 1b : désignation de la plage de calcul par bornage :
		///			- [da=YYYY-MM-DD HH :MM] : heure de début de la fiche horaire (premier départ).
		///			  HH=heures, MM=minutes. Si non fournie et si heure limite d’arrivée non fournie, la fiche
		///			  horaire commence à la date et l’heure courante. Si non fournie et si heure limite d’arrivée
		///			  fournie, prend la valeur de l’heure limite d’arrivée diminuée de 24 heures et de deux
		///			  minutes par kilomètres à effectuer à vol d’oiseau
		///			- [ha=YYYY-MM-DD HH :MM] : heure limite de départ (fin de la fiche horaire). HH=heures,
		///			  MM=minutes. Si non fournie, la fiche horaire stoppe 24 heures après l’heure de début de la
		///			  fiche horaire. Si l’heure maximale est inférieure à l’heure minimale, alors elle est considérée
		///			  comme correspondant au lendemain matin
		///			- [ia=YYYY-MM-DD HH :MM] : heure limite d’arrivée (fin de la fiche horaire). HH=heures,
		///			  MM=minutes. Si non fournie, l’heure limite d’arrivée est égale à l’heure limite de départ. Si
		///			  l’heure maximale est inférieure à l’heure minimale, alors elle est considérée comme
		///			  correspondant au lendemain matin
		///			- [ii=YYYY-MM-DD HH :MM] : heure de début de la fiche horaire (première arrivée).
		///			  HH=heures, MM=minutes. Si non fournie, la fiche horaire commence à l’heure limite
		///			  d’arrivée diminuée de 24 heures, ou l’heure courante si la date correspond au jour même.
		/// <h3>Parameters de lieux</h3>
		///		- Choix 2a : désignation des lieux de départ et d’arrivée par texte :
		///			- dct=<commune de départ> : commune de départ sous forme de texte
		///			- [dpt=<arrêt de départ>] : arrêt de départ sous forme de texte. Si non fourni, les arrêts
		///			  principaux de la commune sont choisis.
		///			- act=<commune d’arrivée> : commune d’arrivée sous forme de texte
		///			- [apt=<arrêt d’arrivée>] : arrêt d’arrivée sous forme de texte. Si non fourni, les arrêts
		///			  principaux de la commune sont choisis.
		///		- Choix 2b : désignation des lieux de départ et d’arrivée par pré-enregistrement
		///			- fid=<id trajet favori> : identificateur de trajet favori de l’utilisateur connecté
		///			  Paramètres
		/// <h3>Paramètres de calcul</h3>
		///		- [msn=<nombre de solutions maximum>] : si non fourni, le nombre de solutions calculées
		///		  est illimité. msn doit être un nombre strictement supérieur à 0.
		///		- [ac=35001|35002|35003] : type d’utilisateur pour filtrer sur les règles d’accessibilité et
		///		  appliquer les règles de réservation adéquates. 35001 = piéton, 35002 = PMR, 35003 = vélo.
		///		  Si non fourni, le calcul s’effectue pour un utilisateur piéton.
		///		- [tm=<id filtre mode de transport>] : identificateur de filtre
		///
		/// <h2>Response</h2>
		///		- if the site links to an interface containing a RoutePlannerInterfacePage object, the output is generated
		///		  by the interface
		///		- else a XML output is generated
		class RoutePlannerFunction : public util::FactorableTemplate<transportwebsite::FunctionWithSite,RoutePlannerFunction>
		{
		public:
			static const std::string PARAMETER_SITE;
			static const std::string PARAMETER_MAX_SOLUTIONS_NUMBER;
			static const std::string PARAMETER_DEPARTURE_CITY_TEXT;
			static const std::string PARAMETER_ARRIVAL_CITY_TEXT;
			static const std::string PARAMETER_DEPARTURE_PLACE_TEXT;
			static const std::string PARAMETER_ARRIVAL_PLACE_TEXT;
			static const std::string PARAMETER_ACCESSIBILITY;
			static const std::string PARAMETER_FAVORITE_ID;
			static const std::string PARAMETER_DAY;
			static const std::string PARAMETER_PERIOD_ID;
			static const std::string PARAMETER_LOWEST_DEPARTURE_TIME;
			static const std::string PARAMETER_LOWEST_ARRIVAL_TIME;
			static const std::string PARAMETER_HIGHEST_DEPARTURE_TIME;
			static const std::string PARAMETER_HIGHEST_ARRIVAL_TIME;
			
		private:
			//! \name Parameters
			//@{
				const RoutePlannerInterfacePage*			_page;
				transportwebsite::Site::ExtendedFetchPlaceResult	_departure_place;
				transportwebsite::Site::ExtendedFetchPlaceResult	_arrival_place;
				std::string									_originCityText;
				std::string									_destinationCityText;
				std::string									_originPlaceText;
				std::string									_destinationPlaceText;
				time::DateTime								_startDate;
				time::DateTime								_endDate;
				time::DateTime								_startArrivalDate;
				time::DateTime								_endArrivalDate;
				algorithm::PlanningOrder					_planningOrder;
				graph::AccessParameters						_accessParameters;
				boost::optional<std::size_t>				_maxSolutionsNumber;
				std::size_t									_periodId;
				const transportwebsite::HourPeriod*			_period;
				bool										_home;
				boost::optional<util::RegistryKeyType>		_favoriteId;
			//@}


			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

			static void _XMLDisplayConnectionPlace(
				std::ostream& stream,
				const geography::NamedPlace& place
			);
			static void _XMLDisplayPhysicalStop(
				std::ostream& stream,
				const std::string& tag,
				const env::PhysicalStop& place
			);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(std::ostream& stream, const server::Request& request) const;

			const boost::optional<std::size_t>& getMaxSolutions() const;
			void setMaxSolutions(boost::optional<std::size_t> number);

			RoutePlannerFunction();

			virtual bool isAuthorized(const security::Profile& profile) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_RoutePlannerFunction_H__
