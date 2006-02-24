/*!	\file Point.cpp
	\brief Impl�mentation classe cPoint
	\author Hugues Romain
	\date 2000-2001
*/



#include "Point.h"



/*!	\brief Constructeur
	
Le point est initialis� aux coordonn�es inconnues
*/
cPoint::cPoint()
{
	setX(INCONNU);
	setY(INCONNU);
}


cPoint::~cPoint()
{
}




CoordonneeKMM cPoint::XKMM() const
{
	if (unknownLocation())
		return INCONNU;
	else
		return _XKM * 1000 + _XM;
}

CoordonneeKMM cPoint::YKMM() const
{
	if (unknownLocation())
		return INCONNU;
	else
		return _YKM * 1000 + _YM;
}

bool cPoint::unknownLocation() const
{
	return _XKM == INCONNU || _YKM == INCONNU;
}



/*!	\brief Modificateur de la longitude
	\param newCoordonneeKMM Nouvelle longitude arrondie au m�tre pr�s (INCONNU pour coordonn�es inconnues)
	
	Ce modificateur assure la s�paration de la longitude en deux valeurs : la valeur en kilom�tres, et le reste en m�tres
*/
void cPoint::setX(const CoordonneeKMM __XKMM)
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
void cPoint::setY(const CoordonneeKMM __YKMM)
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
int cPoint::operator== (const cPoint& __Point) const
{
	// Un des points est inconnu : pas d'�galit�
	if (unknownLocation() || __Point.unknownLocation())
		return false;
	
	// Test d'�galit�
	return __Point._XKM == _XKM && __Point._YKM == _YKM;
}