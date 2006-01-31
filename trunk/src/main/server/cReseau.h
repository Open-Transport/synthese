/*------------------*
 |                  |
 |  Classe Reseau   |
 |                  |
 *------------------*/

#ifndef SYNTHESE_CRESEAU_H
#define SYNTHESE_CRESEAU_H

#include "cTexte.h"
 
typedef int tNumeroReseau;

/** Réseau de transport
	@ingroup m05
*/
class cReseau
{
	cTexte vNom;
	cTexte vURL;
	cTexte vDoc;
	
public:
	void setURL(const cTexte& newVal);
	void setDoc(const cTexte& newVal);
	void setNom(const cTexte& newVal);
};

inline void cReseau::setNom(const cTexte &newVal)
{
	vNom.Vide();
	vNom << newVal;
}


inline void cReseau::setDoc(const cTexte &newVal)
{
	vDoc.Vide();
	vDoc << newVal;
}


inline void cReseau::setURL(const cTexte &newVal)
{
	vURL.Vide();
	vURL << newVal;
}

#endif
