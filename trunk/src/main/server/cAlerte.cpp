#include "cAlerte.h"


/*!	\brief Constructeur
	\author Hugues Romain
	\date 2005
	
Les éléments alerte sont initialisés avec les valeurs suivantes :
 - période d'applicabilité permanente
 - niveau d'alerte 1 (ATTENTION)
*/
cAlerte::cAlerte()
{
	_Debut.setMoment(TEMPS_MIN);
	_Fin.setMoment(TEMPS_MAX);
	_Niveau = ALERTE_ATTENTION;
}


/*!	\brief Test d'applicabilité du message d'alerte
	\param __Debut Début de la plage temporelle d'application
	\param __Fin Fin de la plage temporelle d'application
	\return true si le message et non vide et est valide pour tout ou partie de la plage temporelle d'application fournie, false sinon
	\author Hugues Romain
	\date 2005
*/
bool cAlerte::showMessage(const cMoment& __Debut, const cMoment& __Fin) const
{
	//pas de message
	if (!_Message.Taille())
		return false;
		
	//test de la validite du message
	if ((__Fin < _Debut) || (__Debut > _Fin))
		return false;
	
	return true;		
}
