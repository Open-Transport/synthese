
#include "NetworkAccessPoint.h"

/*!	Accesseur vers le nom du quai au sein du point d'arr�t.
	\return R�f�rence vers le nom du quai au sein du point d'arr�t
	\author Hugues Romain
	\date 2001
*/
const std::string& NetworkAccessPoint::getNom() const
{
	return _name;
}


/*!	D�finit le nom du quai au sein du point d'arr�t.
	\param Nom Nom � donner au quai
	\return true si op�ration effectu�e avec succ�s
	\author Hugues Romain
	\date 2005
*/
void NetworkAccessPoint::setNom(const std::string& Nom)
{
	_name = Nom;
}


/** Destructeur.
*/
NetworkAccessPoint::~NetworkAccessPoint()
{
}

/** Constructeur.
	@param _logicalPlace Lieu logique auquel appartient le point d'acc�s
*/
NetworkAccessPoint::NetworkAccessPoint(LogicalPlace* const logicalPlace, size_t rank)
:	cPoint()
, _logicalPlace(logicalPlace)
, _rank(rank)
{
  // TODO ajouter insertion dans la logical place je pense
  // avec index si rank est connu sinon en push_back

}


