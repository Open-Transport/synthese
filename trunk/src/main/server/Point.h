/*!	\file Point.h
	\brief En-tête classe cPoint
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


/*! 	\brief Point géographique
	\author Hugues Romain
	\date 2000-2003
	
	Les coordonnées géographiques Lambert II sont stockées sous forme de deux entiers : l'un pour la partie "kilométrique" de la coordonnées, l'autre pour la partie "métrique" ( \f$ x=x_{km}*1000+x_m \f$ et \f$ y=y_{km}*1000+y_m \f$)
	
	Le but de cette séparation est de permettre des calculs plus rapides lorsque la précision métrique n'est pas nécessaire, en se cantonnant à des calculs sur la partie kilométrique.
*/
class cPoint
{
	friend class cDistanceCarree;

	//! \name Coordonnées Lambert II
	//@{
	CoordonneeKM	_XKM;	//!< Longitude en kilomètres, arrondie au kilomètre inférieur
	CoordonneeKM	_YKM;	//!< Latitude en kilomètres, arrondie au kilomètre inférieur
	CoordonneeM	_XM;		//!< Reste de l'arrondi au kilomètre de la longitude, en mètres
	CoordonneeM	_YM;		//!< Reste de l'arrondi au kilomètre de la latitude, en mètres
	//@}

public:
	
	//!\name Opérateurs
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
	\param newCoordonneeKMM Nouvelle longitude arrondie au mètre près (INCONNU pour coordonnées inconnues)
	
	Ce modificateur assure la séparation de la longitude en deux valeurs : la valeur en kilomètres, et le reste en mètres
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
	\param newCoordonneeKMM Nouvelle latitude arrondie au mètre près (INCONNU pour coordonnées inconnues)

	Ce modificateur assure la séparation de la longitude en deux valeurs : la valeur en kilomètres, et le reste en mètres
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



/*!	\brief Opérateur de comparaison des coordonnées de deux points
	\param AutrePoint le point à tester
	\return true si les coordonnées sont identiques au mètre près, false si l'un des deux au moins est inconnu
	\author Hugues Romain
	\date 2005
*/
inline int cPoint::operator== (const cPoint& __Point) const
{
	// Un des points est inconnu : pas d'égalité
	if (EstInconnu() || __Point.EstInconnu())
		return false;
	
	// Test d'égalité
	return __Point._XKM == _XKM && __Point._YKM == _YKM;
}

#endif
