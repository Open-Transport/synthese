
#ifndef SYNTHESE_CMOMENT_H
#define SYNTHESE_CMOMENT_H

#include "Temps.h"
#include "cDate.h"
#include "cHeure.h"
#include "cHoraire.h"

/** Moment (date + heure)
	@ingroup m04
*/
class cMoment
{
	cHeure vHeure;	//!< Heure
	cDate  vDate;	//!< Date

public:
	
	//! \name Modificateurs
	//@{
	cMoment& addDureeEnJours(tDureeEnJours Val=1);
	cMoment& setMoment(tJour Jour='A', tMois Mois='I', tAnnee Annee='I', tHeure Heures='I', tMinute Minutes='I');
	cMoment& setDate(tJour Jour='A', tMois Mois='I', tAnnee Annee='I');
	cMoment& setHeure(tHeure newHeures='I', tMinute newMinute='I');
	cMoment& setHeure(const cTexte&);
	cMoment& setHeure(const cHeure&);
	cMoment& subDureeEnJours(tDureeEnJours Val);
	cMoment& operator ++ 	(int);
	cMoment& operator -- 	(int);
	cMoment& operator += 	(const tDureeEnMinutes&);
	cMoment& operator -= 	(const tDureeEnMinutes&);
	cMoment& operator = 	(const cTexte&);
	cMoment& operator =		(const cDate&);
	cMoment& operator =		(const cHeure&);
	cMoment& operator =		(const cHoraire&);
	//@}
	
	//! \name Accesseurs
	//@{
	const cDate&	getDate()		const;
	const cHeure&	getHeure() 	const;
	tAnnee		Annee()		const;
	tJour		Jour()		const;
	tMois		Mois()		const;
	tHeure		Heures()		const;
	tMinute		Minutes()		const;
	//@}
	
	//! \name Affichage
	//@{
	template <class T> T& toStringAvec0(T& Objet) const
	{
		vDate.toStringAvec0(Objet);
		Objet << " à ";
		vHeure.toStringAvec0(Objet);
		return(Objet);
	}

	/*template <class T> T& toXML(T& Objet, const char* balise) const
	{
		Objet << "\n<" << balise;
		vDate.toXML(Objet);
		vHeure.toXML(Objet);
		Objet << "/>";
		return(Objet);
	}*/
	//@}

	//! \name Calculateurs
	//@{
	int			   	operator ==		(const cMoment&)		const;
	int			   	operator != 	(const cMoment&)		const;
	int			   	operator <= 	(const cMoment&)		const;
	int			   	operator <= 	(const cHoraire&)		const;
	int			   	operator <  	(const cMoment&)		const;
	int			   	operator <  	(const cHoraire&)		const;
	int			   	operator >= 	(const cMoment&)		const;
	int			   	operator >= 	(const cHoraire&)		const;
	int			   	operator >  	(const cMoment&)		const;
	int			   	operator >  	(const cHoraire&)		const;
	tDureeEnMinutes	operator -  	(const cMoment&)		const;
	bool			OK				()						const;
	//@}
	
	explicit cMoment(tJour Jour='A', tMois Mois='I', tAnnee Annee='I', tHeure Heures='I', tMinute Minutes='I') { setMoment(Jour, Mois, Annee, Heures, Minutes); }
	cMoment(const cDate& date) { operator=(date); }
	~cMoment() {}
};

inline int cMoment::operator == (const cMoment& Op2) const
{
	return(vDate == Op2.vDate && vHeure == Op2.vHeure);
}

inline int cMoment::operator != (const cMoment& Op2) const
{
	return(vDate != Op2.vDate || vHeure != Op2.vHeure);
}

inline int cMoment::operator<=(const cMoment &AutreMoment) const
{
	return(vDate < AutreMoment.vDate || vDate == AutreMoment.vDate && vHeure <= AutreMoment.vHeure);
}

inline int cMoment::operator < (const cMoment &AutreMoment) const
{
	return(vDate < AutreMoment.vDate || vDate == AutreMoment.vDate && vHeure < AutreMoment.vHeure);
}

inline int cMoment::operator<=(const cHoraire &AutreHoraire) const
{
	return(AutreHoraire >= vHeure);
}

inline int cMoment::operator<(const cHoraire &AutreHoraire) const
{
	return(AutreHoraire > vHeure);
}

inline int cMoment::operator>=(const cHoraire &AutreHoraire) const
{
	return(AutreHoraire <= vHeure);
}

inline int cMoment::operator>(const cHoraire &AutreHoraire) const
{
	return(AutreHoraire < vHeure);
}

inline int cMoment::operator>=(const cMoment& AutreMoment) const
{
	return(vDate > AutreMoment.vDate || vDate == AutreMoment.vDate && vHeure >= AutreMoment.vHeure);
}

inline int cMoment::operator > (const cMoment &AutreMoment) const
{
	return(vDate > AutreMoment.vDate || (vDate == AutreMoment.vDate && vHeure > AutreMoment.vHeure));
}

inline cMoment& cMoment::operator += (const tDureeEnMinutes& Op2)
{
	vDate += (vHeure += Op2);
	return(*this);
}

inline cMoment& cMoment::operator -= (const tDureeEnMinutes& Op2)
{
	vDate -= (vHeure -= Op2);
	return(*this);
}

/*!	\brief Ajoute un jour au moment
	\author Hugues Romain
*/
inline cMoment& cMoment::operator ++(int)
{
	vDate++;
	return(*this);
}

/*!	\brief Enlève un jour au moment
	\author Hugues Romain
*/
inline cMoment& cMoment::operator --(int)
{
	vDate--;
	return(*this);
}

/*! 	\brief Récupère l'heure de l'horaire dans le moment sans modifier la date
	\return L'objet lui-même
	\author Hugues Romain
*/
inline cMoment& cMoment::operator = (const cHoraire& Autre)
{
	vHeure = Autre.getHeure();
	return(*this);
}

/*!	\brief Contrôle de la validité des valeurs entrées dans l'objet
	\author Hugues Romain
*/
inline bool cMoment::OK() const
{
	return(vDate.OK() && vHeure.OK());
}

inline cMoment& cMoment::operator = (const cDate& Autre)
{
	vDate = Autre;
	return(*this);
}

inline cMoment& cMoment::operator = (const cHeure& Autre)
{
	vHeure = Autre;
	return(*this);
}

inline tJour cMoment::Jour() const
{
	return(vDate.Jour());
}

inline tMinute cMoment::Mois() const
{
	return(vDate.Mois());
}

inline tAnnee cMoment::Annee() const
{
	return(vDate.Annee());
}

inline tMinute cMoment::Minutes() const
{
	return(vHeure.Minutes());
}

inline tHeure cMoment::Heures() const
{
	return(vHeure.Heures());
}

inline const cDate& cMoment::getDate() const
{
	return(vDate);
}

inline cMoment& cMoment::setDate(tJour Jour, tMois Mois, tAnnee Annee)
{
	vDate.setDate(Jour, Mois, Annee);
	return(*this);
}

inline const cHeure& cMoment::getHeure() const
{
	return(vHeure);
}

inline cMoment& cMoment::setHeure(tHeure newHeures, tMinute newMinute)
{
	vHeure.setHeure(newHeures, newMinute);
	return(*this);
}


inline cMoment& cMoment::addDureeEnJours(tDureeEnJours Val)
{
	vDate += Val;
	return *this;
}

inline cMoment& cMoment::subDureeEnJours(tDureeEnJours Val)
{
	vDate -= Val;
	return(*this);
}

inline cMoment& cMoment::setMoment(tJour Jour, tMois Mois, tAnnee Annee, tHeure Heures, tMinute Minutes)
{
  vDate.setDate(Jour, Mois, Annee);
  if (Heures == 'I')
  	vHeure.setHeure(Jour, Minutes);
else
  vHeure.setHeure(Heures, Minutes);

  return(*this);	
}

inline int cDate::operator <= (const cMoment& Autre) const
{
	return(*this <= Autre.getDate());
}

inline cDate& cDate::operator = (const cMoment& op)
{
	return(*this = op.getDate());
}

template <class T> 
inline T& operator<<(T& flux, const cMoment& Obj)
{
	cTexte __Sortie;
	__Sortie << Obj.getDate() << " " << Obj.getHeure();
	flux << __Sortie;
	return flux;
}

template <> 
inline cTexteCodageInterne& operator<< <cTexteCodageInterne>(cTexteCodageInterne& flux, const cMoment& Obj)
{
	flux << Obj.getDate() << Obj.getHeure();
	return flux;
}

#endif
