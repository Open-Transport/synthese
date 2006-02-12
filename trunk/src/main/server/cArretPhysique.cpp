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
: NetworkAccessPoint()
{
}

cArretPhysique::~cArretPhsyique()
{
}



/** Liste des adresses routières permettant l'accès au lieu logique de l'arrêt physique.
	@param forDeparture : true indique que les adresses sont des point de départ, le temps de correspondance sera donc le temps pour se rendre à ces adresses depuis le point d'arrêt physique. false : inversement.
*/
NetworkAccessPoint::AddressList cArretPhysique::getAddresses(bool forDeparture) const
{
	NetworkAccessPoint::AddressList result;
	LogicalPlace::AddressesMap addresslist = getLogicalPlace()->getAddresses();
	
	for (LogicalPlace::AddressesMap::iterator element = addresslist.begin(); element != addresslist.end(); element++)
		result.insert(pair(
			element->second()
			, forDeparture 
				? getLogicalPlace()->AttenteCorrespondance(rank, element->first())
				: getLogicalPlace()->AttenteCorrespondance(element->first(), rank)
		));

	return result;
}