
#ifndef SYNTHESE_CREQUETESQLINSERT_H
#define SYNTHESE_CREQUETESQLINSERT_H

#include "cTexte.h"
#include "cTexteSQL.h"

/*!	\brief Classe de texte restin�e � recevoir une requ�te SQL d'insertion
	@ingroup m02

Le but de cette classe est de permettre la construction d'une requ�te INSERT de mani�re claire et lisible.
*/
class cRequeteSQLInsert : public cTexte
{
protected:
	cTexte	vNomTable;		//!< Table dans laquelle doit se faire l'insertion
	cTexte	vListeChamps;		//!< Liste des champs de la table � renseigner
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




/*!	\brief Ajout d'un champ � la requ�te INSERT
	\param NomChamp Nom du champ
	\param Fonction Indique si la valeur � donner au champ est une fonction � ex�cuter telle quelle (true)
	\param  Valeur Valeur � donner au champ :
		- Si Fonction vaut true alors le param�tre est consid�r� comme une fonction SQL et sera copi� tel quel � partir du second caract�re
		- Sinon, la valeur est mise au format "cha�ne de caract�res SQL" : mise entre guillemets et nettoy�e de caract�res interdits
		Ex: Toto => "Toto"
		Ex: Il a dit "Super\20"; et il est parti. => "Il a dit 'Super\\20', et il est parti"
	\warning Si Fonction=true, il est n�cessaire de s'assurer que Valeur ne comporte pas de danger, notamment si tout ou partie du param�tre est fourni par un utilisateur
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
	
	// Mise � jour des listes
	vListeChamps << NomChamp;
	if (Fonction)
		vListeValeurs << Valeur;
	else
	{
		cTexteSQL Parametre;
		Parametre << Valeur;
		vListeValeurs << "\"" << Parametre << "\"";
	}
	
	// Mise � jour de la cha�ne r�sultat
	Vide();
	*this << "INSERT INTO " << vNomTable << "(" << vListeChamps << ") VALUES(" << vListeValeurs << ");";
}



#endif
