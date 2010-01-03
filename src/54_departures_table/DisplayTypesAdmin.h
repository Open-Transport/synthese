
/** DisplayTypesAdmin class header.
	@file DisplayTypesAdmin.h

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

#ifndef SYNTHESE_DISPLAY_TYPES_ADMIN_H
#define SYNTHESE_DISPLAY_TYPES_ADMIN_H

#include "AdminInterfaceElementTemplate.h"
#include "ResultHTMLTable.h"

namespace synthese
{
	namespace departurestable
	{

		/** Ecran d'administration des types d'affichage.
			@ingroup m54Admin refAdmin

			@image html cap_admin_display_types.png
			@image latex cap_admin_display_types.png "Ecran d'�dition des types d'affichage" width=14cm

			<i>Titre de la fen�tre</i> : SYNTHESE Admin - Afficheurs - Types

			<i>Barre de navigation</i> :
 				- Lien vers @ref synthese::interface::HomeAdmin
				- Lien vers @ref synthese::interface::DisplaySearchAdmin

			<i>Zone de contenus</i> :
				-# <b>Tableau liste des types</b> : Les colonnes suivantes sont pr�sentes, et permettent d'�diter directement les types d'afficheurs :
					- <tt>Nom</tt> : Texte devant �tre non vide
					- <tt>Interface</tt> : Liste de choix pr�sentant les interfaces install�es (exemple : Lumiplan, HTML charte 1, HTML charte 2, etc.)
					- <tt>Lignes</tt> : Nombre de d�parts affich�s sous forme de liste de choix proposant les nombres de 1 � 50
					- <tt>Actions</tt> : Boutons : 
						- <tt>Ouvrir</tt> : enregistre les modifications effectu�es sur la ligne du tableau correspondante. Si un champ ne respecte pas les r�gles �num�r�es, un message d'erreur emp�che l'enregistrement des modifications :
						@code Le type ne peut �tre modifi� car un ou plusieurs champ n'est pas correctement renseign� @endcode
						- <tt>Supprimer</tt> : supprime le type d'afficheur, seulement s'il n'est utilis� par aucun afficheur. Ce bouton n'appara�t que sur les types non utilis�s. Un contr�le d'int�grit� est r�alis� � nouveau au moment de la suppression proprement dite. En cas d'existence d'afficheur au moment de la suppression, un message d'erreur appara�t :
						@code Ce type d'afficheur ne peut �tre supprim� car il est utilis� par au moins un afficheur. @endcode
					- La derni�re ligne du tableau permet l'ajout de type d'afficheur : le bouton <tt>Ajouter</tt> permet d'ajouter un type d'afficheur directement. Un contr�le des donne�s entr�es est effectu� : en cas de donn�es manquante, l'afficheur n'est pas cr�� et un message d'erreur appara�t :
						@code L'afficheur ne peut �tre cr�� car un ou plusieurs champs n'a pas �t� correctement renseign�. @endcode
						
			<i>S�curit�</i> :
				- Une habilitation publique ArrivalDepartureTableRight de niveau READ sur le p�rim�tre "tout" est n�cessaire pour acc�der � la page.
				- Une habilitation publique ArrivalDepartureTableRight de niveau WRITE sur le p�rim�tre "tout" est n�cessaire pour obtenir les boutons <tt>Modifier</tt> et <tt>Ajouter</tt>.
				
			<i>Journal</i> : Les actions suivantes g�n�rent une entr�e dans le journal ArrivalDepartureTableLog :
				- INFO : Ajout de type d'afficheur
				- INFO : Modification de type d'afficheur
				- INFO : Suppression de type d'afficheur
		*/
		class DisplayTypesAdmin : public admin::AdminInterfaceElementTemplate<DisplayTypesAdmin>
		{
			boost::optional<std::string>				_searchName;
			boost::optional<util::RegistryKeyType>		_searchInterfaceId;
			html::ResultHTMLTable::RequestParameters	_requestParameters;
			
		public:
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_INTERFACE_ID;
			
			
			
			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			virtual void setFromParametersMap(
				const server::ParametersMap& map
			);

			/** Parameters map generator, used when building an url to the admin page.
				@return server::ParametersMap The generated parameters map
				@author Hugues Romain
				@date 2007					
			*/
			virtual server::ParametersMap getParametersMap() const;
			

			/** Display of the content of the admin element.
				@param stream Stream to write on.
			*/
			void display(
				std::ostream& stream,
				interfaces::VariablesMap& variables,
				const admin::AdminRequest& _request
			) const;

			bool isAuthorized(
				const security::User& user
			) const;

			/** Gets sub page of the designed parent page, which are from the current class.
				@param factoryKey Key of the parent class
				@return PageLinks A link to the page if the parent is DisplaySearch
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const std::string& moduleKey,
				const AdminInterfaceElement& currentPage,
				const admin::AdminRequest& request
			) const;

			virtual PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const admin::AdminRequest& request
			) const;

			/** Gets the opening position of the node in the tree view.
				@return Always visible
				@author Hugues Romain
				@date 2008					
			*/
			virtual bool isPageVisibleInTree(
				const AdminInterfaceElement& currentPage,
				const admin::AdminRequest& request
			) const;

		};
	}
}

#endif // SYNTHESE_DisplayTypesAdmin_H__
