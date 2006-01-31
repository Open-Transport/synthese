/*!	\file Point.h
	\brief En-t�te classe cPoint
	\author Hugues Romain
	\date 2000-2003
*/

#ifndef SYNTHESE_POINT_H
#define SYNTHESE_POINT_H

#include "Parametres.h"

typedef short int CoordonneeKM;
typedef short int CoordonneeM;
typedef long int DistanceCarreeKM;
typedef long int CoordonneeKMM;


/*! 	\brief Point g�ographique
	\author Hugues Romain
	\date 2000-2003
	
	Les coordonn�es g�ographiques Lambert II sont stock�es sous forme de deux entiers : l'un pour la partie "kilom�trique" de la coordonn�es, l'autre pour la partie "m�trique" ( \f$ x=x_{km}*1000+x_m \f$ et \f$ y=y_{km}*1000+y_m \f$)
	
	Le but de cette s�paration est de permettre des calculs plus rapides lorsque la pr�cision m�trique n'est pas n�cessaire, en se cantonnant � des calculs sur la partie kilom�trique.
*/
class cPoint
{
	friend class cDistanceCarree;

	//! \name Coordonn�es Lambert II
	//@{
	CoordonneeKM	_XKM;	//!< Longitude en kilom�tres, arrondie au kilom�tre inf�rieur
	CoordonneeKM	_YKM;	//!< Latitude en kilom�tres, arrondie au kilom�tre inf�rieur
	CoordonneeM	_XM;		//!< Reste de l'arrondi au kilom�tre de la longitude, en m�tres
	CoordonneeM	_YM;		//!< Reste de l'arrondi au kilom�tre de la latitude, en m�tres
	//@}

public:
	
	//!\name Op�rateurs
	//@{
	int operator== (const cPoint&)	const;
	//@}
	
	//! \name Accesseurs
	//@{
	bool 		EstInconnu()	const;
	CoordonneeKMM	XKMM()		const;
	CoordonneeKMM	YKMM()		const;
	//@}
	
	//! \name Constructeur
	//@{
	cPoint();
	//@}

	//! \name Modificateurs
	//@{
	void setX(const CoordonneeKMM);
	void setY(const CoordonneeKMM);
	//@}
};



inline CoordonneeKMM cPoint::XKMM() const
{
	if (EstInconnu())
		return INCONNU;
	else
		return _XKM * 1000 + _XM;
}

inline CoordonneeKMM cPoint::YKMM() const
{
	if (EstInconnu())
		return INCONNU;
	else
		return _YKM * 1000 + _YM;
}

inline bool cPoint::EstInconnu() const
{
	return _XKM == INCONNU || _YKM == INCONNU;
}



/*!	\brief Modificateur de la longitude
	\param newCoordonneeKMM Nouvelle longitude arrondie au m�tre pr�s (INCONNU pour coordonn�es inconnues)
	
	Ce modificateur assure la s�paration de la longitude en deux valeurs : la valeur en kilom�tres, et le reste en m�tres
*/
inline void cPoint::setX(const CoordonneeKMM __XKMM)
{
	if (__XKMM == INCONNU)
		_XKM = INCONNU;
	else
	{
		_XKM = (CoordonneeKM) (__XKMM / 1000);
		_XM = (CoordonneeM) (__XKMM % 1000);
	}
}



/*!	\brief Modificateur de la latitude
	\param newCoordonneeKMM Nouvelle latitude arrondie au m�tre pr�s (INCONNU pour coordonn�es inconnues)

	Ce modificateur assure la s�paration de la longitude en deux valeurs : la valeur en kilom�tres, et le reste en m�tres
*/
inline void cPoint::setY(const CoordonneeKMM __YKMM)
{
	if (__YKMM == INCONNU)
		_YKM = INCONNU;
	else
	{
		_YKM = (CoordonneeKM) (__YKMM / 1000);
		_YM = (CoordonneeM) (__YKMM % 1000);
	}
}



/*!	\brief Op�rateur de comparaison des coordonn�es de deux points
	\param AutrePoint le point � tester
	\return true si les coordonn�es sont identiques au m�tre pr�s, false si l'un des deux au moins est inconnu
	\author Hugues Romain
	\date 2005
*/
inline int cPoint::operator== (const cPoint& __Point) const
{
	// Un des points est inconnu : pas d'�galit�
	if (EstInconnu() || __Point.EstInconnu())
		return false;
	
	// Test d'�galit�
	return __Point._XKM == _XKM && __Point._YKM == _YKM;
}

#endif
