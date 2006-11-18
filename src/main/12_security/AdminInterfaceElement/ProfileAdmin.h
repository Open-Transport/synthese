
#ifndef SYNTHESE_SECURITY_PROFILE_H
#define SYNTHESE_SECURITY_PROFILE_H

namespace synthese
{
	namespace interfaces
	{
		/** Ecran d'édition de profil.
			@ingroup m05
		
			@image html cap_admin_profile.png
			@image latex cap_admin_profile.png "Maquette d'écran d'édition de profil" width=14cm

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::HomeAdmin
				- Lien vers synthese::interfaces::ProfilesAdmin
				- Nom du profil

			<i>Zone de contenus</i> :
				-# Champ de <b>modification du nom</b>, accompagné d'un bouton <tt>Supprimer</tt>. Champ obligatoire. La tentative d'enregistrement d'un nom vide produit le message d'erreur suivant :
					@code Un profil doit posséder un nom @endcode
					Le nom doit être unique. En cas de doublon, la modification du nom est refusée et le message d'erreur suivant s'affiche :
					@code Un profil portant le même nom existe déjà @endcode
				-# <b>Tableau des habilitations</b> : représente chaque habilitation définissant le profil, par les colonnes suivantes :
					-# <tt>Module</tt> : Module sur lequel porte l'habilitation (Tous = tous les modules à la fois).
					-# <tt>Opération</tt> : Opération du module sur laquelle porte l'habilitation (Tous = toutes les opérations du modules, y compris administration)
					-# <tt>Périmètre</tt> : Texte représentant le périmètre sur lequel porte l'habilitation, dont le formalisme dépend du module et de l'opération. Un caractère * signifie "peut être remplacé par tout texte".
					-# <tt>Droit</tt> : Droit obtenu pour le module, l'opération, et le périmètre de l'habilitation
					-# <tt>Actions</tt> : contient un bouton supprimer pour chaque habilitation, permettant de supprimer la ligne du tableau. L'appui sur le bouton entraine une confirmation par boite de dialogue standard. La suppression est ensuite définitive.
				-# <b>Formulaire d'ajout d'habilitation</b> :
					-# <tt>Module</tt> : Liste de choix contenant tous les modules de SYNTHESE, plus un choix <tt>Tous</tt> placé en tête et proposé par défaut. Le changement de valeur sur la liste entraine le rafraichissement de la page pour mettre à jour la liste suivante.
					-# <tt>Opération</tt> : Liste des opérations disponibles pour le module sélectionné, plus un choix <tt>Toutes</tt> placé en tête et proposé par défaut. Le changement de valeur sur la liste entraîne le rafraichissement de la page pour mettre à jour la liste suivante.
					-# <tt>Périmètre</tt> : Liste des périmètres possibles pour l'opération sélectionnées, plus un choix <tt>Tout</tt> placé en tête et proposé par défaut.
					-# <tt>Droit</tt> : Liste des niveaux de droit possibles :
						- Utilisation
						- Lecture
						- Modification
						- Ecriture
						- Suppression
					-# <tt>Ajouter</tt> : bouton d'ajout d'habilitation. L'ajout se fait directement sans confirmation.
			
			<i>Sécurité</i>
				- Une habilitation SecurityRights de niveau public WRITE est nécessaire pour accéder à la page, pour laquelle le profil demandé est inférieur ou égal.
				
			<i>Journaux</i> : Les opérations suivantes sont consignées dans le journal de sécurité SecurityLog :
				- INFO : Modification du nom du profil
				- INFO : Ajout d'habilitation au profil
				- INFO : Suppression d'habilitation au profil
		*/
		class ProfileAdmin : public AdminInterfaceElement
		{
		};
	}
}