
#ifndef SYNTHESE_SECURITY_RIGHT_H
#define SYNTHESE_SECURITY_RIGHT_H

namespace synthese
{
	namespace security
	{
		/** Habilitation.
			@ingroup m05
		
			Une habilitation est un droit d'effectuer une ou plusieurs op�ration(s) sur un p�rim�tre donn�.

			Une habilitation est d�finie par :
				-# un module d'application :
					- L'un des namespace de SYNTHESE
					- @c * = Tous les modules
				-# une ou plusieurs op�rations
					- L'une des fonctions publiques SYNTHESE du module s�lectionn�
					- @c * = Toutes les fonctions SYNTHESE du module s�lectionn�
				-# un p�rim�tre d'application qui pourra �tre interpr�t� par la fonction SYNTHESE. Exemples :
					- Masque de code SYNTHESE d'objet (MP/TLS/*, MP/TLS/14/*...)
					- Liste d'objet(s) (MP/TLS/14;MP/TLS/41)
					- @c * = Pas de restriction de p�rim�tre
					- @c belong = Crit�re d'appartenance : droit applicable uniquement sur les objets appartenant � l'utilisateur (Ex : possibilit� d'annuler uniquement une r�servation au nom de l'utilisateur)
				-# le droit obtenu. Chaque niveau de droit inclut les niveaux pr�c�dents.
					- @c use : Utilisation de fonction
					- @c read : Lecture des donn�es
					- @c write : Ecriture de donn�es
					- @c delete : Effacement de donn�es
		*/
		class Right
		{
		};
	}
}