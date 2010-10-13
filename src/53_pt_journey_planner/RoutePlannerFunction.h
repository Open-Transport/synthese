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
#include "FactorableTemplate.h"
#include "TransportWebsite.h"
#include "AlgorithmTypes.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace road
	{
		class Crossing;
		class RoadPlace;
	}

	namespace cms
	{
		class Webpage;
	}

	namespace pt_website
	{
		class HourPeriod;
		class RollingStockFilter;
	}

	namespace geography
	{
		class Place;
		class NamedPlace;
	}

	namespace pt
	{
		class StopPoint;
		class StopArea;
	}

	namespace pt_journey_planner
	{
		class RoutePlannerInterfacePage;
		class UserFavoriteJourney;

		////////////////////////////////////////////////////////////////////
		/// 53.15 Function : public transportation route planner.
		///	@ingroup m53Functions refFunctions
		/// @author Hugues Romain
		///
		/// <h3>Requï¿½te</h3>
		/// <h4>Base parameters</h4>
		///
		/// <ul>
		///		<li>fonction=<b>rp</b></li>
		///		<li>si=<site id> : ID Site</li>
		///		<li>[sid=<session id>] : ID de la session ouverte, issu du retour de la fonction de connexion
		///		  d'utilisateur. Nï¿½cessaire uniquement pour faire appel au trajet favori de l'utilisateur (voir
		///		  plus loin)</li>
		/// </ul>
		///
		///	<h4>Paramï¿½tres de temps</h4>
		/// <ul>
		///		<li>Choix 1a : dï¿½signation de la plage de calcul par jour et pï¿½riode :</li>
		///		<ul><li>dy=YYYY-MM-DD : date du calcul. YYYY=annï¿½e, MM=mois, DD=jour. Si non fournie, la fiche
		///			  horaire est calculï¿½e pour le jour mï¿½me.</li>
		///			<li>pi=<id pï¿½riode> : identificateur de la pï¿½riode dans la base de donnï¿½es. Doit correspondre ï¿½
		///			  une pï¿½riode appartenant au site</li></ul>
		///		<li>Choix 1b : dï¿½signation de la plage de calcul par bornage :</li>
		///		<ul><li>[da=YYYY-MM-DD HH :MM] : heure de dï¿½but de la fiche horaire (premier dï¿½part).
		///			  HH=heures, MM=minutes. Si non fournie et si heure limite d'arrivï¿½e non fournie, la fiche
		///			  horaire commence ï¿½ la date et l'heure courante. Si non fournie et si heure limite d'arrivï¿½e
		///			  fournie, prend la valeur de l'heure limite d'arrivï¿½e diminuï¿½e de 24 heures et de deux
		///			  minutes par kilomï¿½tres ï¿½ effectuer ï¿½ vol d'oiseau</li>
		///			<li>[ha=YYYY-MM-DD HH :MM] : heure limite de dï¿½part (fin de la fiche horaire). HH=heures,
		///			  MM=minutes. Si non fournie, la fiche horaire stoppe 24 heures aprï¿½s l'heure de dï¿½but de la
		///			  fiche horaire. Si l'heure maximale est infï¿½rieure ï¿½ l'heure minimale, alors elle est considï¿½rï¿½e
		///			  comme correspondant au lendemain matin</li>
		///			<li>[ia=YYYY-MM-DD HH :MM] : heure limite d'arrivï¿½e (fin de la fiche horaire). HH=heures,
		///			  MM=minutes. Si non fournie, l'heure limite d'arrivï¿½e est ï¿½gale ï¿½ l'heure limite de dï¿½part. Si
		///			  l'heure maximale est infï¿½rieure ï¿½ l'heure minimale, alors elle est considï¿½rï¿½e comme
		///			  correspondant au lendemain matin</li>
		///			<li>[ii=YYYY-MM-DD HH :MM] : heure de dï¿½but de la fiche horaire (premiï¿½re arrivï¿½e).
		///			  HH=heures, MM=minutes. Si non fournie, la fiche horaire commence ï¿½ l'heure limite
		///			  d'arrivï¿½e diminuï¿½e de 24 heures, ou l'heure courante si la date correspond au jour mï¿½me.</li></ul></ul>
		/// <h4>Parameters de lieux</h4>
		///		<ul><li>Choix 2a : dï¿½signation des lieux de dï¿½part et d'arrivï¿½e par texte :</li>
		///			<ul><li>dct=<commune de dï¿½part> : commune de dï¿½part sous forme de texte</li>
		///			<li>[dpt=<arrï¿½t de dï¿½part>] : arrï¿½t de dï¿½part sous forme de texte. Si non fourni, les arrï¿½ts
		///			  principaux de la commune sont choisis.</li>
		///			<li>act=<commune d'arrivï¿½e> : commune d'arrivï¿½e sous forme de texte</li>
		///			<li>[apt=<arrï¿½t d'arrivï¿½e>] : arrï¿½t d'arrivï¿½e sous forme de texte. Si non fourni, les arrï¿½ts
		///			  principaux de la commune sont choisis.</li></ul>
		///		<li>Choix 2b : dï¿½signation des lieux de dï¿½part et d'arrivï¿½e par prï¿½-enregistrement</li>
		///			<ul><li>fid=<id trajet favori> : identificateur de trajet favori de l'utilisateur connectï¿½
		///			  Paramï¿½tres</li></ul></ul>
		/// <h4>Paramï¿½tres de calcul</h4>
		///		<ul><li>[msn=<nombre de solutions maximum>] : si non fourni, le nombre de solutions calculï¿½es
		///		  est illimitï¿½. msn doit ï¿½tre un nombre strictement supï¿½rieur ï¿½ 0.</li>
		///		<li>[ac=35001|35002|35003] : type d'utilisateur pour filtrer sur les rï¿½gles d'accessibilitï¿½ et
		///		  appliquer les rï¿½gles de rï¿½servation adï¿½quates. 35001 = piï¿½ton, 35002 = PMR, 35003 = vï¿½lo.
		///		  Si non fourni, le calcul s'effectue pour un utilisateur piï¿½ton.</li>
		///		<li>[tm=<id filtre mode de transport>] : identificateur de filtre</li></ul>
		/// <h4>Paramï¿½tres d'affichage (sortie XML uniquement)</h4>
		///		<ul>
		///		<li>[showResTab=yes/no] : "yes" par dï¿½faut : choix d'afficher ou non la balise ResultTable</li>
		///		<li>[showCoords=yes/no] : "yes" par dï¿½faut : choix d'afficher ou non les attributs de coordonnï¿½es</li>
		///		</ul>
		/// <h3>Response</h3>
		///
		///		<ul>
		///		<li>if the site links to an interface containing a RoutePlannerInterfacePage object, the output is generated
		///		  by the interface</li>
		///		<li>else a XML output is generated</li></ul>
		///
		/// <h4>Racine</h4>
		///
		///	La rï¿½ponse se sï¿½pare en trois parties :
		///
		/// @image html routeplanner_result_main.png
		///
		///	<ul><li>query : un rappel de la requï¿½te, permettant de remplir ï¿½ nouveau un formulaire de re-interrogation du serveur</li>
		/// <li>journeys : un descriptif de chaque solution, permettant de construire les feuilles de route dï¿½taillï¿½es</li>
		/// <li>resultTable : une prï¿½sentation des heures de passage par arrï¿½t, organisï¿½es pour ï¿½tre affichï¿½es sous forme de tableau synthï¿½tique</li></ul>
		///	
		/// <h4>Rappel de la requï¿½te</h4>
		///
		///	La requï¿½te (query) est dï¿½finie comme suit :
		/// 
		/// @image html xml_query.png
		///
		/// <ul><li>Attributs :</li><ul>
		///		<li>siteId : identificateur du site demandï¿½</li>
		///		<li>userProfile : type d'utilisateur pour filtrer sur les rï¿½gles d'accessibilitï¿½ et appliquer les rï¿½gles de rï¿½servation adï¿½quates. 35001 = piï¿½ton, 35002 = PMR, 35003 = vï¿½lo. Si la valeur n'ï¿½tait pas fournie lors de la requï¿½te, le champ est renseignï¿½ par la valeur par dï¿½faut 35001 (piï¿½ton).</li>
		///		<li>maxSolutions (optionnel) : nombre maximal de solutions. Si le nombre de solutions n'ï¿½tait pas limitï¿½, alors l'objet n'est pas fourni</li>
		///		<li>sessionID (optionnel) : identificateur de la session fournie si validï¿½e</li></ul>
		///	<li>Plage temporelle du calcul (objet timeBounds) telle que dï¿½finie par les paramï¿½tres fournis ou rï¿½sultant de l'application d'une pï¿½riode ï¿½ une date :</li><ul>
		///		<li>minDepartureHour : heure de dï¿½part minimale demandï¿½e (valeur du paramï¿½tre d'entrï¿½e hi, si non fourni initialement : contient la valeur par dï¿½faut qui a ï¿½tï¿½ affectï¿½e par le systï¿½me) NB : Cette valeur peut ï¿½tre diffï¿½rente de l'heure de dï¿½part de la premiï¿½re solution trouvï¿½e</li>
		///		<li>maxDepatureHour : heure de dï¿½part maximale demandï¿½e (valeur du paramï¿½tre ha, si non fourni initialement : contient la valeur par dï¿½faut qui a ï¿½tï¿½ affectï¿½e par le systï¿½me). NB : Cette valeur peut ï¿½tre diffï¿½rente de l'heure de dï¿½part de la derniï¿½re solution trouvï¿½e</li>
		///		<li>minArrivalHour : heure d'arrivï¿½e minimale demandï¿½e (valeur du paramï¿½tre ii, si non fourni initialement : contient la valeur par dï¿½faut qui a ï¿½tï¿½ affectï¿½e par le systï¿½me) NB : Cette valeur peut ï¿½tre diffï¿½rente de l'heure d'arrivï¿½e de la premiï¿½re solution trouvï¿½e</li>
		///		<li>maxArrivalHour : heure d'arrivï¿½e maximale demandï¿½e (valeur du paramï¿½tre ia, si non fourni initialement : contient la valeur par dï¿½faut qui a ï¿½tï¿½ affectï¿½e par le systï¿½me). NB : Cette valeur peut ï¿½tre diffï¿½rente de l'heure d'arrivï¿½e de la derniï¿½re solution trouvï¿½e</li></ul>
		///	<li>Pï¿½riode de calcul (optionnel : si effectivement utilisï¿½e par la requï¿½te) :</li><ul>
		///		<li>date : date demandï¿½e (valeur du paramï¿½tre da, si non fourni initialement : contient la valeur par dï¿½faut qui a ï¿½tï¿½ affectï¿½e par le systï¿½me)</li>
		///		<li>id : id de la pï¿½riode dans la base de donnï¿½es</li>
		///		<li>name : texte de libellï¿½ de la pï¿½riode</li></ul>
		///	<li>Lieux de dï¿½part et d'arrivï¿½e (objet places) :</li>
		///		<li>departureCity : commune de dï¿½part sous forme de texte</li>
		///		<li>departureCityNameTrust : taux de confiance de l'interprï¿½tation du nom de commune entrï¿½ (1 = correspondance exacte, 0 = champ commune vide)</li>
		///		<li>departureStop (optionnel) : arrï¿½t de dï¿½part sous forme de texte. Si le calcul d'effectue au dï¿½part d'une commune (calcul effectuï¿½ au dï¿½part des lieux principaux de la commune) alors l'objet n'est pas fourni.</li>
		///		<li>departureStopNameTrust : taux de confiance de l'interprï¿½tation du nom de l'arrï¿½t entrï¿½ (1 = correspondance exacte ou champ vide)</li>
		///		<li>arrivalCity : commune d'arrivï¿½e sous forme de texte</li>
		///		<li>arrivalCityNameTrust : taux de confiance de l'interprï¿½tation du nom de commune entrï¿½ (1 = correspondance exacte, 0 = champ commune vide)</li>
		///		<li>arrivalStop (optionnel) : arrï¿½t d'arrivï¿½e sous forme de texte. Si le calcul d'effectue vers une commune (calcul effectuï¿½ ï¿½ l'arrivï¿½e des lieux principaux de la commune) alors l'objet n'est pas fourni.</li>
		///		<li>arrivalStopNameTrust : taux de confiance de l'interprï¿½tation du nom de l'arrï¿½t entrï¿½ (1 = correspondance exacte ou champ vide)</li></ul>
		///	<li>Trajet favori (objet favorite, optionnel : uniquement si fourni dans la requï¿½te) :</li><ul>
		///		<li>id : identificateur du trajet favori demandï¿½</li></ul>
		///	<li>Filtre de mode transports (objet transportModeFilter, optionnel)</li><ul>
		///		<li>id : identificateur du filtre</li>
		///		<li>name : nom du filtre</li></ul></ul>
		///
		/// <h4>Itinï¿½raires</h4>
		///		Les solutions (journeys) sont une composition de solutions (journey). Si aucune solution n'a pu ï¿½tre trouvï¿½e, l'objet est vide. Le diagramme suivant montre la composition de chaque objet solution, dï¿½crite en trois parties :
		///
		/// @image html xml_journeys.png
		///
		/// <ul><li>les attributs, qui donnent des informations de rï¿½sumï¿½ sur le contenu des tronï¿½ons</li>
		///		<ul>
		///			<li>continuousServiceDuration (optionnel) : indique par sa prï¿½sence que la solution est valable en continu pendant une plage horaire. L'attribut contient la durï¿½e de cette plage horaire. Les bornes effectives de la plage horaire sont dï¿½finies par les heures de dï¿½part du premier tronï¿½on de l'itinï¿½raire (chunk). Si cet attribut n'est pas fourni, alors l'itinï¿½raire dï¿½finit une solution unique ï¿½ heure fixe</li>
		///			<li>hasAStopAlert (boolï¿½en) : si vrai, indique qu'au moins un itinï¿½raire passe par au moins un arrï¿½t ayant un message d'alerte valable ï¿½ l'heure de passage de l'itinï¿½raire.</li>
		///			<li>hasALineAlert (boolï¿½en) : si vrai, indique qu'au moins un itinï¿½raire utilise au moins une ligne ayant un message d'alerte valable ï¿½ l'heure d'utilisation</li>
		///		</ul>
		///		<li>l'objet reservation (optionnel) : indique par sa prï¿½sence que l'itinï¿½raire est soumis ï¿½ une modalitï¿½ de rï¿½servation des places, dï¿½finie par une sï¿½rie d'attributs. L'absence d'objet indique que la rï¿½servation est impossible :</li>
		///		<ul>
		///			<li>type : compulsory = rï¿½servation obligatoire des places, optional = rï¿½servation possible des places</li>
		///			<li>deadLine : indique la date et l'heure limite de rï¿½servation. Au-delï¿½ de cette limite, la rï¿½servation devient impossible. Si elle est obligatoire, alors l'itinï¿½raire n'est plus utilisable. Si elle est facultative, alors l'itinï¿½raire reste utilisable mais n'est plus ouvert ï¿½ la rï¿½servation des places</li>
		///			<li>online  (boolï¿½en) : indique si la rï¿½servation des places est possible directement sur le serveur SYNTHESE</li>
		///			<li>phoneNumber (optionnel) : indique le numï¿½ro de tï¿½lï¿½phone de la centrale de rï¿½servation tï¿½lï¿½phonique</li>
		///			<li>openingHours (optionnel) : indique les horaires d'ouverture de la centrale de rï¿½servation tï¿½lï¿½phonique</li>
		///		</ul>
		///		<li>l'objet chunks contient les tronï¿½ons qui dï¿½crivent l'itinï¿½raire dans l'ordre chronologique d'utilisation. Chaque tronï¿½on peut ï¿½tre de trois types diffï¿½rents :</li>
		///		<ul>
		///			<li>street : marche ï¿½ pied le long d'une rue dï¿½signï¿½e</li>
		///			<li>transport : utilisation d'une ligne de transport public</li>
		///			<li>connection : transfert ï¿½ pied d'un arrï¿½t vers un autre diffï¿½rent, sans pour autant fournir le descriptif du trajet ï¿½ emprunter</li>
		///	</ul></ul>
		///
		///	<h4>Tronï¿½on d'itinï¿½raire ï¿½ pied le long d'une rue</h4>
		///		Un section de marche ï¿½ pied le long d'une rue (street) est dï¿½crite comme suit :
		///
		/// @image html routeplanner_result_street.png
		///
		///	<ul><li>departureTime : date/heure de dï¿½part. Si service continu, il s'agit du dï¿½but de la plage horaire ï¿½ l'arrivï¿½e du tronï¿½on</li>
		///		<li>endDepartureTime (optionnel, si service continu uniquement) : fin de la plage horaire de desserte au dï¿½part du tronï¿½on</li>
		///		<li>arrivalTime : date/heure d'arrivï¿½e. Si service continu, il s'agit du dï¿½but de la plage horaire ï¿½ l'arrivï¿½e du tronï¿½on</li>
		///		<li>endArrivalTime (optionnel, si service continu uniquement) : fin  de la plage horaire de desserte ï¿½ l'arrivï¿½e du tronï¿½on</li>
		///		<li>length : longueur du tronï¿½on en mï¿½tres</li>
		///		<li>city : nom de la ville ï¿½ laquelle appartient la rue utilisï¿½e</li>
		///		<li>name : nom de la rue utilisï¿½e</li>
		///		<li>startAddress : objet lieu de dï¿½part du tronï¿½on (voir type PlaceType correspondant)</li>
		///		<li>endAddress : objet lieu d'arrivï¿½e du tronï¿½on (voir type PlaceType correspondant)</li></ul>
		///
		///	<h4>Tronï¿½on d'itinï¿½raire sur ligne de transport public</h4>
		///
		///	Une section utilisant une ligne de transport public (JourneyTransportType) est dï¿½crite comme suit :
		///
		/// @image html routeplanner_result_journey.png
		///
		/// <ul><li>departureTime : date/heure de dï¿½part. Si service continu, il s'agit du dï¿½but de la plage horaire ï¿½ l'arrivï¿½e du tronï¿½on</li>
		///		<li>endDepartureTime (optionnel, si service continu uniquement) : fin de la plage horaire de desserte au dï¿½part du tronï¿½on</li>
		///		<li>arrivalTime : date/heure d'arrivï¿½e. Si service continu, il s'agit du dï¿½but de la plage horaire ï¿½ l'arrivï¿½e du tronï¿½on</li>
		///		<li>endArrivalTime (optionnel, si service continu uniquement) : fin  de la plage horaire de desserte ï¿½ l'arrivï¿½e du tronï¿½on</li>
		///		<li>length : longueur du tronï¿½on en mï¿½tres</li>
		///		<li>possibleWaitingTime (optionnel) : indique par sa prï¿½sence que le temps de parcours inclut une durï¿½e d'attente potentielle, qui peut faire l'objet d'une indication au client</li>
		///		<li>startStopIsTerminus : indique si l'arrï¿½t de dï¿½part du trajet est l'origine de la ligne utilisï¿½e</li>
		///		<li>endStopIsTerminus : indique si l'arrï¿½t d'arrivï¿½e du trajet est la destination de la ligne utilisï¿½e</li>
		///		<li>destinationText : si prï¿½sent, indique la destination affichï¿½e sur le vï¿½hicule ï¿½ emprunter</li>
		///		<li>startStop : arrï¿½t physique de dï¿½part (type PhysicalStopType ï¿½ voir plus bas)</li>
		///		<li>endStop : arrï¿½t physique d'arrivï¿½e (type PhysicalStopType ï¿½ voir plus bas)</li>
		///		<li>destinationStop : arrï¿½t physique de destination du vï¿½hicule ï¿½ emprunter</li>
		///		<li>line : ligne utilisï¿½e, dï¿½crite par plusieurs attributs :</li>
		///		<ul><li>id : identificateur de la ligne dans la base de donnï¿½es de SYNTHESE (pï¿½renne)</li>
		///			<li>color (optionnel) : couleur de la ligne au format RGB HTML (#RRGGBB)</li>
		///			<li>imgURL (optionnel) : URL d'une image reprï¿½sentant le logo de la ligne. Par convention, cette URL peut ï¿½tre partielle pour permettre la mise ï¿½ disposition de plusieurs versions d'images (ex : sncf-grand.png et sncf-petit.png)</li>
		///			<li>cssClass (optionnel) : classe CSS dï¿½finissant le graphisme du numï¿½ro de ligne dans un cartouche (couleur de fond, couleur de texte, style de caractï¿½res, etc.). Correspond en gï¿½nï¿½ral ï¿½ la reprï¿½sentation officielle de la ligne sur les divers supports</li>
		///			<li>shortName (optionnel) : numï¿½ro de ligne</li>
		///			<li>longName (optionnel) : texte dï¿½signant la ligne incluant l'article (ex. : la navette du cimetiï¿½re)</li>
		///			<li>alert (optionnel) : message d'alerte en cours de validitï¿½ sur la ligne au moment du trajet (type AlertType ï¿½ voir plus bas)</li></ul>
		///		<li>vehicleType : mode de transport, dï¿½crit par plusieurs attributs :</li>
		///		<ul><li>id : identificateur du mode de transport dans la base de donnï¿½es SYNTHESE (pï¿½renne)</li>
		///			<li>name : nom du mode de transport incluant l'article (ex : le train)</li>
		///	</ul></ul>
		///
		/// <h4>Arrï¿½ts physiques</h4>
		///
		///	Un arrï¿½t physique (PhysicalStopType) est dï¿½crit comme suit :
		///
		/// @image html routeplanner_result_stop.png
		///
		/// <ul><li>x, y : coordonnï¿½es de l'arrï¿½t selon la projection Lambert II Etendu</li>
		///		<li>latitude, longitude : coordonnï¿½es de l'arrï¿½t selon le systï¿½me WGS84</li>
		///		<li>id : identificateur de l'arrï¿½t dans la base de donnï¿½es de SYNTHESE (pï¿½renne)</li>
		///		<li>name (optionnel) : nom de l'arrï¿½t permettant de le distinguer par rapport aux autres arrï¿½ts de la mï¿½me zone d'arrï¿½ts (ex : quai 12)</li>
		///		<li>connectionPlace : zone d'arrï¿½t (ConnectionPlaceType ï¿½ voir plus loin) ï¿½ laquelle appartient l'arrï¿½t</li>
		///	</ul>
		///
		/// <h4>Tronï¿½on d'itinï¿½raire de jonction ï¿½ pied</h4>
		///
		///	Un trajet ï¿½ pied de liaison entre deux arrï¿½ts sans prï¿½cision concernant les rues ï¿½ emprunter (JourneyConnectionType) est dï¿½crit comme suit :
		///
		/// @image html routeplanner_result_connect.png
		///
		/// <ul><li>departureTime : date/heure de dï¿½part. Si service continu, il s'agit du dï¿½but de la plage horaire ï¿½ l'arrivï¿½e du tronï¿½on</li>
		///		<li>endDepartureTime (optionnel, si service continu uniquement) : fin de la plage horaire de desserte au dï¿½part du tronï¿½on</li>
		///		<li>arrivalTime : date/heure d'arrivï¿½e. Si service continu, il s'agit du dï¿½but de la plage horaire ï¿½ l'arrivï¿½e du tronï¿½on</li>
		///		<li>endArrivalTime (optionnel, si service continu uniquement) : fin  de la plage horaire de desserte ï¿½ l'arrivï¿½e du tronï¿½on</li>
		///		<li>length : longueur du tronï¿½on en mï¿½tres</li>
		///		<li>startStop : arrï¿½t physique de dï¿½part (type PhysicalStopType ï¿½ voir plus haut)</li>
		///		<li>endStop : arrï¿½t physique d'arrivï¿½e (type PhysicalStopType ï¿½ voir plus haut)</li>
		///	</ul>
		///
		/// <h4>Tableau des rï¿½sultats</h4>
		///
		/// Le tableau des rï¿½sultats triï¿½ par zone d'arrï¿½t (type ResultTableType) est dï¿½crit comme suit :
		///
		/// @image html xml_resulttable.png
		///
		/// Chaque rangï¿½e du tableau est reprï¿½sentï¿½e par un objet nommï¿½ row, contenant les ï¿½lï¿½ments suivants (pas d'objet si aucune solution trouvï¿½e) :
		/// <ul><li>type : type de rangï¿½e de tableau (departure (dï¿½part), connection (correspondance), ou arrival (arrivï¿½e))</li>
		///		<li>place : lieu correspondant ï¿½ la ligne du tableau (type PlaceType ï¿½ voir plus bas)</li>
		///		<li>cells : cellules de la rangï¿½e, composï¿½e d'objets cell :</li>
		///		<ul><li>arrivalDateTime : date/heure d'arrivï¿½e, fourni uniquement si la colonne reprï¿½sente une solution qui dessert en arrivï¿½e l'arrï¿½t reprï¿½sentï¿½ par la ligne</li>
		///			<li>endArrivalDateTime : derniï¿½re date/heure d'arrivï¿½e dans un service continu, fourni uniquement si la colonne reprï¿½sente une solution qui dessert en arrivï¿½e l'arrï¿½t reprï¿½sentï¿½ par la ligne, et dans le cas d'un service continu</li>
		///			<li>departureDateTime : date/heure de dï¿½part, fourni uniquement si la colonne reprï¿½sente une solution qui dessert en dï¿½part l'arrï¿½t reprï¿½sentï¿½ par la ligne</li>
		///			<li>endDepartureDateTime : derniï¿½re date/heure de dï¿½part dans un service continu, fourni uniquement si la colonne reprï¿½sente une solution qui dessert en dï¿½part l'arrï¿½t reprï¿½sentï¿½ par la ligne, et dans le cas d'un service continu</li>
		///			<li>pedestrian : si prï¿½sent, indique que la cellule est concernï¿½e par un trajet piï¿½ton, qui peut alors ï¿½tre affichï¿½ diffï¿½remment. Les valeurs possibles de pedestrian sont :
		///			<ul><li>departure : indique qu'un trajet piï¿½ton dï¿½marre ï¿½ la cellule. Un pictogramme peut remplacer l'heure de dï¿½part si souhaitï¿½.</li>
		///				<li>arrival : indique qu'un trajet piï¿½ton se termine sur la cellule. Un pictograme peut remplacer l'heure d'arrivï¿½e si souhaitï¿½.</li>
		///				<li>traversal : indique qu'un trajet piï¿½ton a dï¿½marrï¿½ avant cette cellule et se terminera aprï¿½s cette cellule (dans le sens de lecture de la colonne). Un pictogramme peut l'indiquer si souhaitï¿½.</li>
		///			</ul>
		/// </ul></ul>
		///
		/// <h4>Lieux (gï¿½nï¿½rique)</h4>
		///
		///	Les lieux (PlaceType) peuvent ï¿½tre de trois types :
		/// @image html xml_place.png
		///
		/// <ul><li>connectionPlace : Zones d'arrï¿½t (ConnectionPlaceType), dï¿½crites comme suit :</li>
		/// @image html routeplanner_result_stopare.png
		///
		///		<ul><li>x, y : coordonnï¿½es de la zone d'arrï¿½t selon la projection Lambert II Etendu</li>
		///			<li>latitude, longitude : coordonnï¿½es de la zone d'arrï¿½t selon le systï¿½me WGS84</li>
		///			<li>id : identificateur de la zone d'arrï¿½t dans la base de donnï¿½es de SYNTHESE (pï¿½renne)</li>
		///			<li>city : nom de la ville ï¿½ laquelle appartient la zone d'arrï¿½t</li>
		///			<li>name : nom de la zone d'arrï¿½t dans la ville</li>
		///			<li>alert : message d'alerte valable dans au moins une des solutions utilisant la zone d'arrï¿½t (type AlertType ï¿½ voir plus bas)</li>
		///		</ul>
		///		<li>publicPlace : Lieux publics (PublicPlaceType) dï¿½crits comme suit :</li>
		/// @image html routeplanner_result_public.png
		///		<ul><li>x, y : coordonnï¿½es du lieu public selon la projection Lambert II Etendu</li>
		///			<li>latitude, longitude : coordonnï¿½es du lieu public selon le systï¿½me WGS84</li>
		///			<li>id : identificateur du lieu public dans la base de donnï¿½es de SYNTHESE (pï¿½renne)</li>
		///			<li>city : nom de la ville ï¿½ laquelle appartient le lieu public</li>
		///			<li>name : nom du lieu public dans la ville</li>
		///		</ul>
		///		<li>address : Adresse (AddressType), dï¿½crites comme suit :</li>
		///	@image html xml_address.png
		///		<ul><li>x, y : coordonnï¿½es du lieu public selon la projection Lambert II Etendu</li>
		///			<li>latitude, longitude : coordonnï¿½es du lieu public selon le systï¿½me WGS84</li>
		///			<li>id : identificateur du lieu public dans la base de donnï¿½es de SYNTHESE (pï¿½renne)</li>
		///			<li>city : nom de la ville ï¿½ laquelle appartient le lieu public</li>
		///			<li>number : numï¿½ro de l'habitation. Optionnel : si non prï¿½sent, l'objet reprï¿½sente la rue entiï¿½re</li>
		///			<li>streetName : nom de la rue dans la ville</li>
		///	</ul></ul>
		///
		/// <h4>Message d'alerte</h4>
		///
		/// Un message d'alerte (AlertType) se dï¿½finit comme suit :
		/// @image html xml_alert.png
		///
		/// <ul><li>contenu : texte du message d'alerte</li>
		///		<li>id : identificateur de l'alerte dans la base de donnï¿½es SYNTHESE (pï¿½renne)</li>
		///		<li>level : niveau d'alerte (info = information, warning = attention, interruption = interruption de service)</li>
		///		<li>startValidity : date/heure de dï¿½but d'application du message d'alerte</li>
		///		<li>endValidity : date/heure de fin d'application du message d'alerte</li>
		///	</ul>
		///
		/// <h3>Attachments</h3>
		///
		///	<ul>
		///	<li><a href="include/53_pt_routeplanner/routeplanner_result.xsd">Response XSD Schema</a></li>
		///	<li><a href="include/53_pt_routeplanner/routeplanner_resultSample.xml">Example of XML response</a></li>
		///	</ul>
		class RoutePlannerFunction:
			public util::FactorableTemplate<cms::FunctionWithSite<true>,RoutePlannerFunction>
		{
		public:
			static const std::string PARAMETER_SITE;
			static const std::string PARAMETER_MAX_SOLUTIONS_NUMBER;
			static const std::string PARAMETER_MAX_DEPTH;
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
			static const std::string PARAMETER_ROLLING_STOCK_FILTER_ID;
			static const std::string PARAMETER_PAGE;
			static const std::string PARAMETER_SCHEDULES_ROW_PAGE;
			static const std::string PARAMETER_SCHEDULES_CELL_PAGE;
			static const std::string PARAMETER_LINES_ROW_PAGE;
			static const std::string PARAMETER_LINE_MARKER_PAGE;
			static const std::string PARAMETER_BOARD_PAGE;
			static const std::string PARAMETER_WARNING_PAGE;
			static const std::string PARAMETER_RESERVATION_PAGE;
			static const std::string PARAMETER_DURATION_PAGE;
			static const std::string PARAMETER_TEXT_DURATION_PAGE;
			static const std::string PARAMETER_MAP_PAGE;
			static const std::string PARAMETER_MAP_LINE_PAGE;
			static const std::string PARAMETER_DATE_TIME_PAGE;
			static const std::string PARAMETER_STOP_CELL_PAGE;
			static const std::string PARAMETER_SERVICE_CELL_PAGE;
			static const std::string PARAMETER_JUNCTION_CELL_PAGE;
			static const std::string PARAMETER_MAP_STOP_PAGE;
			static const std::string PARAMETER_MAP_SERVICE_PAGE;
			static const std::string PARAMETER_MAP_JUNCTION_PAGE;
			static const std::string PARAMETER_SHOW_RESULT_TABLE;
			static const std::string PARAMETER_SHOW_COORDINATES;
			
		private:
			//! \name Parameters
			//@{
				road::RoadModule::ExtendedFetchPlaceResult	_departure_place;
				road::RoadModule::ExtendedFetchPlaceResult	_arrival_place;
				std::string									_originCityText;
				std::string									_destinationCityText;
				std::string									_originPlaceText;
				std::string									_destinationPlaceText;
				boost::posix_time::ptime					_startDate;
				boost::posix_time::ptime					_endDate;
				boost::posix_time::ptime					_startArrivalDate;
				boost::posix_time::ptime					_endArrivalDate;
				algorithm::PlanningOrder					_planningOrder;
				graph::AccessParameters						_accessParameters;
				boost::optional<std::size_t>				_maxSolutionsNumber;
				std::size_t									_periodId;
				const pt_website::HourPeriod*			_period;
				bool										_home;
				boost::shared_ptr<const UserFavoriteJourney>		_favorite;
				boost::shared_ptr<const pt_website::RollingStockFilter>	_rollingStockFilter;
				bool										_outputRoadApproachDetail;
				bool _showResTab;
				bool _showCoords;
			//@}

			//! @name Pages
			//@{
				boost::shared_ptr<const cms::Webpage> _page;
				boost::shared_ptr<const cms::Webpage> _schedulesRowPage;
				boost::shared_ptr<const cms::Webpage> _schedulesCellPage;
				boost::shared_ptr<const cms::Webpage> _linesRowPage;
				boost::shared_ptr<const cms::Webpage> _lineMarkerPage;
				boost::shared_ptr<const cms::Webpage> _boardPage;
				boost::shared_ptr<const cms::Webpage> _warningPage;
				boost::shared_ptr<const cms::Webpage> _reservationPage;
				boost::shared_ptr<const cms::Webpage> _durationPage;
				boost::shared_ptr<const cms::Webpage> _textDurationPage;
				boost::shared_ptr<const cms::Webpage> _mapPage;
				boost::shared_ptr<const cms::Webpage> _mapLinePage;
				boost::shared_ptr<const cms::Webpage> _dateTimePage;
				boost::shared_ptr<const cms::Webpage> _stopCellPage;
				boost::shared_ptr<const cms::Webpage> _serviceCellPage;
				boost::shared_ptr<const cms::Webpage> _junctionPage;
				boost::shared_ptr<const cms::Webpage> _mapStopCellPage;
				boost::shared_ptr<const cms::Webpage> _mapServiceCellPage;
				boost::shared_ptr<const cms::Webpage> _mapJunctionPage;
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
				const geography::NamedPlace& place,
				bool showCoords
			);
			static void _XMLDisplayPhysicalStop(
				std::ostream& stream,
				const std::string& tag,
				const pt::StopPoint& place,
				bool showCoords
			);
			static void _XMLDisplayAddress(
				std::ostream& stream,
				const road::Crossing& place,
				const road::RoadPlace& roadPlace,
				bool showCoords
			);
			static void _XMLDisplayRoadPlace(
				std::ostream& stream,
				const road::RoadPlace& roadPlace,
				bool showCoords
			);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(std::ostream& stream, const server::Request& request) const;

			const boost::optional<std::size_t>& getMaxSolutions() const;
			void setMaxSolutions(boost::optional<std::size_t> number);

			RoutePlannerFunction();

			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_RoutePlannerFunction_H__
