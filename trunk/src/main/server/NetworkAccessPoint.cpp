
#include "NetworkAccessPoint.h"

/*!	Accesseur vers le nom du quai au sein du point d'arr�t.
	\return R�f�rence vers le nom du quai au sein du point d'arr�t
	\author Hugues Romain
	\date 2001
*/
const cTexte& NetworkAccessPoint::getNom() const
{
	return _name;
}


/*!	D�finit le nom du quai au sein du point d'arr�t.
	\param Nom Nom � donner au quai
	\return true si op�ration effectu�e avec succ�s
	\author Hugues Romain
	\date 2005
*/
bool NetworkAccessPoint::setNom(const cTexte& Nom)
{
	_name.Vide();
	_name << Nom;
	return true;
}


/** Destructeur.
*/
NetworkAccessPoint::~NetworkAccessPoint()
{
}