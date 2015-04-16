
/** DisplaySearchAdmin class header.
	@file DisplaySearchAdmin.h

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

#ifndef SYNTHESE_DISPLAY_SEARCH_ADMIN_H
#define SYNTHESE_DISPLAY_SEARCH_ADMIN_H

#include "ResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"
#include <boost/optional.hpp>

namespace synthese
{
	namespace geography
	{
		class NamedPlace;
	}

	namespace departure_boards
	{
		class DisplayScreen;

		/** Ecran de gestion du parc matériel d'afficheurs.
			@ingroup m54Admin refAdmin

			@image html cap_admin_displays.png
			@image latex cap_admin_displays.png "Maquette de l'écran de gestion des afficheurs" width=14cm

			<i>Titre de la fenêtre</i> : SYNTHESE Admin - Afficheurs

			<i>Barre de navigation</i> :
				- Lien vers l'accueil de la console d'administration

			<i>Zone de contenus</i> :
				-# <b>Zone de recherche</b> : Différents champs permettent d'effectuer une recherche d'afficheurs :
					- @c UID : l'UID de l'afficheur doit contenir le texte entré dans le champ (numérique). Laisser le champ vide n'effectue aucun filtrage sur l'UID
					- @c Emplacement : liste de choix formée de l'ensemble des arrêts logiques du réseau qui possèdent au moins un afficheur. Un choix @c (tous) permet d'annuler le filtre sur l'emplacement.
					- @c Ligne : Liste de choix formée de l'ensemble des codes exploitant des lignes qui desservent au moins un arrêt logique possédant au moins un afficheur, représentées par leur code exploitant. Un choix @c (tous) permet d'annuler le filtre sur la ligne.
					- @c Type : Liste de choix formée par l'ensemble des types d'afficheurs définis sur l'écran @ref synthese::interface::DisplayTypesAdmin. Un choix @c (tousà permet d'annuler le filtre sur le type d'afficheur.
					- @c Etat : Liste de choix formée par des combinaisons d'état de maintenance :
						- @c (tous) : le filtre est désactivé
						- @c OK : seuls les afficheurs en bon état de fonctionnement sont sélectionnés
						- @c Warning : seuls les afficheurs en état Warning (matériel ou cohérence données) sont sélectionnés
						- @c Warning+Error : seuls les afficheurs en état Warning ou Error (matériel ou cohérence données) sont sélectionnés
						- @c Error : seuls les afficheurs en état Error (matériel ou cohérence données) sont sélectionnés
						- <tt>Hors service</tt> : seuls les afficheurs hors service sont sélectionnés
					- #c Message : Lise de choix formée par les éas d'envoi de message possibles :
						- #c (tous) : le filtre est désactivé
						- @c Aucun : seuls les afficheurs sans message sont sélecionnés
						- <tt>Un message</tt> : Seuls les aficheurs diffusant un message sont sélectionnés
						- <tt>Conflit</tt> : Seuls les afficheurs diffusant deux messages ou plus sont sélectionnés
						- <tt>Avec message</tt> : Seuls les afficheurs diffusant un message ou plus sont sélecionnés
					Au chargement, la page affiche l'ensemble des afficheurs pouvant être vus d'après les droits de l'utilisateur.
				-# <b>Tableau résultat de recherche</b> : Les colonnes suivantes sont présentes :
					- @c UID : Code SYNTHESE de l'afficheur. Un clic sur l'UID va vers la page @ref synthese::interface::DisplayAdmin.
					- @c Emplacement : Résumé de l'emplacement de l'afficheur. Un clic sur l'emplacement va vers la page @ref synthese::interface::DisplayLocations sur le lieu logique de l'afficheur. Le contenu est constitué par les trois champs suivants, séparés par un /.  :
						- Lieu logique
						- Lieu physique (arrêt physique ou autre emplacement interne au lieu logique)
						- Complément de précision
					- @c Type : Type d'afficheur. Un clic sur le type va vers la page @ref synthese::interface::DisplayTypesAdmin.
					- @c Etat : Résumé de l'état de maintenance. Un clic sur le contenu de la colonne va vers la page @ref synthese::interface::DisplayMaintenanceAdmin . Les pastilles affichent une infobulle lorsque le pointeur de souris les effleure, décrivant le texte correspondant à la couleur affichée :
						- Si l'afficheur est déclaré en service, deux points de couleur (vert = OK, orange = Warning, rouge = Error), faisant apparaître une infobulle précisant leur signification au contact du pointeur de souris - voir détail sur page DisplayMaintenanceAdmin :
							- le premier point correspond au contrôle de cohérence de données
							- le second point correspond à l'état du matériel
						- Si l'afficheur est déclaré hors service, la mention HS suivie de la date de la mise hors service est présente, en couleur rouge
					- #c Msg : Résumé de l'état de diffusion de message, sous forme de point coloré :
							- le point est vert avec une infobulle "Pas de message" si aucun message n'est diffusé sur l'afficheur
							- le point est orange avec une infobulle contenant un aperçu du message, si un message est diffusé sur l'afficheur
							- le point est rouge avec une infobulle "Conflit" indiquant le nombre de messages en confli, le message effectivement affiché, et le(s) message(s) masqué(s) par le conflit
					- @c Actions : Trois boutons permettent d'accéder aux fonctions suivantes :
						- @c Ouvrir: Dirige vers la page @ref synthese::interface::DisplayAdmin
						- @c Simuler : Ouvre une fenêtre pop-up effectuant un affichage similaire à ce qui est diffusé sur l'afficheur, pour les écrans de type HTML. Pour les écran au protocole Lumiplan, le code Lumiplan est affiché.
						- @c Supervision : Dirige vers la page @ref synthese::interface::DisplayMaintenanceAdmin
					- La dernière ligne du tableau comprend un lien "Créer un nouvel afficheur" et donne sur une page de paramétrage d'afficheur vide.
					- Un clic sur le titre des colonnes du tableau effectue un tri selon la colonne (ascendant puis descendant).
				-# <b>Lien afficheurs suivants</b> : Au maximum 50 afficheurs sont représentés sur la page. En cas de dépassement de ce nombre, ce lien apparaît et permet de se rendre aux afficheurs suivants. A partir de la seconde page, un lien <tt>Afficheurs précédents</tt> est également proposé.
				-# <b>Bouton Supprimer</b> : Permet de supprimer le(s) afficheur(s) sélectionné(s). Un message de confirmaion apparaît avant la suppression, permettant d'annuler l'action. Pour des raisons d'intégrité, la suppression n'efface pas l'afficheur de la base de données s'il a déjà diffusé au moins un message. Il permet de le supprimer des écrans de recherche.

			<i>Sécurité</i> :
				- Une habilitation publique ArrivalDepartureTableRight de niveau READ est nécessaire pour accéder à la page. Le résultat de la recherche dépend du périmètre de l'habilitation.
				- Une habilitation publique ArrivalDepartureTableRight de niveau WRITE est nécessaire pour obtenir les boutons <tt>Modifier</tt> et <tt>Créer un nouvel afficheur</tt>
				- Une habilitation publique ArrivalDepartureTableRight de niveau USE est nécessaire pour obtenir les boutons <tt>Simuler</tt>, sur les afficheurs contenus dans le périmètre de l'habilitation.
				- Une habilitation publique DisplayMaintenanceRight de niveau READ est nécessaire pour obtenir les boutons <tt>Supervision</tt>, sur les afficheurs contenus dans le périmètre de l'habilitation.
				- Une habilitation publique ArrivalDepartureTableRight de niveau DELETE est nécessaire pour obtenir le bouton <tt>Supprimer</tt>

			<i>Journal</i> : Les actions suivants entrainent une écriture dans le journal du téléaffichage ArrivalDepartureTableLog :
				- INFO : création d'un afficheur
				- INFO : suppression d'un afficheur

			<i>Journal</i> : Les actions suivants entraînent une écriture dans le journal de maintenance des afficheurs DisplayMaintenanceLog :
				- WARNING : première constatation d'une absence d'entrée de type contrôle sur un afficheur dans un délai compris entre 150% et 500% de la durée présupposée entre deux contrôles.
				- ERROR : première constatation d'une absence d'entrée de type contrôle sur un afficheur dans un délai supérieur à 500% de la durée présupposée entre deux contrôles.
				- NB : Ces deux entrées apparaissent à la première visualisation d'un problème de ce type dans une console d'administration, afin de suppléer à l'absence de démon de surveillance. Un passage en contrôle continu avec alerte pourrait être implémenté.
		*/
		class DisplaySearchAdmin :
			public admin::AdminInterfaceElementTemplate<DisplaySearchAdmin>
		{
			std::string										_searchCity;
			std::string										_searchStop;
			std::string										_searchName;
			boost::optional<util::RegistryKeyType>				_searchLineId;
			boost::optional<util::RegistryKeyType>				_searchTypeId;
			boost::optional<boost::shared_ptr<const geography::NamedPlace> >
															_place;
			boost::optional<int>							_searchState;
			boost::optional<int>							_searchMessage;
			html::ResultHTMLTable::RequestParameters		_requestParameters;

		public:
			static const std::string PARAMETER_SEARCH_CITY;
			static const std::string PARAMETER_SEARCH_STOP;
			static const std::string PARAMETER_SEARCH_NAME;
			static const std::string PARAMETER_SEARCH_LINE_ID;
			static const std::string PARAMETER_SEARCH_TYPE_ID;
			static const std::string PARAMETER_SEARCH_STATE;
			static const std::string PARAMETER_SEARCH_MESSAGE;
			static const std::string PARAMETER_SEARCH_LOCALIZATION_ID;

			static const std::string TAB_DISPLAY_SCREENS;
			static const std::string TAB_CPU;

			DisplaySearchAdmin();

			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			/** Parameters map generator, used when building an url to the admin page.
					@return util::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007
				*/
			virtual util::ParametersMap getParametersMap() const;



			/** Display of the content of the admin element.
				@param stream Stream to write on.
			*/
			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;


			static std::string getHtmlSearchForm(
				const html::HTMLForm& form
				, const std::string& cityName
				, const std::string& stopName
				, const std::string& displayName
				, boost::optional<util::RegistryKeyType> lineUid,
				boost::optional<util::RegistryKeyType> typeUid,
				boost::optional<int> state,
				boost::optional<int> message
			);

			bool isAuthorized(
				const security::User& user
			) const;

			/** Gets sub page of the designed parent page, which are from the current class.
				@param factoryKey Key of the parent class
				@param request User request
				@return PageLinks A link to the page if the parent is Home
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const server::ModuleClass& module,
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;


			virtual AdminInterfaceElement::PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;

			virtual std::string getTitle() const;


			virtual void _buildTabs(
				const security::Profile& profile
			) const;

			void setPlace(boost::optional<util::RegistryKeyType> id);

			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;

			//! @name Getters
			//@{
				boost::optional<boost::shared_ptr<const geography::NamedPlace> > getPlace() const { return _place; }
			//@}
		};
}	}

#endif
