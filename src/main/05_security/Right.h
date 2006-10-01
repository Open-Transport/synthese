
#ifndef SYNTHESE_SECURITY_RIGHT_H
#define SYNTHESE_SECURITY_RIGHT_H

namespace synthese
{
	namespace security
	{
		/** Habilitation.
			@ingroup m05
		
			Une habilitation est un droit d'effectuer une ou plusieurs opération(s) sur un périmètre donné.

			Une habilitation est définie par :
				-# un module d'application :
					- L'un des namespace de SYNTHESE
					- @c * = Tous les modules
				-# une ou plusieurs opérations
					- L'une des fonctions publiques SYNTHESE du module sélectionné
					- @c * = Toutes les fonctions SYNTHESE du module sélectionné
				-# un périmètre d'application qui pourra être interprété par la fonction SYNTHESE. Exemples :
					- Masque de code SYNTHESE d'objet (MP/TLS/*, MP/TLS/14/*...)
					- Liste d'objet(s) (MP/TLS/14;MP/TLS/41)
					- @c * = Pas de restriction de périmètre
					- @c belong = Critère d'appartenance : droit applicable uniquement sur les objets appartenant à l'utilisateur (Ex : possibilité d'annuler uniquement une réservation au nom de l'utilisateur)
				-# le droit obtenu. Chaque niveau de droit inclut les niveaux précédents.
					- @c use : Utilisation de fonction
					- @c read : Lecture des données
					- @c write : Ecriture de données
					- @c delete : Effacement de données
		*/
		class Right
		{
		};
	}
}