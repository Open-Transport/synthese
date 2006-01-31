/*!	\file cArretPhysique.cpp
	\brief Impl�mentation classe quai
*/

#include "cArretPhysique.h"

/*!	\brief Constructeur
	\version 2
	\author Hugues Romain
	\date 2001-2005
*/
cArretPhysique::cArretPhysique()
{
	_Photo = NULL;
}

/*!	\brief D�finit les coordonn�es de longitude du quai
	\param X Latitude en coordonn�es Lambert II Etendu
	\return true si op�ration effectu�e avec succ�s
	\author Hugues Romain
	\date 2005
*/
bool cArretPhysique::setX(const CoordonneeKMM X)
{
	_Point.setX(X);
	return true;
}

/*!	\brief D�finit les coordonn�es de latitude du quai
	\param Y Latitude en coordonn�es Lambert II Etendu
	\return true si op�ration effectu�e avec succ�s
	\author Hugues Romain
	\date 2005
*/
bool cArretPhysique::setY(const CoordonneeKMM Y)
{
	_Point.setY(Y);
	return true;
}
