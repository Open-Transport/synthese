
#ifndef SYNTHESE_CHEURE_H
#define SYNTHESE_CHEURE_H

#include "Temps.h"

/** Heure
	@ingroup m04
*/
class cHeure
{
	tHeure  vHeures;	//!< Heures
	tMinute vMinutes;	//!< Minutes

public :
	//! \name Accesseurs
	//@{
	tMinute	Minutes()		const;
	tHeure	Heures()		const;
	bool		EstInconnu()	const;
	//@}
	
	//! \name Affichage
	//@{
	/*template <class T> T& toXML(T& Objet) const
	{
		Objet << " heures=\"" << TXT(vHeures) << "\" minutes=\"" << TXT2(vMinutes, 2) << "\"";
		return(Objet);
	}*/

	template <class T> T& toStringAvec0(T& Objet) const
	{
		Objet << TXT2(vHeures, 2) << ":" << TXT2(vMinutes, 2);
		return(Objet);
	}
	//@}

	
	//! \name Modificateurs
	//@{
	void			setHeure(tHeure newHeure = 'A', tMinute newMinutes = 'I');
	tDureeEnJours	operator += (const cDureeEnMinutes&);
	tDureeEnJours	operator -= (const cDureeEnMinutes&);
//	cHeure&		operator =  (int);
	cHeure&		operator =  (const cTexte&);
	cHeure&		operator =  (const cHoraire&);
	//@}

	//! \name Calculateurs
	//@{
	int				operator == (const cHeure&)	const;
	int				operator != (const cHeure&)	const;
	int				operator <= (const cHeure&)	const;
	int				operator <  (const cHeure&)	const;
	int				operator >= (const cHeure&)	const;
	int				operator >  (const cHeure&)	const;
	cDureeEnMinutes	operator -  (const cHeure&) 	const;
	bool				OK() 					const;
	//@}

	//!	\name Constructeur
	//@{
	cHeure() { vHeures = INCONNU; vMinutes = INCONNU; }
	~cHeure() {}
	//@}
};

/*!	\brief Contrôle de la validité des valeurs entrées dans l'objet
	\author Hugues Romain
*/
inline bool cHeure::OK() const
{
	return(vHeures >= 0 && vHeures < HEURES_PAR_JOUR && vMinutes >= 0 && vMinutes < MINUTES_PAR_HEURE);
}

inline int cHeure::operator == (const cHeure& Op2) const
{
	return(vHeures == Op2.vHeures && vMinutes == Op2.vMinutes);
}

inline int cHeure::operator != (const cHeure& Op2) const
{
	return(vHeures != Op2.vHeures || vMinutes != Op2.vMinutes);
}

inline int cHeure::operator <= (const cHeure &Op2) const
{
	return(vHeures < Op2.vHeures || (vHeures == Op2.vHeures && vMinutes <= Op2.vMinutes));
}

inline int cHeure::operator<(const cHeure &Op2) const
{
	return(vHeures < Op2.vHeures || vHeures == Op2.vHeures && vMinutes < Op2.vMinutes);
}

inline int cHeure::operator>=(const cHeure &Op2) const
{
	return(vHeures > Op2.vHeures || vHeures == Op2.vHeures && vMinutes >= Op2.vMinutes);
}

inline int	cHeure::operator > (const cHeure &Op2) const
{
	return(vHeures > Op2.vHeures || (vHeures == Op2.vHeures && vMinutes > Op2.vMinutes));
}


/*
inline cHeure& cHeure::operator =(int curNombre)
{
	vHeures = (tHeure) (curNombre / 100);
	vMinutes  = (tMinute) (curNombre % 100);
	return(*this);
}
*/

inline tHeure cHeure::Heures() const
{
	return vHeures;
}

inline tMinute cHeure::Minutes() const
{
	return vMinutes;
}



/*!	\brief Test l'heure est-elle inconnue
	\return true si l'heure est inconnue, false sinon
	\author Hugues Romain
	\date 2005
*/
inline bool cHeure::EstInconnu() const
{
	return Heures() == INCONNU || Minutes() == INCONNU;
}



/*!	\brief Affichage standard d'une heure
	\param flux Flux sur lequel est affichée l'heure
	\param Obj L'heure à afficher
	\author Hugues Romain
	\date 2005
*/
template <class T>
inline T& operator<<(T& flux, const cHeure& Obj)
{
	flux << cTexte(5).Copie(Obj.Heures()).Copie(":").Copie(Obj.Minutes(),2);
	return flux;
}



/*!	\brief Codage texte interne d'une heure
	\param flux Flux sur lequel est codée l'heure
	\param Obj L'heure à coder
	\author Hugues Romain
	\date 2005
*/
template <>
inline cTexteCodageInterne& operator<< <cTexteCodageInterne>(cTexteCodageInterne& flux, const cHeure& Obj)
{
	flux << cTexte(4).Copie(Obj.Heures(), 2).Copie(Obj.Minutes(),2);
	return flux;
}

#endif
