#include "cAlerte.h"


/*!	\brief Constructeur
	\author Hugues Romain
	\date 2005
	
Les �l�ments alerte sont initialis�s avec les valeurs suivantes :
 - p�riode d'applicabilit� permanente
 - niveau d'alerte 1 (ATTENTION)
*/
cAlerte::cAlerte()
{
	_Debut.setMoment(TEMPS_MIN);
	_Fin.setMoment(TEMPS_MAX);
	_Niveau = ALERTE_ATTENTION;
}


/*!	\brief Test d'applicabilit� du message d'alerte
	\param __Debut D�but de la plage temporelle d'application
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
