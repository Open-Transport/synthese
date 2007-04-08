
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
			@image latex cap_admin_display_maint.png "Maquette de l'�cran de supervision d'un afficheur" width=14cm

			<i>Titre de la fen�tre</i> : SYNTHESE Admin - Afficheurs - [Lieu logique] - [Lieu physique] - [Compl�ment de pr�cision] - Maintenance

			<i>Barre de navigation</i> :
 				- Lien vers @ref synthese::interface::HomeAdmin
				- Lien vers @ref synthese::interface::DisplaySearchAdmin
				- Nom du lieu logique auquel appartient l'afficheur, lien vers @ref synthese::interface::DisplayLocationsAdmin sur le lieu logique de l'afficheur
				- Nom du lieu physique auquel appartient l'afficheur
				- Compl�ment de pr�cision sur l'emplacement de l'afficheur (non affich� si le compl�ment est vide)
				- Texte <tt>Maintenance</tt>

			<i>Zone de contenus</i> : 
				-# <b>Formulaire de param�trage</b> :
					-# <tt>Nombre de contr�les mat�riels par jour</tt> : Liste d�roulante d�finissant le nombre de contr�les mat�riels devant �tre faits chaque jour (valeurs possibles 1 � 48), afin de d�clencher une alerte en cas d'absence de contr�le au del� d'une certaine dur�e
					-# <tt>Afficheur d�clar� en service</tt> : Choix OUI/NON permettant de d�clarer l'afficheur en service ou non :
						- OUI : l'afficheur est consid�r� comme th�oriquement en service
						- NON : l'afficheur est consid�r� comme hors service
					-# <tt>Message de maintenance</tt> : Texte libre destin� � recevoir des compl�ments d'informations sur l'�tat de maintenance de l'afficheur (ex : nature de la panne, etc)
				-# Zone <b>Contr�le de coh�rence des donn�es</b> : le contr�le de coh�rence de donn�es est effectu� en temps r�el lors de l'affichage de la page.
					-# Etiquette color�e : Indique le r�sultat du dernier contr�le suivi de la dur�e depuis laquelle le r�sultat est identique :
						- <tt>Warning</tt> (orange) si le contenu de l'afficheur est vide car aucune ligne ne fr�quente l'afficheur (si l'afficheur est vide durant la totalit� du d�lai de validit� de la base SYNTHESE)
						- <tt>Error</tt> (rouge) si l'afficheur est positionn� sur un emplacement qui n'existe plus (ex : Arr�t physique supprim� ext�rieurement)
					-# <tt>Motif de l'alerte</tt> : Texte explicatif sur l'erreur relev�e
					-# <tt>Date du dernier contr�le positif</tt> : Indique la derni�re date connue de contr�le de coh�rence positif
				-# Zone <b>Contr�le du mat�riel</b> : le contr�le du mat�riel consiste en la lecture du journal aliment� par ailleurs par les postes clients.
					-# Etiquette color�e : Indique le r�sultat du dernier contr�le, suivi de la dur�e depuis laquelle le r�sultat est identique :
						- <tt>Warning</tt> (orange) si aucun contr�le n'est pr�sent dans le journal depuis une p�riode comprise entre 150% et 500% de la dur�e pr�suppos�e entre deux contr�les (param�tre <tt>Nombre de contr�les par jour</tt>). Cette situation indique que le client n'a pas mis � jour le journal, sans pour autant pouvoir assurer que le terminal est bel est bien hors de fonctionnement.
						- <tt>Warning</tt> (orange) si un contr�le est pr�sent dans le journal, et rel�ve une avarie n'ayant pas d'impact direct sur l'affichage, mais devant toutefois faire l'objet d'un traitement
						- <tt>Error</tt> (rouge) si un contr�le est pr�sent dans le journal, et rel�ve une avarie ayant conduit le terminal � s'�teindre
						- <tt>Error</tt> (rouge) si aucun contr�le n'est pr�sent dans le journal depuis une p�riode sup�rieure � 500% de la dur�e pr�suppos�e entre deux contr�les (param�tre <tt>Nombre de contr�les par jour</tt>).
						- <tt>Error</tt> (rouge) si le terminal est d�clar� hors service au niveau du pr�sent �cran.
					-# <tt>Date du dernier contr�le positif</tt> : Indique la derni�re date connue de contr�le mat�riel positif
					-# <tt>Motif de l'alerte</tt> : Texte explicatif sur l'erreur relev�e
					
			<i>S�curit�</i> :
				- Une habilitation publique DisplayMaintenanceRight de niveau READ est n�cessire pour afficher la page en mode consultation. L'emplacement de l'afficheur doit entrer dans le p�rim�tre de l'habilitation.
				- Une habilitation publique DisplayMaintenanceRight de niveau WRITE est n�cessire pour afficher la page en mode modification. L'emplacement de l'afficheur doit entrer dans le p�rim�tre de l'habilitation. 

			<i>Journal</i> : Les actions suivantes g�n�rent une entr�e dans le journal de maintenance des afficheurs DisplayMaintenanceLog :
				- INFO : Modification de param�tre
				- WARNING : D�tection d'une erreur de coh�rence de donn�es lors de l'affichage de la page
				- ERROR : Passage en hors service
				- WARNING : premi�re constatation d'une absence d'entr�e de type contr�le sur un afficheur dans un d�lai compris entre 150% et 500% de la dur�e pr�suppos�e entre deux contr�les. 
				- ERROR : premi�re constatation d'une absence d'entr�e de type contr�le sur un afficheur dans un d�lai sup�rieur � 500% de la dur�e pr�suppos�e entre deux contr�les. 
				- NB : Ces deux entr�es apparaissent � la premi�re visualisation d'un probl�me de ce type dans une console d'administration, afin de suppl�er � l'absence de d�mon de surveillance. Un passage en contr�le continu avec alerte pourrait �tre impl�ment�.
				- NB : Des entr�es INFO/WARNING/ERROR sont entr�es dans le journal de maintenance directement par les clients de supervision via une requ�te SendLogEntry.
			
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
