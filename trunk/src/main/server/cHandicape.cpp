/*!	\file cHandicape.cpp
	\brief Implémentation classe modalité de prise en charge des handicapés
*/



#include "cHandicape.h"



/*!	\brief Constructeur
	\author Hugues Romain
	\date 2005

Les objets modalité de prise en charge des handicapés sont initialisés par les valeurs par défaut suivantes :
 - Prise en charge gratuite
 - Pas de réservation
 - Une place par véhicule
*/
cHandicape::cHandicape()
{
	setPrix(0);
	setResa(NULL);
	setContenance(1);
}
