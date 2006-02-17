
#ifndef SYNTHESE_CHORAIRE_H
#define SYNTHESE_CHORAIRE_H

#include "Temps.h"

/** Horaires (heure + dur�e en jours depuis le d�part)
	@ingroup m04
*/
class cHoraire
{
	cHeure			vHeure;	//!< Heure
	tDureeEnJours	vJPlus;	//!< Nombre de jours pass�s depuis le d�part (l'utiliser en relatif au cas o� le d�part ait un JPlus > 0)

public:
	//! \name Accesseurs
	//@{
	const cHeure& 	getHeure()		const;
	tMinute 		Minutes() 		const;
	tHeure 			Heures() 		const;
	tDureeEnJours 	JPlus() 		const;
	//@}
	
	//! \name Modificateurs
	//@{
	cHoraire&	setMinimum();
	cHoraire&	setMaximum();
	cHoraire&	setJPlus(tDureeEnJours JPlus);
	cHoraire&	operator =  (const cTexte&);
//	cHoraire&	operator =  (int);
	cHoraire&	operator += (const tDureeEnMinutes&);
	//@}
	
	//! \name Op�rateurs
	//@{
	int				operator <  (const cHoraire&)	const;
	int				operator <  (const cHeure&)	const;
	int				operator <= (const cHoraire&)	const;
	int				operator <= (const cHeure&)	const;
	int				operator >= (const cHoraire&)	const;
	int				operator >= (const cHeure&)	const;
	int				operator >  (const cHeure&)	const;
	tDureeEnMinutes	operator -  (const cHoraire&)	const;
	bool				OK()						const;
	//@}
};

/*!	\brief Contr�le de la validit� des valeurs entr�es dans l'objet
	\author Hugues Romain
*/
inline bool cHoraire::OK() const
{
	return(vJPlus >= 0 && vJPlus <=28 && vHeure.OK());
}

/*! \brief Augmente l'horaire de "DureeAAjouter" minutes
	\return l'objet lui-m�me.
	\warning NE PREND QUE DES DUREES POSITIVES (facile � modifier)
	\author Hugues Romain 
	\date 2000/2001
	*/
inline cHoraire& cHoraire::operator += (const tDureeEnMinutes& DureeAAjouter)
{
	vJPlus = (tDureeEnJours) (vJPlus + (vHeure += DureeAAjouter));
	return(*this);
}

inline int cHoraire::operator < (const cHoraire &AutreHoraire) const
{
	return (vJPlus < AutreHoraire.vJPlus || vJPlus == AutreHoraire.vJPlus && vHeure < AutreHoraire.vHeure);
}

inline int cHoraire::operator <= (const cHoraire &AutreHoraire) const
{
	return (vJPlus < AutreHoraire.vJPlus || vJPlus == AutreHoraire.vJPlus && vHeure <= AutreHoraire.vHeure);
}

inline int cHoraire::operator >= (const cHoraire &AutreHoraire) const
{
	return (vJPlus > AutreHoraire.vJPlus || vJPlus == AutreHoraire.vJPlus && vHeure >= AutreHoraire.vHeure);
}

inline int cHoraire::operator < (const cHeure& AutreHeure) const
{
	return (vHeure < AutreHeure);
}

inline int cHoraire::operator <= (const cHeure& AutreHeure) const
{
	return (vHeure <= AutreHeure);
}

inline int cHoraire::operator > (const cHeure& AutreHeure) const
{
	return (vHeure > AutreHeure);
}

inline int cHoraire::operator >= (const cHeure& AutreHeure) const
{
	return (vHeure >= AutreHeure);
}

/*
inline cHoraire& cHoraire::operator = (int newVal)
{
	vHeure = newVal;
	return(*this);
}
*/

inline cHoraire& cHoraire::setJPlus(tDureeEnJours JPlus)
{
	vJPlus = JPlus;
	return(*this);
}

inline cHoraire& cHoraire::setMaximum()
{
	vJPlus = 255;
	vHeure.setHeure('M', 'M');
	return(*this);
}

inline cHoraire& cHoraire::setMinimum()
{
	vJPlus = 0;
	vHeure.setHeure('m', 'm');
	return(*this);
}

inline tDureeEnJours cHoraire::JPlus() const
{
	return(vJPlus);
}


inline tHeure cHoraire::Heures() const
{
	return(vHeure.Heures());
}

inline tMinute cHoraire::Minutes() const
{
	return(vHeure.Minutes());
}

inline const cHeure& cHoraire::getHeure() const
{
	return(vHeure);
}

inline cHeure& cHeure::operator = (const cHoraire& Autre)
{
	return(*this = Autre.getHeure());
}

/*
template <class charT, class Traits> basic_ostream<charT, Traits>& operator<<(
		basic_ostream<charT, Traits> &flux, const cHoraire& Obj)
		{
			typename basic_ostream<charT, Traits>::sentry init(flux);
			if (init)
			{
				flux << Obj.JPlus() << " " << Obj.getHeure();
			}
			return(flux);
		}
		
template <class Traits> basic_ostream<cTexteCodageInterne, Traits>& operator<<(
		basic_ostream<cTexteCodageInterne, Traits> &flux, const cHoraire& Obj)
		{
			typename basic_ostream<cTexteCodageInterne, Traits>::sentry init(flux);
			if (init)
			{
				flux << Obj.JPlus() << Obj.getHeure();
			}
			return(flux);
		}
*/

template <class T>
inline T& operator<<(T& flux, const cHoraire& Obj)
{
	flux << Obj.JPlus() << " " << Obj.getHeure();
	return flux;
}


template <>
inline cTexteCodageInterne& operator<< <cTexteCodageInterne>(cTexteCodageInterne& flux, const cHoraire& Obj)
{
	flux << Obj.JPlus() << Obj.getHeure();
	return flux;
}


#endif
