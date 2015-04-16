
/** DisplayAdmin class header.
	@file DisplayAdmin.h

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

#ifndef SYNTHESE_DISPLAY_ADMIN_H
#define SYNTHESE_DISPLAY_ADMIN_H

#include "AdminInterfaceElementTemplate.h"
#include "DBLogHTMLView.h"

namespace synthese
{
	namespace pt
	{
		class StopArea;
	}

	namespace departure_boards
	{
		class DisplayScreen;
		class DisplayMonitoringStatus;

		/** Ecran de paramétrage d'un afficheur.
			@ingroup m54Admin refAdmin

			@section tech Onglet technique

			@image html cap_admin_display.png
			@image latex cap_admin_display.png "Maquette de l'écran de paramétrage d'un afficheur" width=14cm

			<i>Titre de la fenêtre</i> : SYNTHESE Admin - Afficheurs - [Lieu logique] - [Lieu physique] - [Complément de précision]

			<i>Barre de navigation</i> :
 				- Lien vers @ref synthese::interface::HomeAdmin
				- Lien vers @ref synthese::interface::DisplaySearchAdmin
				- Nom du lieu logique auquel appartient l'afficheur, lien vers @ref synthese::interface::DisplayLocationsAdmin sur le lieu logique de l'afficheur
				- Nom du lieu physique auquel appartient l'afficheur
				- Complément de précision sur l'emplacement de l'afficheur (non affiché si le complément est vide)

			<i>Zone de contenus</i> : Un formulaire de modification constitué par les éléments suivants :
				-# <b>Emplacement</b>
					-# <tt>Lieu logique</tt> : Liste déroulante constituée par l'ensemble des arrêts logiques triés par ordre alphabétique de commune puis par ordre alphabérique de nom. Champ obligatoire. En cas de changement de lieu logique, la page est rafraichie pour mettre à jour la liste des lieux physiques.
					-# <tt>Lieu physique</tt> : Liste déroulante constituée par l'ensemble des lieux physiques présents dans le lieu logique sélectionné (arrêts physiques + autres lieux d'affichages gérés par la fonction @ref synthese::interfaces::DisplayLocationsAdmin. Ce champ est obligatoire.
					-# <tt>Geometry</tt>: Géolocalisation de l'afficheur
					-# <tt>Complément de précision</tt> : Champ texte libre permettant de préciser l'emplacement de l'afficheur, utile s'il se trouve dans un lieu physique déjà équipé d'un autre afficheur. Ce champ est obligatoire si au moins deux afficheurs sont présents dans le même lieu physique. Il est facultatif sinon.
				-# <b>Données techniques</b>
					-# <tt>Type d'afficheur</tt> : Liste déroulante constituée par l'ensemble des types d'afficheur disponibles (issus de l'écran @ref synthese::interface::DisplayTypesAdmin). Ce champ est obligatoire.
					-# <tt>Code de branchement</tt> : Liste déroulante consituée par les nombres de 0 à 128. Ce champ correspond au code à envoyer par l'interface pour sélectionner l'afficheur destinataire parmi un groupe branché sur le même port RS485. Ce champ est obligatoire si l'afficheur est de type Oscar. Si l'afficheur n'est pas de type Oscar, la valeur données à ce champ est ignorée.
					-# <tt>UID</tt> : ID unique généré par le système pour désigner l'afficheur. L'UID est fourni à titre informatif mais ne peut être modifié par l'utilisateur.
				-# <b>Apparence</b>
					-# <tt>Titre</tt> : Titre à afficher en haut de l'afficheur. Champ facultatif. Champ inopérant pour l'interface Lumiplan (ce paramètre peut être inopérant si le champ n'est pas intégré à l'interface).
					-# <tt>Clignotement</tt> : Liste déroulante permettant de choisir la règle de clignotement de l'heure de départ (ce paramètre peut être désactivé par l'interface) :
						- Pas de clignotement
						- 1 minute avant l'effacement
						- 2 minutes avant l'effacement
						- 3 minutes avant l'effacement
					-# <tt>Affichage numéro de quai</tt> : Choix OUI/NON indiquant si le numéro de quai de départ doit être affiché (ce paramètre peut être inopérant si le champ n'est pas intégré à l'interface)
					-# <tt>Affichage numéro de service</tt> : Choix OUI/NON indiquant si le numéro de service doit être affiché (ce paramètre peut être inopérant si le champ n'est pas intégré à l'interface)
				-# <b>Contenu</b>
					-# <tt>Horaires</tt> : Liste déroulante permettant de choisir entre l'affichage du tableau de départs ou des arrivées
					-# <tt>Sélection terminus</tt> : Liste déroulante permettant de choisir entre :
						- l'affichage de tous les passages au droit des arrêts sélectionnés
						- l'affichage des seuls services ayant pour origine ou terminus les arrêts sélectionnés
					-# <tt>Délai maximum d'affichage</tt> : Champ numérique précisant le délai maximal entre l'heure courante et l'heure de départ pour qu'un service soit affiché. La durée fournie est en minutes. Une valeur 0 désactive l'affichage. Une valeur non renseignée implique l'affichage jusqu'à remplissage de l'écran, quel que soit le délai entre l'heure courante et l'heure de départ. Attention : il est déconseillé de mettre un délai supérieur à 24 heures, car dans le cas contraire on ne peut alors pas différencier un départ du jour d'un départ le lendemain, le jour de départ n'étant pas affiché.
					-# <tt>Délai d'effacement</tt> : Liste déroulante permettant de choisir le moment où le départ n'est plus affiché :
						- 3 minutes avant le départ
						- 2 minutes avant le départ
						- 1 minute avant le départ
						- à l'heure exacte du départ
						- 1 minutes après le départ
						- 2 minutes après le départ
						- 3 minutes après le départ
					-# <tt>Sélection sur arrêt physique</tt> : Liste présentant l'ensemble des arrêts physiques de l'arrêt logique. Cocher l'un d'entre eux entraîne la sélection des départs se produisant à l'arrêt physique. Plusieurs arrêts physiques peuvent être sélectionnés. Une case supplémentaire est proposée en fin de liste : <tt>Tous (y compris nouveaux)</tt>. Ce choix consiste à afficher l'ensemble des départs de l'arrêt logique. La différence entre ce choix et cocher tous les arrêts physiques réside dans le fait qu'en cas de création d'un nouvel arrêt physique, il est intégré automatiquement à l'affichage si la case Tous est cochée, et ne l'est pas sinon.
					-# <tt>Sélection sur arrêt logique à ne pas desservir</tt> : Liste présentant l'ensemble des arrêts logiques qui ne doivent pas faire partie des destinations d'une ligne pour qu'elle soit affichée :
						- Un bouton <tt>Supprimer</tt> est présent au niveau de chaque arrêt logique permettant de supprimer un arrêt à ne pas desservir
						- Une liste déroulante munie d'un bouton <tt>Ajouter</tt> permet d'ajouter un arrêt logique à ne pas desservir. Le contenu de la liste déroulante est l'ensemble des arrêts logiques desservis par les lignes passant par l'emplacement de l'afficheur.
				-# <b>Présélection</b>
					-# <tt>Activer</tt> : Choix OUI/NON permettant de mettre en service la règle de présélection de départs s'ils desservent une destination majeure non desservie par les prochains départs selon l'ordre chronologique. En cas d'activation de cette règle, les derniers départs de l'ordre chronologique sont remplacés par les premiers départs permettant de se rendre à l'une des destinations faisant l'objet de la préselection.
					-# <tt>Délai maximum présélection</tt> : Champ numérique indiquant le délai maximum entre l'heure courante et l'heure de départ qu'un service potentiellement préselectionné doit respecté pour être affiché. La valeur fournie est en minutes. Une valeur 0 désactive la présélection. Une valeur non fournie indique que la présélection s'opère sans limite de durée.
					-# <tt>Arrêts de desserte intermédiaire supplémentaires</tt> : Liste présentant l'ensemble des arrêt logiques faisant l'objet d'une présélection qui s'ajoutent aux terminus des différents parcours types des lignes affichées. Il est inutile d'ajouter ici un terminus de ligne : il fait obligatoirement partie de la présélection.
						- Un bouton <tt>Supprimer</tt> est présent au niveau de chaque arrêt logique permettant de supprimer un arrêt à présélectionner
						- Une liste déroulante munie d'un bouton <tt>Ajouter</tt> permet d'ajouter un arrêt logique à présélectionner. Le contenu de la liste déroulante est l'ensemble des arrêts logiques desservis par les lignes partant de l'emplacement de l'afficheur.

			<i>Sécurité</i> :
				- Une habilitation publique ArrivalDepartureTableRight de niveau READ est nécessaire pour afficher la page en mode consultation. L'emplacement de l'afficheur doit entrer dans le périmètre de l'habilitation.
				- Une habilitation publique ArrivalDepartureTableRight de niveau WRITE est nécessaire pour afficher la page en mode modification. L'emplacement de l'afficheur doit entrer dans le périmètre de l'habilitation. Le contenu des diverses listes déroulantes est impacté par le périmètre de l'habilitation.

			<i>Journal</i> : Les actions suivantes génèrent une entrée dans le journal du téléaffichage ArrivalDepartureTableLog :
				- INFO : Modification d'un afficheur

			@section maint Onglet maintenance

			@image html cap_admin_display_maint.png
			@image latex cap_admin_display_maint.png "Maquette de l'écran de supervision d'un afficheur" width=14cm

			<i>Titre de la fenêtre</i> : SYNTHESE Admin - Afficheurs - [Lieu logique] - [Lieu physique] - [Complément de précision] - Maintenance

			<i>Barre de navigation</i> :
 				- Lien vers @ref synthese::interface::HomeAdmin
				- Lien vers @ref synthese::interface::DisplaySearchAdmin
				- Nom du lieu logique auquel appartient l'afficheur, lien vers @ref synthese::interface::DisplayLocationsAdmin sur le lieu logique de l'afficheur
				- Nom du lieu physique auquel appartient l'afficheur
				- Complément de précision sur l'emplacement de l'afficheur (non affiché si le complément est vide)
				- Texte <tt>Maintenance</tt>

			<i>Zone de contenus</i> :
				-# <b>Formulaire de paramétrage</b> :
					-# <tt>Nombre de contrôles matériels par jour</tt> : Liste déroulante définissant le nombre de contrôles matériels devant être faits chaque jour (valeurs possibles 1 à 48), afin de déclencher une alerte en cas d'absence de contrôle au delà d'une certaine durée
					-# <tt>Afficheur déclaré en service</tt> : Choix OUI/NON permettant de déclarer l'afficheur en service ou non :
						- OUI : l'afficheur est considéré comme théoriquement en service
						- NON : l'afficheur est considéré comme hors service
					-# <tt>Message de maintenance</tt> : Texte libre destiné à recevoir des compléments d'informations sur l'état de maintenance de l'afficheur (ex : nature de la panne, etc)
				-# Zone <b>Contrôle de cohérence des données</b> : le contrôle de cohérence de données est effectué en temps réel lors de l'affichage de la page.
					-# Etiquette colorée : Indique le résultat du dernier contrôle suivi de la durée depuis laquelle le résultat est identique :
						- <tt>Warning</tt> (orange) si le contenu de l'afficheur est vide car aucune ligne ne fréquente l'afficheur (si l'afficheur est vide durant la totalité du délai de validité de la base SYNTHESE)
						- <tt>Error</tt> (rouge) si l'afficheur est positionné sur un emplacement qui n'existe plus (ex : Arrêt physique supprimé extérieurement)
					-# <tt>Motif de l'alerte</tt> : Texte explicatif sur l'erreur relevée
					-# <tt>Date du dernier contrôle positif</tt> : Indique la dernière date connue de contrôle de cohérence positif
				-# Zone <b>Contrôle du matériel</b> : le contrôle du matériel consiste en la lecture du journal alimenté par ailleurs par les postes clients.
					-# Etiquette colorée : Indique le résultat du dernier contrôle, suivi de la durée depuis laquelle le résultat est identique :
						- <tt>Warning</tt> (orange) si aucun contrôle n'est présent dans le journal depuis une période comprise entre 150% et 500% de la durée présupposée entre deux contrôles (paramètre <tt>Nombre de contrôles par jour</tt>). Cette situation indique que le client n'a pas mis à jour le journal, sans pour autant pouvoir assurer que le terminal est bel est bien hors de fonctionnement.
						- <tt>Warning</tt> (orange) si un contrôle est présent dans le journal, et relève une avarie n'ayant pas d'impact direct sur l'affichage, mais devant toutefois faire l'objet d'un traitement
						- <tt>Error</tt> (rouge) si un contrôle est présent dans le journal, et relève une avarie ayant conduit le terminal à s'éteindre
						- <tt>Error</tt> (rouge) si aucun contrôle n'est présent dans le journal depuis une période supérieure à 500% de la durée présupposée entre deux contrôles (paramètre <tt>Nombre de contrôles par jour</tt>).
						- <tt>Error</tt> (rouge) si le terminal est déclaré hors service au niveau du présent écran.
					-# <tt>Date du dernier contrôle positif</tt> : Indique la dernière date connue de contrôle matériel positif
					-# <tt>Motif de l'alerte</tt> : Texte explicatif sur l'erreur relevée

			<i>Sécurité</i> :
				- Une habilitation publique DisplayMaintenanceRight de niveau READ est nécessire pour afficher la page en mode consultation. L'emplacement de l'afficheur doit entrer dans le périmètre de l'habilitation.
				- Une habilitation publique DisplayMaintenanceRight de niveau WRITE est nécessire pour afficher la page en mode modification. L'emplacement de l'afficheur doit entrer dans le périmètre de l'habilitation.

			<i>Journal</i> : Les actions suivantes génèrent une entrée dans le journal de maintenance des afficheurs DisplayMaintenanceLog :
				- INFO : Modification de paramètre
				- WARNING : Détection d'une erreur de cohérence de données lors de l'affichage de la page
				- ERROR : Passage en hors service
				- WARNING : première constatation d'une absence d'entrée de type contrôle sur un afficheur dans un délai compris entre 150% et 500% de la durée présupposée entre deux contrôles.
				- ERROR : première constatation d'une absence d'entrée de type contrôle sur un afficheur dans un délai supérieur à 500% de la durée présupposée entre deux contrôles.
				- NB : Ces deux entrées apparaissent à la première visualisation d'un problème de ce type dans une console d'administration, afin de suppléer à l'absence de démon de surveillance. Un passage en contrôle continu avec alerte pourrait être implémenté.
				- NB : Des entrées INFO/WARNING/ERROR sont entrées dans le journal de maintenance directement par les clients de supervision via une requête SendLogEntry.
		*/
		class DisplayAdmin:
			public admin::AdminInterfaceElementTemplate<DisplayAdmin>
		{
		public:
			static const std::string TAB_TECHNICAL;
			static const std::string TAB_MAINTENANCE;
			static const std::string TAB_CONTENT;
			static const std::string TAB_APPEARANCE;
			static const std::string TAB_RESULT;
			static const std::string TAB_LOG;

		private:
			boost::shared_ptr<const DisplayScreen>				_displayScreen;
			boost::shared_ptr<const DisplayMonitoringStatus>	_status;
			dblog::DBLogHTMLView								_maintenanceLogView;
			dblog::DBLogHTMLView								_generalLogView;

		public:
			DisplayAdmin();

			void setScreen(boost::shared_ptr<const DisplayScreen> value);
			boost::shared_ptr<const DisplayScreen> getScreen() const;

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

			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;

			bool isAuthorized(
				const security::User& user
			) const;

			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;

			virtual std::string getTitle() const;

			virtual void _buildTabs(
				const security::Profile& profile
			) const;

			virtual AdminInterfaceElement::PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;
		};
}	}

#endif
