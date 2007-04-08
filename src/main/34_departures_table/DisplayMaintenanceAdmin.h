
/** DisplayMaintenanceAdmin class header.
	@file DisplayMaintenanceAdmin.h

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

#ifndef SYNTHESE_DisplayMaintenanceAdmin_H__
#define SYNTHESE_DisplayMaintenanceAdmin_H__

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace departurestable
	{
		class DisplayScreen;

		/** Ecran de supervision d'un afficheur.
			@ingroup m34

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
		class DisplayMaintenanceAdmin : public admin::AdminInterfaceElement
		{
			DisplayScreen*	_displayScreen;

		public:
			
			DisplayMaintenanceAdmin();
			
			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(const server::ParametersMap& map);

			/** Display of the content of the admin element.
				@param stream Stream to write on.
			*/
			void display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request=NULL) const;

			/** Title of the admin compound.
				@return The title of the admin compound, for display purposes.
			*/
			std::string getTitle() const;

			bool isAuthorized(const server::FunctionRequest<admin::AdminRequest>* request) const;

		};
	}
}

#endif // SYNTHESE_DisplayMaintenanceAdmin_H__
