
#ifndef SYNTHESE_CRESULTATBASEDEDONNEES_CELLULE_H
#define SYNTHESE_CRESULTATBASEDEDONNEES_CELLULE_H

#include "cTexte.h"
#include "cMoment.h"

/** @ingroup m02 */
class cResultatBaseDeDonnees_Cellule
{
protected:
	
public:
	virtual int		getNombre()	const { return INCONNU; }
	virtual cTexte	getTexte()	const { cTexte __Texte; return __Texte; }
	virtual cMoment	getMoment()	const { return cMoment(); }
	virtual tBool3	getBool()	const { return Indifferent; }
};



/** @ingroup m02 */
class cResultatBaseDeDonnees_Cellule_Nombre : public cResultatBaseDeDonnees_Cellule
{
protected:
	int	_Valeur;	//!< Valeur entière

public:

	//!	\name Accesseurs
	//@{
	int		getNombre()		const { return _Valeur; }
	cTexte	getTexte()		const { cTexte __Valeur; __Valeur << _Valeur; return __Valeur; }
	tBool3	getBool()		const { return _Valeur == INCONNU ? Indifferent : (_Valeur > 0 ? Vrai : Faux); }
	//@}

	/*!	\brief Constructeur
		CRO : Ce constructeur peut être modifié en fonction des besoins
	*/
	cResultatBaseDeDonnees_Cellule_Nombre(int __Valeur) { _Valeur = __Valeur; }
};



/** @ingroup m02 */
class cResultatBaseDeDonnees_Cellule_Texte : public cResultatBaseDeDonnees_Cellule
{
protected:
	cTexte	_Valeur;	//!< Valeur texte

public:

	//!	\name Accesseurs
	//@{
	int		getNombre()	const { return _Valeur.GetNombre(); }
	cTexte	getTexte()		const { return _Valeur; }
	//@}

	/*!	\brief Constructeur
		CRO : Ce constructeur peut être modifié en fonction des besoins
	*/
	cResultatBaseDeDonnees_Cellule_Texte(const cTexte& __Valeur) { _Valeur = __Valeur; }
};



/** @ingroup m02 */
class cResultatBaseDeDonnees_Cellule_Moment : public cResultatBaseDeDonnees_Cellule
{
protected:
	cMoment	_Valeur;	//!< Valeur moment

public:

	//!	\name Accesseurs
	//@{
	cMoment	getMoment()	const { return _Valeur; }
	//@}

	/*!	\brief Constructeur
	*/
	cResultatBaseDeDonnees_Cellule_Moment(const cMoment& __Valeur) { _Valeur = __Valeur; }
};

#endif
