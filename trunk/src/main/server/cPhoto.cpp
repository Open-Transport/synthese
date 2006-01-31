/*--------------------------------------*
 |                                      |
 |  APDOS - SYNTHESE v0.6               |
 |  © Hugues Romain 2000/2002           |
 |  cPhoto.cpp                          |
 |  Implementation Classes Photo        |
 |                                      |
 *--------------------------------------*/

#include "cPhoto.h"

/*! \brief Constructeur
	\author Hugues Romain
	\date 2002
*/
cPhoto::cPhoto(tIndex newCode)
{
	_Index = newCode;
}



/*! \brief Constructeur de lien vers autre photo

Effectue la copie des chaines de caracètres fournies
	
	\author Hugues Romain
	\date 2002
*/	
bool cMapPhoto::SetDonnees(const cTexte& newCoords, tIndex newLienPhoto, const cTexteHTML& newAlt)
{
	vCoords = newCoords;
	vLienPhoto = newLienPhoto;
	vAlt = newAlt;
	vURL.Vide();
	return true;
}



/*! \brief Constructeur de lien vers URL

Effectue la copie des chaînes de caractères fournies

	\author Hugues Romain
	\date 2002
*/
bool cMapPhoto::SetDonnees(const cTexte& newCoords, const cTexte& newURL, const cTexteHTML& newAlt)
{
	vCoords = newCoords;
	vLienPhoto = 0;
	vAlt = newAlt;
	vURL = newURL;
	return true;
}
