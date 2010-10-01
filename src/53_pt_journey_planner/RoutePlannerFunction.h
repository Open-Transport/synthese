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
		/// <h3>Requ�te</h3>
		/// <h4>Base parameters</h4>
		///
		/// <ul>
		///		<li>fonction=<b>rp</b></li>
		///		<li>si=<site id> : ID Site</li>
		///		<li>[sid=<session id>] : ID de la session ouverte, issu du retour de la fonction de connexion
		///		  d'utilisateur. N�cessaire uniquement pour faire appel au trajet favori de l'utilisateur (voir
		///		  plus loin)</li>
		/// </ul>
		///
		///	<h4>Param�tres de temps</h4>
		/// <ul>
		///		<li>Choix 1a : d�signation de la plage de calcul par jour et p�riode :</li>
		///		<ul><li>dy=YYYY-MM-DD : date du calcul. YYYY=ann�e, MM=mois, DD=jour. Si non fournie, la fiche
		///			  horaire est calcul�e pour le jour m�me.</li>
		///			<li>pi=<id p�riode> : identificateur de la p�riode dans la base de donn�es. Doit correspondre �
		///			  une p�riode appartenant au site</li></ul>
		///		<li>Choix 1b : d�signation de la plage de calcul par bornage :</li>
		///		<ul><li>[da=YYYY-MM-DD HH :MM] : heure de d�but de la fiche horaire (premier d�part).
		///			  HH=heures, MM=minutes. Si non fournie et si heure limite d'arriv�e non fournie, la fiche
		///			  horaire commence � la date et l'heure courante. Si non fournie et si heure limite d'arriv�e
		///			  fournie, prend la valeur de l'heure limite d'arriv�e diminu�e de 24 heures et de deux
		///			  minutes par kilom�tres � effectuer � vol d'oiseau</li>
		///			<li>[ha=YYYY-MM-DD HH :MM] : heure limite de d�part (fin de la fiche horaire). HH=heures,
		///			  MM=minutes. Si non fournie, la fiche horaire stoppe 24 heures apr�s l'heure de d�but de la
		///			  fiche horaire. Si l'heure maximale est inf�rieure � l'heure minimale, alors elle est consid�r�e
		///			  comme correspondant au lendemain matin</li>
		///			<li>[ia=YYYY-MM-DD HH :MM] : heure limite d'arriv�e (fin de la fiche horaire). HH=heures,
		///			  MM=minutes. Si non fournie, l'heure limite d'arriv�e est �gale � l'heure limite de d�part. Si
		///			  l'heure maximale est inf�rieure � l'heure minimale, alors elle est consid�r�e comme
		///			  correspondant au lendemain matin</li>
		///			<li>[ii=YYYY-MM-DD HH :MM] : heure de d�but de la fiche horaire (premi�re arriv�e).
		///			  HH=heures, MM=minutes. Si non fournie, la fiche horaire commence � l'heure limite
		///			  d'arriv�e diminu�e de 24 heures, ou l'heure courante si la date correspond au jour m�me.</li></ul></ul>
		/// <h4>Parameters de lieux</h4>
		///		<ul><li>Choix 2a : d�signation des lieux de d�part et d'arriv�e par texte :</li>
		///			<ul><li>dct=<commune de d�part> : commune de d�part sous forme de texte</li>
		///			<li>[dpt=<arr�t de d�part>] : arr�t de d�part sous forme de texte. Si non fourni, les arr�ts
		///			  principaux de la commune sont choisis.</li>
		///			<li>act=<commune d'arriv�e> : commune d'arriv�e sous forme de texte</li>
		///			<li>[apt=<arr�t d'arriv�e>] : arr�t d'arriv�e sous forme de texte. Si non fourni, les arr�ts
		///			  principaux de la commune sont choisis.</li></ul>
		///		<li>Choix 2b : d�signation des lieux de d�part et d'arriv�e par pr�-enregistrement</li>
		///			<ul><li>fid=<id trajet favori> : identificateur de trajet favori de l'utilisateur connect�
		///			  Param�tres</li></ul></ul>
		/// <h4>Param�tres de calcul</h4>
		///		<ul><li>[msn=<nombre de solutions maximum>] : si non fourni, le nombre de solutions calcul�es
		///		  est illimit�. msn doit �tre un nombre strictement sup�rieur � 0.</li>
		///		<li>[ac=35001|35002|35003] : type d'utilisateur pour filtrer sur les r�gles d'accessibilit� et
		///		  appliquer les r�gles de r�servation ad�quates. 35001 = pi�ton, 35002 = PMR, 35003 = v�lo.
		///		  Si non fourni, le calcul s'effectue pour un utilisateur pi�ton.</li>
		///		<li>[tm=<id filtre mode de transport>] : identificateur de filtre</li></ul>
		/// <h4>Param�tres d'affichage (sortie XML uniquement)</h4>
		///		<ul>
		///		<li>[showResTab=yes/no] : "yes" par d�faut : choix d'afficher ou non la balise ResultTable</li>
		///		<li>[showCoords=yes/no] : "yes" par d�faut : choix d'afficher ou non les attributs de coordonn�es</li>
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
		///	La r�ponse se s�pare en trois parties :
		///
		/// @image html routeplanner_result_main.png
		///
		///	<ul><li>query : un rappel de la requ�te, permettant de remplir � nouveau un formulaire de re-interrogation du serveur</li>
		/// <li>journeys : un descriptif de chaque solution, permettant de construire les feuilles de route d�taill�es</li>
		/// <li>resultTable : une pr�sentation des heures de passage par arr�t, organis�es pour �tre affich�es sous forme de tableau synth�tique</li></ul>
		///	
		/// <h4>Rappel de la requ�te</h4>
		///
		///	La requ�te (query) est d�finie comme suit :
		/// 
		/// @image html xml_query.png
		///
		/// <ul><li>Attributs :</li><ul>
		///		<li>siteId : identificateur du site demand�</li>
		///		<li>userProfile : type d'utilisateur pour filtrer sur les r�gles d'accessibilit� et appliquer les r�gles de r�servation ad�quates. 35001 = pi�ton, 35002 = PMR, 35003 = v�lo. Si la valeur n'�tait pas fournie lors de la requ�te, le champ est renseign� par la valeur par d�faut 35001 (pi�ton).</li>
		///		<li>maxSolutions (optionnel) : nombre maximal de solutions. Si le nombre de solutions n'�tait pas limit�, alors l'objet n'est pas fourni</li>
		///		<li>sessionID (optionnel) : identificateur de la session fournie si valid�e</li></ul>
		///	<li>Plage temporelle du calcul (objet timeBounds) telle que d�finie par les param�tres fournis ou r�sultant de l'application d'une p�riode � une date :</li><ul>
		///		<li>minDepartureHour : heure de d�part minimale demand�e (valeur du param�tre d'entr�e hi, si non fourni initialement : contient la valeur par d�faut qui a �t� affect�e par le syst�me) NB : Cette valeur peut �tre diff�rente de l'heure de d�part de la premi�re solution trouv�e</li>
		///		<li>maxDepatureHour : heure de d�part maximale demand�e (valeur du param�tre ha, si non fourni initialement : contient la valeur par d�faut qui a �t� affect�e par le syst�me). NB : Cette valeur peut �tre diff�rente de l'heure de d�part de la derni�re solution trouv�e</li>
		///		<li>minArrivalHour : heure d'arriv�e minimale demand�e (valeur du param�tre ii, si non fourni initialement : contient la valeur par d�faut qui a �t� affect�e par le syst�me) NB : Cette valeur peut �tre diff�rente de l'heure d'arriv�e de la premi�re solution trouv�e</li>
		///		<li>maxArrivalHour : heure d'arriv�e maximale demand�e (valeur du param�tre ia, si non fourni initialement : contient la valeur par d�faut qui a �t� affect�e par le syst�me). NB : Cette valeur peut �tre diff�rente de l'heure d'arriv�e de la derni�re solution trouv�e</li></ul>
		///	<li>P�riode de calcul (optionnel : si effectivement utilis�e par la requ�te) :</li><ul>
		///		<li>date : date demand�e (valeur du param�tre da, si non fourni initialement : contient la valeur par d�faut qui a �t� affect�e par le syst�me)</li>
		///		<li>id : id de la p�riode dans la base de donn�es</li>
		///		<li>name : texte de libell� de la p�riode</li></ul>
		///	<li>Lieux de d�part et d'arriv�e (objet places) :</li>
		///		<li>departureCity : commune de d�part sous forme de texte</li>
		///		<li>departureCityNameTrust : taux de confiance de l'interpr�tation du nom de commune entr� (1 = correspondance exacte, 0 = champ commune vide)</li>
		///		<li>departureStop (optionnel) : arr�t de d�part sous forme de texte. Si le calcul d'effectue au d�part d'une commune (calcul effectu� au d�part des lieux principaux de la commune) alors l'objet n'est pas fourni.</li>
		///		<li>departureStopNameTrust : taux de confiance de l'interpr�tation du nom de l'arr�t entr� (1 = correspondance exacte ou champ vide)</li>
		///		<li>arrivalCity : commune d'arriv�e sous forme de texte</li>
		///		<li>arrivalCityNameTrust : taux de confiance de l'interpr�tation du nom de commune entr� (1 = correspondance exacte, 0 = champ commune vide)</li>
		///		<li>arrivalStop (optionnel) : arr�t d'arriv�e sous forme de texte. Si le calcul d'effectue vers une commune (calcul effectu� � l'arriv�e des lieux principaux de la commune) alors l'objet n'est pas fourni.</li>
		///		<li>arrivalStopNameTrust : taux de confiance de l'interpr�tation du nom de l'arr�t entr� (1 = correspondance exacte ou champ vide)</li></ul>
		///	<li>Trajet favori (objet favorite, optionnel : uniquement si fourni dans la requ�te) :</li><ul>
		///		<li>id : identificateur du trajet favori demand�</li></ul>
		///	<li>Filtre de mode transports (objet transportModeFilter, optionnel)</li><ul>
		///		<li>id : identificateur du filtre</li>
		///		<li>name : nom du filtre</li></ul></ul>
		///
		/// <h4>Itin�raires</h4>
		///		Les solutions (journeys) sont une composition de solutions (journey). Si aucune solution n'a pu �tre trouv�e, l'objet est vide. Le diagramme suivant montre la composition de chaque objet solution, d�crite en trois parties :
		///
		/// @image html xml_journeys.png
		///
		/// <ul><li>les attributs, qui donnent des informations de r�sum� sur le contenu des tron�ons</li>
		///		<ul>
		///			<li>continuousServiceDuration (optionnel) : indique par sa pr�sence que la solution est valable en continu pendant une plage horaire. L'attribut contient la dur�e de cette plage horaire. Les bornes effectives de la plage horaire sont d�finies par les heures de d�part du premier tron�on de l'itin�raire (chunk). Si cet attribut n'est pas fourni, alors l'itin�raire d�finit une solution unique � heure fixe</li>
		///			<li>hasAStopAlert (bool�en) : si vrai, indique qu'au moins un itin�raire passe par au moins un arr�t ayant un message d'alerte valable � l'heure de passage de l'itin�raire.</li>
		///			<li>hasALineAlert (bool�en) : si vrai, indique qu'au moins un itin�raire utilise au moins une ligne ayant un message d'alerte valable � l'heure d'utilisation</li>
		///		</ul>
		///		<li>l'objet reservation (optionnel) : indique par sa pr�sence que l'itin�raire est soumis � une modalit� de r�servation des places, d�finie par une s�rie d'attributs. L'absence d'objet indique que la r�servation est impossible :</li>
		///		<ul>
		///			<li>type : compulsory = r�servation obligatoire des places, optional = r�servation possible des places</li>
		///			<li>deadLine : indique la date et l'heure limite de r�servation. Au-del� de cette limite, la r�servation devient impossible. Si elle est obligatoire, alors l'itin�raire n'est plus utilisable. Si elle est facultative, alors l'itin�raire reste utilisable mais n'est plus ouvert � la r�servation des places</li>
		///			<li>online  (bool�en) : indique si la r�servation des places est possible directement sur le serveur SYNTHESE</li>
		///			<li>phoneNumber (optionnel) : indique le num�ro de t�l�phone de la centrale de r�servation t�l�phonique</li>
		///			<li>openingHours (optionnel) : indique les horaires d'ouverture de la centrale de r�servation t�l�phonique</li>
		///		</ul>
		///		<li>l'objet chunks contient les tron�ons qui d�crivent l'itin�raire dans l'ordre chronologique d'utilisation. Chaque tron�on peut �tre de trois types diff�rents :</li>
		///		<ul>
		///			<li>street : marche � pied le long d'une rue d�sign�e</li>
		///			<li>transport : utilisation d'une ligne de transport public</li>
		///			<li>connection : transfert � pied d'un arr�t vers un autre diff�rent, sans pour autant fournir le descriptif du trajet � emprunter</li>
		///	</ul></ul>
		///
		///	<h4>Tron�on d'itin�raire � pied le long d'une rue</h4>
		///		Un section de marche � pied le long d'une rue (street) est d�crite comme suit :
		///
		/// @image html routeplanner_result_street.png
		///
		///	<ul><li>departureTime : date/heure de d�part. Si service continu, il s'agit du d�but de la plage horaire � l'arriv�e du tron�on</li>
		///		<li>endDepartureTime (optionnel, si service continu uniquement) : fin de la plage horaire de desserte au d�part du tron�on</li>
		///		<li>arrivalTime : date/heure d'arriv�e. Si service continu, il s'agit du d�but de la plage horaire � l'arriv�e du tron�on</li>
		///		<li>endArrivalTime (optionnel, si service continu uniquement) : fin  de la plage horaire de desserte � l'arriv�e du tron�on</li>
		///		<li>length : longueur du tron�on en m�tres</li>
		///		<li>city : nom de la ville � laquelle appartient la rue utilis�e</li>
		///		<li>name : nom de la rue utilis�e</li>
		///		<li>startAddress : objet lieu de d�part du tron�on (voir type PlaceType correspondant)</li>
		///		<li>endAddress : objet lieu d'arriv�e du tron�on (voir type PlaceType correspondant)</li></ul>
		///
		///	<h4>Tron�on d'itin�raire sur ligne de transport public</h4>
		///
		///	Une section utilisant une ligne de transport public (JourneyTransportType) est d�crite comme suit :
		///
		/// @image html routeplanner_result_journey.png
		///
		/// <ul><li>departureTime : date/heure de d�part. Si service continu, il s'agit du d�but de la plage horaire � l'arriv�e du tron�on</li>
		///		<li>endDepartureTime (optionnel, si service continu uniquement) : fin de la plage horaire de desserte au d�part du tron�on</li>
		///		<li>arrivalTime : date/heure d'arriv�e. Si service continu, il s'agit du d�but de la plage horaire � l'arriv�e du tron�on</li>
		///		<li>endArrivalTime (optionnel, si service continu uniquement) : fin  de la plage horaire de desserte � l'arriv�e du tron�on</li>
		///		<li>length : longueur du tron�on en m�tres</li>
		///		<li>possibleWaitingTime (optionnel) : indique par sa pr�sence que le temps de parcours inclut une dur�e d'attente potentielle, qui peut faire l'objet d'une indication au client</li>
		///		<li>startStopIsTerminus : indique si l'arr�t de d�part du trajet est l'origine de la ligne utilis�e</li>
		///		<li>endStopIsTerminus : indique si l'arr�t d'arriv�e du trajet est la destination de la ligne utilis�e</li>
		///		<li>destinationText : si pr�sent, indique la destination affich�e sur le v�hicule � emprunter</li>
		///		<li>startStop : arr�t physique de d�part (type PhysicalStopType � voir plus bas)</li>
		///		<li>endStop : arr�t physique d'arriv�e (type PhysicalStopType � voir plus bas)</li>
		///		<li>destinationStop : arr�t physique de destination du v�hicule � emprunter</li>
		///		<li>line : ligne utilis�e, d�crite par plusieurs attributs :</li>
		///		<ul><li>id : identificateur de la ligne dans la base de donn�es de SYNTHESE (p�renne)</li>
		///			<li>color (optionnel) : couleur de la ligne au format RGB HTML (#RRGGBB)</li>
		///			<li>imgURL (optionnel) : URL d'une image repr�sentant le logo de la ligne. Par convention, cette URL peut �tre partielle pour permettre la mise � disposition de plusieurs versions d'images (ex : sncf-grand.png et sncf-petit.png)</li>
		///			<li>cssClass (optionnel) : classe CSS d�finissant le graphisme du num�ro de ligne dans un cartouche (couleur de fond, couleur de texte, style de caract�res, etc.). Correspond en g�n�ral � la repr�sentation officielle de la ligne sur les divers supports</li>
		///			<li>shortName (optionnel) : num�ro de ligne</li>
		///			<li>longName (optionnel) : texte d�signant la ligne incluant l'article (ex. : la navette du cimeti�re)</li>
		///			<li>alert (optionnel) : message d'alerte en cours de validit� sur la ligne au moment du trajet (type AlertType � voir plus bas)</li></ul>
		///		<li>vehicleType : mode de transport, d�crit par plusieurs attributs :</li>
		///		<ul><li>id : identificateur du mode de transport dans la base de donn�es SYNTHESE (p�renne)</li>
		///			<li>name : nom du mode de transport incluant l'article (ex : le train)</li>
		///	</ul></ul>
		///
		/// <h4>Arr�ts physiques</h4>
		///
		///	Un arr�t physique (PhysicalStopType) est d�crit comme suit :
		///
		/// @image html routeplanner_result_stop.png
		///
		/// <ul><li>x, y : coordonn�es de l'arr�t selon la projection Lambert II Etendu</li>
		///		<li>latitude, longitude : coordonn�es de l'arr�t selon le syst�me WGS84</li>
		///		<li>id : identificateur de l'arr�t dans la base de donn�es de SYNTHESE (p�renne)</li>
		///		<li>name (optionnel) : nom de l'arr�t permettant de le distinguer par rapport aux autres arr�ts de la m�me zone d'arr�ts (ex : quai 12)</li>
		///		<li>connectionPlace : zone d'arr�t (ConnectionPlaceType � voir plus loin) � laquelle appartient l'arr�t</li>
		///	</ul>
		///
		/// <h4>Tron�on d'itin�raire de jonction � pied</h4>
		///
		///	Un trajet � pied de liaison entre deux arr�ts sans pr�cision concernant les rues � emprunter (JourneyConnectionType) est d�crit comme suit :
		///
		/// @image html routeplanner_result_connect.png
		///
		/// <ul><li>departureTime : date/heure de d�part. Si service continu, il s'agit du d�but de la plage horaire � l'arriv�e du tron�on</li>
		///		<li>endDepartureTime (optionnel, si service continu uniquement) : fin de la plage horaire de desserte au d�part du tron�on</li>
		///		<li>arrivalTime : date/heure d'arriv�e. Si service continu, il s'agit du d�but de la plage horaire � l'arriv�e du tron�on</li>
		///		<li>endArrivalTime (optionnel, si service continu uniquement) : fin  de la plage horaire de desserte � l'arriv�e du tron�on</li>
		///		<li>length : longueur du tron�on en m�tres</li>
		///		<li>startStop : arr�t physique de d�part (type PhysicalStopType � voir plus haut)</li>
		///		<li>endStop : arr�t physique d'arriv�e (type PhysicalStopType � voir plus haut)</li>
		///	</ul>
		///
		/// <h4>Tableau des r�sultats</h4>
		///
		/// Le tableau des r�sultats tri� par zone d'arr�t (type ResultTableType) est d�crit comme suit :
		///
		/// @image html xml_resulttable.png
		///
		/// Chaque rang�e du tableau est repr�sent�e par un objet nomm� row, contenant les �l�ments suivants (pas d'objet si aucune solution trouv�e) :
		/// <ul><li>type : type de rang�e de tableau (departure (d�part), connection (correspondance), ou arrival (arriv�e))</li>
		///		<li>place : lieu correspondant � la ligne du tableau (type PlaceType � voir plus bas)</li>
		///		<li>cells : cellules de la rang�e, compos�e d'objets cell :</li>
		///		<ul><li>arrivalDateTime : date/heure d'arriv�e, fourni uniquement si la colonne repr�sente une solution qui dessert en arriv�e l'arr�t repr�sent� par la ligne</li>
		///			<li>endArrivalDateTime : derni�re date/heure d'arriv�e dans un service continu, fourni uniquement si la colonne repr�sente une solution qui dessert en arriv�e l'arr�t repr�sent� par la ligne, et dans le cas d'un service continu</li>
		///			<li>departureDateTime : date/heure de d�part, fourni uniquement si la colonne repr�sente une solution qui dessert en d�part l'arr�t repr�sent� par la ligne</li>
		///			<li>endDepartureDateTime : derni�re date/heure de d�part dans un service continu, fourni uniquement si la colonne repr�sente une solution qui dessert en d�part l'arr�t repr�sent� par la ligne, et dans le cas d'un service continu</li>
		///			<li>pedestrian : si pr�sent, indique que la cellule est concern�e par un trajet pi�ton, qui peut alors �tre affich� diff�remment. Les valeurs possibles de pedestrian sont :
		///			<ul><li>departure : indique qu'un trajet pi�ton d�marre � la cellule. Un pictogramme peut remplacer l'heure de d�part si souhait�.</li>
		///				<li>arrival : indique qu'un trajet pi�ton se termine sur la cellule. Un pictograme peut remplacer l'heure d'arriv�e si souhait�.</li>
		///				<li>traversal : indique qu'un trajet pi�ton a d�marr� avant cette cellule et se terminera apr�s cette cellule (dans le sens de lecture de la colonne). Un pictogramme peut l'indiquer si souhait�.</li>
		///			</ul>
		/// </ul></ul>
		///
		/// <h4>Lieux (g�n�rique)</h4>
		///
		///	Les lieux (PlaceType) peuvent �tre de trois types :
		/// @image html xml_place.png
		///
		/// <ul><li>connectionPlace : Zones d'arr�t (ConnectionPlaceType), d�crites comme suit :</li>
		/// @image html routeplanner_result_stopare.png
		///
		///		<ul><li>x, y : coordonn�es de la zone d'arr�t selon la projection Lambert II Etendu</li>
		///			<li>latitude, longitude : coordonn�es de la zone d'arr�t selon le syst�me WGS84</li>
		///			<li>id : identificateur de la zone d'arr�t dans la base de donn�es de SYNTHESE (p�renne)</li>
		///			<li>city : nom de la ville � laquelle appartient la zone d'arr�t</li>
		///			<li>name : nom de la zone d'arr�t dans la ville</li>
		///			<li>alert : message d'alerte valable dans au moins une des solutions utilisant la zone d'arr�t (type AlertType � voir plus bas)</li>
		///		</ul>
		///		<li>publicPlace : Lieux publics (PublicPlaceType) d�crits comme suit :</li>
		/// @image html routeplanner_result_public.png
		///		<ul><li>x, y : coordonn�es du lieu public selon la projection Lambert II Etendu</li>
		///			<li>latitude, longitude : coordonn�es du lieu public selon le syst�me WGS84</li>
		///			<li>id : identificateur du lieu public dans la base de donn�es de SYNTHESE (p�renne)</li>
		///			<li>city : nom de la ville � laquelle appartient le lieu public</li>
		///			<li>name : nom du lieu public dans la ville</li>
		///		</ul>
		///		<li>address : Adresse (AddressType), d�crites comme suit :</li>
		///	@image html xml_address.png
		///		<ul><li>x, y : coordonn�es du lieu public selon la projection Lambert II Etendu</li>
		///			<li>latitude, longitude : coordonn�es du lieu public selon le syst�me WGS84</li>
		///			<li>id : identificateur du lieu public dans la base de donn�es de SYNTHESE (p�renne)</li>
		///			<li>city : nom de la ville � laquelle appartient le lieu public</li>
		///			<li>number : num�ro de l'habitation. Optionnel : si non pr�sent, l'objet repr�sente la rue enti�re</li>
		///			<li>streetName : nom de la rue dans la ville</li>
		///	</ul></ul>
		///
		/// <h4>Message d'alerte</h4>
		///
		/// Un message d'alerte (AlertType) se d�finit comme suit :
		/// @image html xml_alert.png
		///
		/// <ul><li>contenu : texte du message d'alerte</li>
		///		<li>id : identificateur de l'alerte dans la base de donn�es SYNTHESE (p�renne)</li>
		///		<li>level : niveau d'alerte (info = information, warning = attention, interruption = interruption de service)</li>
		///		<li>startValidity : date/heure de d�but d'application du message d'alerte</li>
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
