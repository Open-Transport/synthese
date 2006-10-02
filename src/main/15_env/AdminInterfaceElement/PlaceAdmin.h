
#ifndef SYNTHESE_ENV_PLACE_ADMIN_H
#define SYNTHESE_ENV_PLACE_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran d'�dition d'emplacement.
			@ingroup m15

			@image html cap_admin_place.png
			@image latex cap_admin_place.png "Maquette de l'�cran de recherche de lieux" width=14cm
			
			<i>Titre de la fen�tre</i> :
				- SYNTHESE Admin - Emplacements - [Commune] [Nom]

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminHome
				- Lien vers synthese::interfaces::PlacesAdmin
				- Texte [Commune] [Nom]

			<i>Zone de contenu</i> : <b>Formulaire d'�dition</b> :
				-# <b>Emplacements physiques</b>
					-# <tt>Arr�ts physiques</tt> : Les arr�ts physiques sont rappel�s ici pour �viter de cr�er des emplacements existants. Ils ne sont pas modifiables car proviennent des donn�es de d�marrage.
					-# <tt>Emplacements d'affichage</tt> : Tableau correspondant aux emplacements d'affichage non li�s � des arr�ts physiques (ex: salle d'attente). Chaque emplacement est d�sign� par son nom. Un formulaire permet d'�diter directement chaque emplacement :
						-# <tt>Nom</tt> : Champ texte obligatoire.
						-# Bouton <tt>Renommer</tt> : Change le nom de l'emplacement par celui entr�. En cas de champ nom vide, le renommage est abandonn� et un message d'erreur apparait :
							@code L'emplacement ne peut �tre renomm� par un nom vide @code
							En outre, le nom d'un emplacement doit �tre unique au sein du lieu logique, arr�ts physiques inclus. En cas de renommage avec un nom existant, il est abandonn� et un message d'erreur apparait :
							@code L'emplacement ne peut �tre renomm� par le nom sp�cifi� car il est d�j� utilis�. @endcode
						-# Bouton <tt>Supprimer</tt> : Supprime l'emplacement, seulement si celui-ci n'est utilis� par aucun afficheur. Le bouton supprimer n'apparait pas dans un tel cas. Lors de la suppression ce crit�re d'int�grit� est � nouveau control�. En cas d'�chec un message d'erreur apparait :
							@code Cet emplacement ne peut �tre supprim� car au moins un terminal d'afficheur y est d�clar�. @endcode
						-# Bouton <tt>Ajouter</tt> : Comportement identique au bouton <tt>Renommer</tt>, mais sur un emplacement nouveau.
			
			<i>S�curit�</i>
				- Une habilitation de niveau lecture sur le module environnement et sur l'op�ration emplacements est n�cessaire pour afficher la page en consultation. Le p�rim�tre doit sp�cifier soit la commune du lieu logique, soit le lieu logique lui-m�me, soit une ligne qui le dessert.
				- Une habilitation de niveau �criture sur le module environnement et sur l'op�ration emplacements est n�cessaire pour effectuer des modification. Le p�rim�tre doit sp�cifier soit la commune du lieu logique, soit le lieu logique lui-m�me, soit une ligne qui le dessert.

			<i>Journaux</i> : Les �v�nements suivants entrainent la cr�ation d'une entr�e dans le journal de l'administration des donn�es r�seau de l'environnement :
				- Renommage d'emplacement
				- Suppression d'emplacement
				- Cr�ation d'emplacement

		*/
		class PlaceAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif