
#include "NetworkAccessPoint.h"

/*!	Accesseur vers le nom du quai au sein du point d'arrét.
	\return Référence vers le nom du quai au sein du point d'arrét
	\author Hugues Romain
	\date 2001
*/
const std::string& NetworkAccessPoint::getNom() const
{
	return _name;
}


/*!	Définit le nom du quai au sein du point d'arrét.
	\param Nom Nom à donner au quai
	\return true si opération effectuée avec succès
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
	@param _logicalPlace Lieu logique auquel appartient le point d'accès
*/
NetworkAccessPoint::NetworkAccessPoint(LogicalPlace* const logicalPlace, size_t rank)
:	cPoint()
, _logicalPlace(logicalPlace)
, _rank(rank)
{
  // TODO ajouter insertion dans la logical place je pense
  // avec index si rank est connu sinon en push_back

}


