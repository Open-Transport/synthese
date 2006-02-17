/*! \file cDistanceCarree.cpp
	\brief Impl�mentation classe cDistanceCarree
*/

#include "cDistanceCarree.h"

/*!	\brief Constructeur
	\param Valeur valeur de l'objet en km�
	\author Hugues Romain
	\date 2005
*/
cDistanceCarree::cDistanceCarree(const DistanceCarreeKM Valeur)
{
	setFromKM2(Valeur);
}

/*! 	\brief Cr�e une instance contenant un minorant du carr� de la distance entre deux points
	\author Hugues Romain
	\date 2005
	\param Point1,Point2 les deux points
*/
cDistanceCarree::cDistanceCarree(const cPoint& Point1, const cPoint& Point2)
{
	setFromPoints(Point1, Point2);
}
