
#ifndef SYNTHESE_CRESULTATBASEDEDONNEES_H
#define SYNTHESE_CRESULTATBASEDEDONNEES_H

#include "cResultatBaseDeDonnees_Cellule.h"
#include "Parametres.h"
#include <map>

typedef cTableauDynamiquePointeurs<cResultatBaseDeDonnees_Cellule*> tResultatBaseDeDonnees_Enregistrement;
typedef map<char*, tIndex>											tPositionChamps;

/** @ingroup m02 */
class cResultatBaseDeDonnees
{
protected:
	tPositionChamps													_PositionChamps;	//!< Tableau de positionnement des champs dans les enregistrements
	cTableauDynamiqueObjets<tResultatBaseDeDonnees_Enregistrement>	_Donnees;				//!< Données résultat
	tIndex															_EnregistrementCourant;	//!< Enregistrement à lire par défaut

public:
	//!	\name Accès aux données
	//@{
	int		getNombre	(const cTexte& __Champ, tIndex __Index = INCONNU);
	cTexte	getTexte	(const cTexte& __Champ, tIndex __Index = INCONNU);
	cMoment	getMoment	(const cTexte& __Champ, tIndex __Index = INCONNU);
	tBool3	getBool		(const cTexte& __Champ, tIndex __Index = INCONNU);
	bool	GoPremier();
	bool	GoSuivant();
	//@}

	//!	\name Autres accesseurs
	//@{
	tIndex						Taille()				const;
	const map<char*, tIndex>&	getPositionsChamps()	const;
	//@}
	
	//!	\name Modificateurs
	//@{
	void	Vide();
	bool	SetPositionChamp(char* __NomChamp, tIndex __Position);
	bool	AddEnregistrement();
	bool	SetValeur(tIndex __Position, cResultatBaseDeDonnees_Cellule* __Cellule);
	//@}

	cResultatBaseDeDonnees() { Vide(); }
};



#endif
