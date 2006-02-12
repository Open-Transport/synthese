
#include "NetworkAccessPoint.h"

/*!	Accesseur vers le nom du quai au sein du point d'arrét.
	\return Référence vers le nom du quai au sein du point d'arrét
	\author Hugues Romain
	\date 2001
*/
const cTexte& NetworkAccessPoint::getNom() const
{
	return _name;
}


/*!	Définit le nom du quai au sein du point d'arrét.
	\param Nom Nom à donner au quai
	\return true si opération effectuée avec succès
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