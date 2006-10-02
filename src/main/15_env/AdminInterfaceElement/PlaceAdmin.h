
#ifndef SYNTHESE_ENV_PLACE_ADMIN_H
#define SYNTHESE_ENV_PLACE_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran d'édition d'emplacement.
			@ingroup m15

			@image html cap_admin_place.png
			@image latex cap_admin_place.png "Maquette de l'écran de recherche de lieux" width=14cm
			
			<i>Titre de la fenêtre</i> :
				- SYNTHESE Admin - Emplacements - [Commune] [Nom]

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminHome
				- Lien vers synthese::interfaces::PlacesAdmin
				- Texte [Commune] [Nom]

			<i>Zone de contenu</i> : <b>Formulaire d'édition</b> :
				-# <b>Emplacements physiques</b>
					-# <tt>Arrêts physiques</tt> : Les arrêts physiques sont rappelés ici pour éviter de créer des emplacements existants. Ils ne sont pas modifiables car proviennent des données de démarrage.
					-# <tt>Emplacements d'affichage</tt> : Tableau correspondant aux emplacements d'affichage non liés à des arrêts physiques (ex: salle d'attente). Chaque emplacement est désigné par son nom. Un formulaire permet d'éditer directement chaque emplacement :
						-# <tt>Nom</tt> : Champ texte obligatoire.
						-# Bouton <tt>Renommer</tt> : Change le nom de l'emplacement par celui entré. En cas de champ nom vide, le renommage est abandonné et un message d'erreur apparait :
							@code L'emplacement ne peut être renommé par un nom vide @code
							En outre, le nom d'un emplacement doit être unique au sein du lieu logique, arrêts physiques inclus. En cas de renommage avec un nom existant, il est abandonné et un message d'erreur apparait :
							@code L'emplacement ne peut être renommé par le nom spécifié car il est déjà utilisé. @endcode
						-# Bouton <tt>Supprimer</tt> : Supprime l'emplacement, seulement si celui-ci n'est utilisé par aucun afficheur. Le bouton supprimer n'apparait pas dans un tel cas. Lors de la suppression ce critère d'intégrité est à nouveau controlé. En cas d'échec un message d'erreur apparait :
							@code Cet emplacement ne peut être supprimé car au moins un terminal d'afficheur y est déclaré. @endcode
						-# Bouton <tt>Ajouter</tt> : Comportement identique au bouton <tt>Renommer</tt>, mais sur un emplacement nouveau.
			
			<i>Sécurité</i>
				- Une habilitation de niveau lecture sur le module environnement et sur l'opération emplacements est nécessaire pour afficher la page en consultation. Le périmètre doit spécifier soit la commune du lieu logique, soit le lieu logique lui-même, soit une ligne qui le dessert.
				- Une habilitation de niveau écriture sur le module environnement et sur l'opération emplacements est nécessaire pour effectuer des modification. Le périmètre doit spécifier soit la commune du lieu logique, soit le lieu logique lui-même, soit une ligne qui le dessert.

			<i>Journaux</i> : Les événements suivants entrainent la création d'une entrée dans le journal de l'administration des données réseau de l'environnement :
				- Renommage d'emplacement
				- Suppression d'emplacement
				- Création d'emplacement

		*/
		class PlaceAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif