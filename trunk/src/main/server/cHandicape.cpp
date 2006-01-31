/*!	\file cHandicape.cpp
	\brief Impl�mentation classe modalit� de prise en charge des handicap�s
*/



#include "cHandicape.h"



/*!	\brief Constructeur
	\author Hugues Romain
	\date 2005

Les objets modalit� de prise en charge des handicap�s sont initialis�s par les valeurs par d�faut suivantes :
 - Prise en charge gratuite
 - Pas de r�servation
 - Une place par v�hicule
*/
cHandicape::cHandicape()
{
	setPrix(0);
	setResa(NULL);
	setContenance(1);
}
