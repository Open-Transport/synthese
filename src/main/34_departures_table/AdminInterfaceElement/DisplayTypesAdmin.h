
#ifndef SYNTHESE_DISPLAY_TYPES_ADMIN_H
#define SYNTHESE_DISPLAY_TYPES_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran d'administration des types d'affichage.
			@ingroup m34

			@image html cap_admin_display_types.png
			@image latex cap_admin_display_types.png "Ecran d'�dition des types d'affichage" width=14cm

			<i>Titre de la fen�tre</i> : SYNTHESE Admin - Afficheurs - Types

			<i>Barre de navigation</i> :
 				- Lien vers synthese::interface::HomeAdmin
				- Lien vers synthese::interface::DisplaySearchAdmin

			<i>Zone de contenus</i> :
				-# <b>Tableau liste des types</b> : Les colonnes suivantes sont pr�sentes, et permettent d'�diter directement les types d'afficheurs :
					- <tt>Nom</tt> : Texte devant �tre non vide
					- <tt>Interface</tt> : Liste de choix pr�sentant les interfaces install�es (exemple : Lumiplan, HTML charte 1, HTML charte 2, etc.)
					- <tt>Lignes</tt> : Nombre de d�parts affich�s sous forme de liste de choix proposant les nombres de 1 � 50
					- <tt>Actions</tt> : Boutons : 
						- <tt>Modifier</tt> : enregistre les modifications effectu�es sur la ligne du tableau correspondante. Si un champ ne respecte pas les r�gles �num�r�es, un message d'erreur emp�che l'enregistrement des modifications :
						@code Le type ne peut �tre modifi� car un ou plusieurs champ n'est pas correctement renseign� @endcode
						- <tt>Supprimer</tt> : supprime le type d'afficheur, seulement s'il n'est utilis� par aucun afficheur. Ce bouton n'appara�t que sur les types non utilis�s. Un contr�le d'int�grit� est r�alis� � nouveau au moment de la suppression proprement dite. En cas d'existence d'afficheur au moment de la suppression, un message d'erreur appara�t :
						@code Ce type d'afficheur ne peut �tre supprim� car il est utilis� par au moins un afficheur. @endcode
					- La derni�re ligne du tableau permet l'ajout de type d'afficheur : le bouton <tt>Ajouter</tt> permet d'ajouter un type d'afficheur directement. Un contr�le des donne�s entr�es est effectu� : en cas de donn�es manquante, l'afficheur n'est pas cr�� et un message d'erreur appara�t :
						@code L'afficheur ne peut �tre cr�� car un ou plusieurs champs n'a pas �t� correctement renseign�. @endcode
						
			<i>S�curit�</i> :
				- L'habilitation Lecture sur * sur le module Tableaux de d�parts est n�cessaire pour visualiser la page en consultation
				- L'habilitation Ecriture sur * sur le module Tableaux de d�parts est n�cessaire pour �diter les types d'afficheurs, en cr�er, et en supprimer
		*/
		class DisplayTypesAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif