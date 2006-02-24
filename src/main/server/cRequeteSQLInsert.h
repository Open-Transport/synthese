
#ifndef SYNTHESE_CREQUETESQLINSERT_H
#define SYNTHESE_CREQUETESQLINSERT_H

#include "cTexte.h"
#include "cTexteSQL.h"

/*!	\brief Classe de texte restinée à recevoir une requête SQL d'insertion
	@ingroup m02

Le but de cette classe est de permettre la construction d'une requête INSERT de manière claire et lisible.
*/
class cRequeteSQLInsert : public cTexte
{
protected:
	cTexte	vNomTable;		//!< Table dans laquelle doit se faire l'insertion
	cTexte	vListeChamps;		//!< Liste des champs de la table à renseigner
	cTexte	vListeValeurs;		//!< Liste des valeurs que doivent prendre les champs
	
public:
	//! \name Modificateurs
	//@{
	template <class T> 
	void AddChamp(const cTexte& NomChamp, const T& Valeur, const bool Fonction=false);
	//@}	
	
	//! \name Constructeurs
	//@{
	cRequeteSQLInsert(const cTexte& NomTable);
	//@}
};




/*!	\brief Ajout d'un champ à la requête INSERT
	\param NomChamp Nom du champ
	\param Fonction Indique si la valeur à donner au champ est une fonction à exécuter telle quelle (true)
	\param  Valeur Valeur à donner au champ :
		- Si Fonction vaut true alors le paramètre est considéré comme une fonction SQL et sera copié tel quel à partir du second caractère
		- Sinon, la valeur est mise au format "chaîne de caractères SQL" : mise entre guillemets et nettoyée de caractères interdits
		Ex: Toto => "Toto"
		Ex: Il a dit "Super\20"; et il est parti. => "Il a dit 'Super\\20', et il est parti"
	\warning Si Fonction=true, il est nécessaire de s'assurer que Valeur ne comporte pas de danger, notamment si tout ou partie du paramètre est fourni par un utilisateur
*/
template <class T> 
inline void cRequeteSQLInsert::AddChamp(const cTexte& NomChamp, const T& Valeur, const bool Fonction)
{
	// Ajout des virgules si il s'agit de la seconde modification au moins
	if (vListeChamps.Taille())
	{
		vListeChamps << ", ";
		vListeValeurs << ", ";
	}
	
	// Mise à jour des listes
	vListeChamps << NomChamp;
	if (Fonction)
		vListeValeurs << Valeur;
	else
	{
		cTexteSQL Parametre;
		Parametre << Valeur;
		vListeValeurs << "\"" << Parametre << "\"";
	}
	
	// Mise à jour de la chaîne résultat
	Vide();
	*this << "INSERT INTO " << vNomTable << "(" << vListeChamps << ") VALUES(" << vListeValeurs << ");";
}



#endif
