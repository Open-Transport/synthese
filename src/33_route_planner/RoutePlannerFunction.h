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
#include "Site.h"
#include "RoutePlannerTypes.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace road
	{
		class Address;
		class RoadPlace;
	}

	namespace transportwebsite
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
		class PhysicalStop;
	}

	namespace pt
	{
		class PublicTransportStopZoneConnectionPlace;
	}

	namespace routeplanner
	{
		class RoutePlannerInterfacePage;
		class UserFavoriteJourney;

		////////////////////////////////////////////////////////////////////
		/// 53.15 Function : public transportation route planner.
		///	@ingroup m53Functions refFunctions
		/// @author Hugues Romain
		///
		/// <h3>Requête</h3>
		/// <h4>Base parameters</h4>
		///
		/// <ul>
		///		<li>fonction=<b>rp</b></li>
		///		<li>si=<site id> : ID Site</li>
		///		<li>[sid=<session id>] : ID de la session ouverte, issu du retour de la fonction de connexion
		///		  d'utilisateur. Nécessaire uniquement pour faire appel au trajet favori de l'utilisateur (voir
		///		  plus loin)</li>
		/// </ul>
		///
		///	<h4>Paramètres de temps</h4>
		/// <ul>
		///		<li>Choix 1a : désignation de la plage de calcul par jour et période :</li>
		///		<ul><li>dy=YYYY-MM-DD : date du calcul. YYYY=année, MM=mois, DD=jour. Si non fournie, la fiche
		///			  horaire est calculée pour le jour même.</li>
		///			<li>pi=<id période> : identificateur de la période dans la base de données. Doit correspondre à
		///			  une période appartenant au site</li></ul>
		///		<li>Choix 1b : désignation de la plage de calcul par bornage :</li>
		///		<ul><li>[da=YYYY-MM-DD HH :MM] : heure de début de la fiche horaire (premier départ).
		///			  HH=heures, MM=minutes. Si non fournie et si heure limite d'arrivée non fournie, la fiche
		///			  horaire commence à la date et l'heure courante. Si non fournie et si heure limite d'arrivée
		///			  fournie, prend la valeur de l'heure limite d'arrivée diminuée de 24 heures et de deux
		///			  minutes par kilomètres à effectuer à vol d'oiseau</li>
		///			<li>[ha=YYYY-MM-DD HH :MM] : heure limite de départ (fin de la fiche horaire). HH=heures,
		///			  MM=minutes. Si non fournie, la fiche horaire stoppe 24 heures après l'heure de début de la
		///			  fiche horaire. Si l'heure maximale est inférieure à l'heure minimale, alors elle est considérée
		///			  comme correspondant au lendemain matin</li>
		///			<li>[ia=YYYY-MM-DD HH :MM] : heure limite d'arrivée (fin de la fiche horaire). HH=heures,
		///			  MM=minutes. Si non fournie, l'heure limite d'arrivée est égale à l'heure limite de départ. Si
		///			  l'heure maximale est inférieure à l'heure minimale, alors elle est considérée comme
		///			  correspondant au lendemain matin</li>
		///			<li>[ii=YYYY-MM-DD HH :MM] : heure de début de la fiche horaire (première arrivée).
		///			  HH=heures, MM=minutes. Si non fournie, la fiche horaire commence à l'heure limite
		///			  d'arrivée diminuée de 24 heures, ou l'heure courante si la date correspond au jour même.</li></ul></ul>
		/// <h4>Parameters de lieux</h4>
		///		<ul><li>Choix 2a : désignation des lieux de départ et d'arrivée par texte :</li>
		///			<ul><li>dct=<commune de départ> : commune de départ sous forme de texte</li>
		///			<li>[dpt=<arrêt de départ>] : arrêt de départ sous forme de texte. Si non fourni, les arrêts
		///			  principaux de la commune sont choisis.</li>
		///			<li>act=<commune d'arrivée> : commune d'arrivée sous forme de texte</li>
		///			<li>[apt=<arrêt d'arrivée>] : arrêt d'arrivée sous forme de texte. Si non fourni, les arrêts
		///			  principaux de la commune sont choisis.</li></ul>
		///		<li>Choix 2b : désignation des lieux de départ et d'arrivée par pré-enregistrement</li>
		///			<ul><li>fid=<id trajet favori> : identificateur de trajet favori de l'utilisateur connecté
		///			  Paramètres</li></ul></ul>
		/// <h4>Paramètres de calcul</h4>
		///		<ul><li>[msn=<nombre de solutions maximum>] : si non fourni, le nombre de solutions calculées
		///		  est illimité. msn doit être un nombre strictement supérieur à 0.</li>
		///		<li>[ac=35001|35002|35003] : type d'utilisateur pour filtrer sur les règles d'accessibilité et
		///		  appliquer les règles de réservation adéquates. 35001 = piéton, 35002 = PMR, 35003 = vélo.
		///		  Si non fourni, le calcul s'effectue pour un utilisateur piéton.</li>
		///		<li>[tm=<id filtre mode de transport>] : identificateur de filtre</li></ul>
		///
		/// <h3>Response</h3>
		///
		///		<ul>
		///		<li>if the site links to an interface containing a RoutePlannerInterfacePage object, the output is generated
		///		  by the interface</li>
		///		<li>else a XML output is generated</li></ul>
		///
		/// <h4>Racine</h4>
		///
		///	La réponse se sépare en trois parties :
		///
		/// @image html routeplanner_result_main.png
		///
		///	<ul><li>query : un rappel de la requête, permettant de remplir à nouveau un formulaire de re-interrogation du serveur</li>
		/// <li>journeys : un descriptif de chaque solution, permettant de construire les feuilles de route détaillées</li>
		/// <li>resultTable : une présentation des heures de passage par arrêt, organisées pour être affichées sous forme de tableau synthétique</li></ul>
		///	
		/// <h4>Rappel de la requête</h4>
		///
		///	La requête (query) est définie comme suit :
		/// 
		/// @image html xml_query.png
		///
		/// <ul><li>Attributs :</li><ul>
		///		<li>siteId : identificateur du site demandé</li>
		///		<li>userProfile : type d'utilisateur pour filtrer sur les règles d'accessibilité et appliquer les règles de réservation adéquates. 35001 = piéton, 35002 = PMR, 35003 = vélo. Si la valeur n'était pas fournie lors de la requête, le champ est renseigné par la valeur par défaut 35001 (piéton).</li>
		///		<li>maxSolutions (optionnel) : nombre maximal de solutions. Si le nombre de solutions n'était pas limité, alors l'objet n'est pas fourni</li>
		///		<li>sessionID (optionnel) : identificateur de la session fournie si validée</li></ul>
		///	<li>Plage temporelle du calcul (objet timeBounds) telle que définie par les paramètres fournis ou résultant de l'application d'une période à une date :</li><ul>
		///		<li>minDepartureHour : heure de départ minimale demandée (valeur du paramètre d'entrée hi, si non fourni initialement : contient la valeur par défaut qui a été affectée par le système) NB : Cette valeur peut être différente de l'heure de départ de la première solution trouvée</li>
		///		<li>maxDepatureHour : heure de départ maximale demandée (valeur du paramètre ha, si non fourni initialement : contient la valeur par défaut qui a été affectée par le système). NB : Cette valeur peut être différente de l'heure de départ de la dernière solution trouvée</li>
		///		<li>minArrivalHour : heure d'arrivée minimale demandée (valeur du paramètre ii, si non fourni initialement : contient la valeur par défaut qui a été affectée par le système) NB : Cette valeur peut être différente de l'heure d'arrivée de la première solution trouvée</li>
		///		<li>maxArrivalHour : heure d'arrivée maximale demandée (valeur du paramètre ia, si non fourni initialement : contient la valeur par défaut qui a été affectée par le système). NB : Cette valeur peut être différente de l'heure d'arrivée de la dernière solution trouvée</li></ul>
		///	<li>Période de calcul (optionnel : si effectivement utilisée par la requête) :</li><ul>
		///		<li>date : date demandée (valeur du paramètre da, si non fourni initialement : contient la valeur par défaut qui a été affectée par le système)</li>
		///		<li>id : id de la période dans la base de données</li>
		///		<li>name : texte de libellé de la période</li></ul>
		///	<li>Lieux de départ et d'arrivée (objet places) :</li>
		///		<li>departureCity : commune de départ sous forme de texte</li>
		///		<li>departureCityNameTrust : taux de confiance de l'interprétation du nom de commune entré (1 = correspondance exacte, 0 = champ commune vide)</li>
		///		<li>departureStop (optionnel) : arrêt de départ sous forme de texte. Si le calcul d'effectue au départ d'une commune (calcul effectué au départ des lieux principaux de la commune) alors l'objet n'est pas fourni.</li>
		///		<li>departureStopNameTrust : taux de confiance de l'interprétation du nom de l'arrêt entré (1 = correspondance exacte ou champ vide)</li>
		///		<li>arrivalCity : commune d'arrivée sous forme de texte</li>
		///		<li>arrivalCityNameTrust : taux de confiance de l'interprétation du nom de commune entré (1 = correspondance exacte, 0 = champ commune vide)</li>
		///		<li>arrivalStop (optionnel) : arrêt d'arrivée sous forme de texte. Si le calcul d'effectue vers une commune (calcul effectué à l'arrivée des lieux principaux de la commune) alors l'objet n'est pas fourni.</li>
		///		<li>arrivalStopNameTrust : taux de confiance de l'interprétation du nom de l'arrêt entré (1 = correspondance exacte ou champ vide)</li></ul>
		///	<li>Trajet favori (objet favorite, optionnel : uniquement si fourni dans la requête) :</li><ul>
		///		<li>id : identificateur du trajet favori demandé</li></ul>
		///	<li>Filtre de mode transports (objet transportModeFilter, optionnel)</li><ul>
		///		<li>id : identificateur du filtre</li>
		///		<li>name : nom du filtre</li></ul></ul>
		///
		/// <h4>Itinéraires</h4>
		///		Les solutions (journeys) sont une composition de solutions (journey). Si aucune solution n'a pu être trouvée, l'objet est vide. Le diagramme suivant montre la composition de chaque objet solution, décrite en trois parties :
		///
		/// @image html xml_journeys.png
		///
		/// <ul><li>les attributs, qui donnent des informations de résumé sur le contenu des tronçons</li>
		///		<ul>
		///			<li>continuousServiceDuration (optionnel) : indique par sa présence que la solution est valable en continu pendant une plage horaire. L'attribut contient la durée de cette plage horaire. Les bornes effectives de la plage horaire sont définies par les heures de départ du premier tronçon de l'itinéraire (chunk). Si cet attribut n'est pas fourni, alors l'itinéraire définit une solution unique à heure fixe</li>
		///			<li>hasAStopAlert (booléen) : si vrai, indique qu'au moins un itinéraire passe par au moins un arrêt ayant un message d'alerte valable à l'heure de passage de l'itinéraire.</li>
		///			<li>hasALineAlert (booléen) : si vrai, indique qu'au moins un itinéraire utilise au moins une ligne ayant un message d'alerte valable à l'heure d'utilisation</li>
		///		</ul>
		///		<li>l'objet reservation (optionnel) : indique par sa présence que l'itinéraire est soumis à une modalité de réservation des places, définie par une série d'attributs. L'absence d'objet indique que la réservation est impossible :</li>
		///		<ul>
		///			<li>type : compulsory = réservation obligatoire des places, optional = réservation possible des places</li>
		///			<li>deadLine : indique la date et l'heure limite de réservation. Au-delà de cette limite, la réservation devient impossible. Si elle est obligatoire, alors l'itinéraire n'est plus utilisable. Si elle est facultative, alors l'itinéraire reste utilisable mais n'est plus ouvert à la réservation des places</li>
		///			<li>online  (booléen) : indique si la réservation des places est possible directement sur le serveur SYNTHESE</li>
		///			<li>phoneNumber (optionnel) : indique le numéro de téléphone de la centrale de réservation téléphonique</li>
		///			<li>openingHours (optionnel) : indique les horaires d'ouverture de la centrale de réservation téléphonique</li>
		///		</ul>
		///		<li>l'objet chunks contient les tronçons qui décrivent l'itinéraire dans l'ordre chronologique d'utilisation. Chaque tronçon peut être de trois types différents :</li>
		///		<ul>
		///			<li>street : marche à pied le long d'une rue désignée</li>
		///			<li>transport : utilisation d'une ligne de transport public</li>
		///			<li>connection : transfert à pied d'un arrêt vers un autre différent, sans pour autant fournir le descriptif du trajet à emprunter</li>
		///	</ul></ul>
		///
		///	<h4>Tronçon d'itinéraire à pied le long d'une rue</h4>
		///		Un section de marche à pied le long d'une rue (street) est décrite comme suit :
		///
		/// @image html routeplanner_result_street.png
		///
		///	<ul><li>departureTime : date/heure de départ. Si service continu, il s'agit du début de la plage horaire à l'arrivée du tronçon</li>
		///		<li>endDepartureTime (optionnel, si service continu uniquement) : fin de la plage horaire de desserte au départ du tronçon</li>
		///		<li>arrivalTime : date/heure d'arrivée. Si service continu, il s'agit du début de la plage horaire à l'arrivée du tronçon</li>
		///		<li>endArrivalTime (optionnel, si service continu uniquement) : fin  de la plage horaire de desserte à l'arrivée du tronçon</li>
		///		<li>length : longueur du tronçon en mètres</li>
		///		<li>city : nom de la ville à laquelle appartient la rue utilisée</li>
		///		<li>name : nom de la rue utilisée</li>
		///		<li>startAddress : objet lieu de départ du tronçon (voir type PlaceType correspondant)</li>
		///		<li>endAddress : objet lieu d'arrivée du tronçon (voir type PlaceType correspondant)</li></ul>
		///
		///	<h4>Tronçon d'itinéraire sur ligne de transport public</h4>
		///
		///	Une section utilisant une ligne de transport public (JourneyTransportType) est décrite comme suit :
		///
		/// @image html routeplanner_result_journey.png
		///
		/// <ul><li>departureTime : date/heure de départ. Si service continu, il s'agit du début de la plage horaire à l'arrivée du tronçon</li>
		///		<li>endDepartureTime (optionnel, si service continu uniquement) : fin de la plage horaire de desserte au départ du tronçon</li>
		///		<li>arrivalTime : date/heure d'arrivée. Si service continu, il s'agit du début de la plage horaire à l'arrivée du tronçon</li>
		///		<li>endArrivalTime (optionnel, si service continu uniquement) : fin  de la plage horaire de desserte à l'arrivée du tronçon</li>
		///		<li>length : longueur du tronçon en mètres</li>
		///		<li>possibleWaitingTime (optionnel) : indique par sa présence que le temps de parcours inclut une durée d'attente potentielle, qui peut faire l'objet d'une indication au client</li>
		///		<li>startStopIsTerminus : indique si l'arrêt de départ du trajet est l'origine de la ligne utilisée</li>
		///		<li>endStopIsTerminus : indique si l'arrêt d'arrivée du trajet est la destination de la ligne utilisée</li>
		///		<li>destinationText : si présent, indique la destination affichée sur le véhicule à emprunter</li>
		///		<li>startStop : arrêt physique de départ (type PhysicalStopType – voir plus bas)</li>
		///		<li>endStop : arrêt physique d'arrivée (type PhysicalStopType – voir plus bas)</li>
		///		<li>destinationStop : arrêt physique de destination du véhicule à emprunter</li>
		///		<li>line : ligne utilisée, décrite par plusieurs attributs :</li>
		///		<ul><li>id : identificateur de la ligne dans la base de données de SYNTHESE (pérenne)</li>
		///			<li>color (optionnel) : couleur de la ligne au format RGB HTML (#RRGGBB)</li>
		///			<li>imgURL (optionnel) : URL d'une image représentant le logo de la ligne. Par convention, cette URL peut être partielle pour permettre la mise à disposition de plusieurs versions d'images (ex : sncf-grand.png et sncf-petit.png)</li>
		///			<li>cssClass (optionnel) : classe CSS définissant le graphisme du numéro de ligne dans un cartouche (couleur de fond, couleur de texte, style de caractères, etc.). Correspond en général à la représentation officielle de la ligne sur les divers supports</li>
		///			<li>shortName (optionnel) : numéro de ligne</li>
		///			<li>longName (optionnel) : texte désignant la ligne incluant l'article (ex. : la navette du cimetière)</li>
		///			<li>alert (optionnel) : message d'alerte en cours de validité sur la ligne au moment du trajet (type AlertType – voir plus bas)</li></ul>
		///		<li>vehicleType : mode de transport, décrit par plusieurs attributs :</li>
		///		<ul><li>id : identificateur du mode de transport dans la base de données SYNTHESE (pérenne)</li>
		///			<li>name : nom du mode de transport incluant l'article (ex : le train)</li>
		///	</ul></ul>
		///
		/// <h4>Arrêts physiques</h4>
		///
		///	Un arrêt physique (PhysicalStopType) est décrit comme suit :
		///
		/// @image html routeplanner_result_stop.png
		///
		/// <ul><li>x, y : coordonnées de l'arrêt selon la projection Lambert II Etendu</li>
		///		<li>latitude, longitude : coordonnées de l'arrêt selon le système WGS84</li>
		///		<li>id : identificateur de l'arrêt dans la base de données de SYNTHESE (pérenne)</li>
		///		<li>name (optionnel) : nom de l'arrêt permettant de le distinguer par rapport aux autres arrêts de la même zone d'arrêts (ex : quai 12)</li>
		///		<li>connectionPlace : zone d'arrêt (ConnectionPlaceType – voir plus loin) à laquelle appartient l'arrêt</li>
		///	</ul>
		///
		/// <h4>Tronçon d'itinéraire de jonction à pied</h4>
		///
		///	Un trajet à pied de liaison entre deux arrêts sans précision concernant les rues à emprunter (JourneyConnectionType) est décrit comme suit :
		///
		/// @image html routeplanner_result_connect.png
		///
		/// <ul><li>departureTime : date/heure de départ. Si service continu, il s'agit du début de la plage horaire à l'arrivée du tronçon</li>
		///		<li>endDepartureTime (optionnel, si service continu uniquement) : fin de la plage horaire de desserte au départ du tronçon</li>
		///		<li>arrivalTime : date/heure d'arrivée. Si service continu, il s'agit du début de la plage horaire à l'arrivée du tronçon</li>
		///		<li>endArrivalTime (optionnel, si service continu uniquement) : fin  de la plage horaire de desserte à l'arrivée du tronçon</li>
		///		<li>length : longueur du tronçon en mètres</li>
		///		<li>startStop : arrêt physique de départ (type PhysicalStopType – voir plus haut)</li>
		///		<li>endStop : arrêt physique d'arrivée (type PhysicalStopType – voir plus haut)</li>
		///	</ul>
		///
		/// <h4>Tableau des résultats</h4>
		///
		/// Le tableau des résultats trié par zone d'arrêt (type ResultTableType) est décrit comme suit :
		///
		/// @image html xml_resulttable.png
		///
		/// Chaque rangée du tableau est représentée par un objet nommé row, contenant les éléments suivants (pas d'objet si aucune solution trouvée) :
		/// <ul><li>type : type de rangée de tableau (departure (départ), connection (correspondance), ou arrival (arrivée))</li>
		///		<li>place : lieu correspondant à la ligne du tableau (type PlaceType – voir plus bas)</li>
		///		<li>cells : cellules de la rangée, composée d'objets cell :</li>
		///		<ul><li>arrivalDateTime : date/heure d'arrivée, fourni uniquement si la colonne représente une solution qui dessert en arrivée l'arrêt représenté par la ligne</li>
		///			<li>endArrivalDateTime : dernière date/heure d'arrivée dans un service continu, fourni uniquement si la colonne représente une solution qui dessert en arrivée l'arrêt représenté par la ligne, et dans le cas d'un service continu</li>
		///			<li>departureDateTime : date/heure de départ, fourni uniquement si la colonne représente une solution qui dessert en départ l'arrêt représenté par la ligne</li>
		///			<li>endDepartureDateTime : dernière date/heure de départ dans un service continu, fourni uniquement si la colonne représente une solution qui dessert en départ l'arrêt représenté par la ligne, et dans le cas d'un service continu</li>
		/// </ul></ul>
		///
		/// <h4>Lieux (générique)</h4>
		///
		///	Les lieux (PlaceType) peuvent être de trois types :
		/// @image html xml_place.png
		///
		/// <ul><li>connectionPlace : Zones d'arrêt (ConnectionPlaceType), décrites comme suit :</li>
		/// @image html routeplanner_result_stopare.png
		///
		///		<ul><li>x, y : coordonnées de la zone d'arrêt selon la projection Lambert II Etendu</li>
		///			<li>latitude, longitude : coordonnées de la zone d'arrêt selon le système WGS84</li>
		///			<li>id : identificateur de la zone d'arrêt dans la base de données de SYNTHESE (pérenne)</li>
		///			<li>city : nom de la ville à laquelle appartient la zone d'arrêt</li>
		///			<li>name : nom de la zone d'arrêt dans la ville</li>
		///			<li>alert : message d'alerte valable dans au moins une des solutions utilisant la zone d'arrêt (type AlertType – voir plus bas)</li>
		///		</ul>
		///		<li>publicPlace : Lieux publics (PublicPlaceType) décrits comme suit :</li>
		/// @image html routeplanner_result_public.png
		///		<ul><li>x, y : coordonnées du lieu public selon la projection Lambert II Etendu</li>
		///			<li>latitude, longitude : coordonnées du lieu public selon le système WGS84</li>
		///			<li>id : identificateur du lieu public dans la base de données de SYNTHESE (pérenne)</li>
		///			<li>city : nom de la ville à laquelle appartient le lieu public</li>
		///			<li>name : nom du lieu public dans la ville</li>
		///		</ul>
		///		<li>address : Adresse (AddressType), décrites comme suit :</li>
		///	@image html xml_address.png
		///		<ul><li>x, y : coordonnées du lieu public selon la projection Lambert II Etendu</li>
		///			<li>latitude, longitude : coordonnées du lieu public selon le système WGS84</li>
		///			<li>id : identificateur du lieu public dans la base de données de SYNTHESE (pérenne)</li>
		///			<li>city : nom de la ville à laquelle appartient le lieu public</li>
		///			<li>number : numéro de l'habitation. Optionnel : si non présent, l'objet représente la rue entière</li>
		///			<li>streetName : nom de la rue dans la ville</li>
		///	</ul></ul>
		///
		/// <h4>Message d'alerte</h4>
		///
		/// Un message d'alerte (AlertType) se définit comme suit :
		/// @image html xml_alert.png
		///
		/// <ul><li>contenu : texte du message d'alerte</li>
		///		<li>id : identificateur de l'alerte dans la base de données SYNTHESE (pérenne)</li>
		///		<li>level : niveau d'alerte (info = information, warning = attention, interruption = interruption de service)</li>
		///		<li>startValidity : date/heure de début d'application du message d'alerte</li>
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
			public util::FactorableTemplate<transportwebsite::FunctionWithSite<true>,RoutePlannerFunction>
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
			static const std::string PARAMETER_ROLLING_STOCK_FILTER_ID;
			
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
				boost::posix_time::ptime					_startDate;
				boost::posix_time::ptime					_endDate;
				boost::posix_time::ptime					_startArrivalDate;
				boost::posix_time::ptime					_endArrivalDate;
				algorithm::PlanningOrder					_planningOrder;
				graph::AccessParameters						_accessParameters;
				boost::optional<std::size_t>				_maxSolutionsNumber;
				std::size_t									_periodId;
				const transportwebsite::HourPeriod*			_period;
				bool										_home;
				boost::shared_ptr<const UserFavoriteJourney>		_favorite;
				boost::shared_ptr<const transportwebsite::RollingStockFilter>	_rollingStockFilter;
				bool										_outputRoadApproachDetail;
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
				const pt::PhysicalStop& place
			);
			static void _XMLDisplayAddress(
				std::ostream& stream,
				const road::Address& place,
				const road::RoadPlace& roadPlace
			);
			static void _XMLDisplayRoadPlace(
				std::ostream& stream,
				const road::RoadPlace& roadPlace
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
