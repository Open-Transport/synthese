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
		/// <h2>Requ�te</h2>
		/// <h3>Parameters de base</h3>
		///		- fonction=<b>rp</b>
		///		- si=<site id> : ID Site
		///		- [sid=<session id>] : ID de la session ouverte, issu du retour de la fonction de connexion
		///		  d�utilisateur. N�cessaire uniquement pour faire appel au trajet favori de l�utilisateur (voir
		///		  plus loin)
		///
		///	<h3>Param�tres de temps</h3>
		///		- Choix 1a : d�signation de la plage de calcul par jour et p�riode :
		///			- dy=YYYY-MM-DD : date du calcul. YYYY=ann�e, MM=mois, DD=jour. Si non fournie, la fiche
		///			  horaire est calcul�e pour le jour m�me.
		///			- pi=<id p�riode> : identificateur de la p�riode dans la base de donn�es. Doit correspondre �
		///			  une p�riode appartenant au site
		///		- Choix 1b : d�signation de la plage de calcul par bornage :
		///			- [da=YYYY-MM-DD HH :MM] : heure de d�but de la fiche horaire (premier d�part).
		///			  HH=heures, MM=minutes. Si non fournie et si heure limite d�arriv�e non fournie, la fiche
		///			  horaire commence � la date et l�heure courante. Si non fournie et si heure limite d�arriv�e
		///			  fournie, prend la valeur de l�heure limite d�arriv�e diminu�e de 24 heures et de deux
		///			  minutes par kilom�tres � effectuer � vol d�oiseau
		///			- [ha=YYYY-MM-DD HH :MM] : heure limite de d�part (fin de la fiche horaire). HH=heures,
		///			  MM=minutes. Si non fournie, la fiche horaire stoppe 24 heures apr�s l�heure de d�but de la
		///			  fiche horaire. Si l�heure maximale est inf�rieure � l�heure minimale, alors elle est consid�r�e
		///			  comme correspondant au lendemain matin
		///			- [ia=YYYY-MM-DD HH :MM] : heure limite d�arriv�e (fin de la fiche horaire). HH=heures,
		///			  MM=minutes. Si non fournie, l�heure limite d�arriv�e est �gale � l�heure limite de d�part. Si
		///			  l�heure maximale est inf�rieure � l�heure minimale, alors elle est consid�r�e comme
		///			  correspondant au lendemain matin
		///			- [ii=YYYY-MM-DD HH :MM] : heure de d�but de la fiche horaire (premi�re arriv�e).
		///			  HH=heures, MM=minutes. Si non fournie, la fiche horaire commence � l�heure limite
		///			  d�arriv�e diminu�e de 24 heures, ou l�heure courante si la date correspond au jour m�me.
		/// <h3>Parameters de lieux</h3>
		///		- Choix 2a : d�signation des lieux de d�part et d�arriv�e par texte :
		///			- dct=<commune de d�part> : commune de d�part sous forme de texte
		///			- [dpt=<arr�t de d�part>] : arr�t de d�part sous forme de texte. Si non fourni, les arr�ts
		///			  principaux de la commune sont choisis.
		///			- act=<commune d�arriv�e> : commune d�arriv�e sous forme de texte
		///			- [apt=<arr�t d�arriv�e>] : arr�t d�arriv�e sous forme de texte. Si non fourni, les arr�ts
		///			  principaux de la commune sont choisis.
		///		- Choix 2b : d�signation des lieux de d�part et d�arriv�e par pr�-enregistrement
		///			- fid=<id trajet favori> : identificateur de trajet favori de l�utilisateur connect�
		///			  Param�tres
		/// <h3>Param�tres de calcul</h3>
		///		- [msn=<nombre de solutions maximum>] : si non fourni, le nombre de solutions calcul�es
		///		  est illimit�. msn doit �tre un nombre strictement sup�rieur � 0.
		///		- [ac=35001|35002|35003] : type d�utilisateur pour filtrer sur les r�gles d�accessibilit� et
		///		  appliquer les r�gles de r�servation ad�quates. 35001 = pi�ton, 35002 = PMR, 35003 = v�lo.
		///		  Si non fourni, le calcul s�effectue pour un utilisateur pi�ton.
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
