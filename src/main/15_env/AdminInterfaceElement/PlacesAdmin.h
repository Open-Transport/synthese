
#ifndef SYNTHESE_ENV_PLACES_ADMIN_H
#define SYNTHESE_ENV_PLACES_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran de recherche de lieux pour administration.
			@ingroup m15

			@image html cap_admin_places.png
			@image latex cap_admin_places.png "Maquette de l'�cran de recherche de lieux" width=14cm
			
			<i>Titre de la fen�tre</i> :
				- SYNTHESE Admin - Emplacements

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminHome
				- Texte <tt>Emplacements</tt>

			<i>Zone de contenu</i> :
				-# <b>Formulaire de recherche</b>
					-# <tt>Commune</tt> : Champ texte permettant de rechercher les emplacements dont le nom de la commune contient le texte saisi. Le champ vide d�sactive le filtre.
					-# <tt>Nom</tt> : Champ texte permettant de rechercher les emplacements dont le nom contient le texte saisi. Le champ vide d�sactive le filtre.
					-# <tt>Terminaux d'affichage</tt> : Liste d�roulante permettant de s�lectionner l'affichage des seuls emplacements disposant d'au moins un afficheur install�, l'affichage des seuls emplacement qui n'en dispose d'aucun, ou de d�sactiver le filtre
					-# <tt>Ligne</tt> : Liste d�roulante permettant de s�lectionner une ligne de transport : seuls les emplacements desservis par la ligne sont s�lectionn�s. Si aucune ligne n'est s�lectionn� le filtre est d�sactiv�.
					-# bouton <tt>Rechercher</tt> : Lance la recherche
				-# <b>Tableau des r�sultats</b> : les emplacements trouv�s sont d�crits dans les colonnes suivantes :
					-# <tt>Commune</tt> : Nom de la commune auquel appartient l'emplacement
					-# <tt>Nom</tt> : Nom de l'emplacement
					-# Bouton <tt>Editer</tt> : Conduit vers la page synthese::interfaces::PlaceAdmin sur l'emplacement s�lectionn�
				-# Un maximum de 50 emplacements est affich� � l'�cran. En cas de d�passement de ce nombre d'apr�s les crit�res de recherche, un lien <tt>Emplacements suivants</tt> apparait et permet de visualiser les entr�es suivantes. A partir de la seconde page, un lien <tt>Emplacements pr�c�dents</tt> apparait �galement.
			
			<i>S�curit�</i>
				- Une habilitation publique PlacesRight de niveau READ est n�cessaire pour acc�der � la page et pour utiliser toutes les fonctionnalit�s.
				- Le r�sultat des recherches d�pend du p�rim�tre des habilitations de niveau READ et sup�rieur

			<i>Journaux</i>
				- Aucune action issue de ce composant d'administration ne g�n�re d'entr�e dans un journal.

		*/
		class PlacesAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif