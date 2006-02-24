/** Impl�mentation classe de gestion des requetes (une instance par thread).
	@file cTexteRequeteSYNTHESE.cpp
*/


#include <cstdlib>
using namespace std;

#include "cTexteRequeteSYNTHESE.h"
#include "Parametres.h"
#include "cDate.h"
#include "cMoment.h"


/*!	\brief Parsage de la requ�te pour exploitation ult�rieure
	\author Christophe Romain
	\author Hugues Romain
	\date 2005
	\warning Conform�ment aux r�gles standard, la taille d'une requ�te est limit�e � 4096 octets.
*/
void cTexteRequeteSYNTHESE::Finalise()
{
	// Finalisation standard de chaine de caract�re
	cTexte::Finalise();

    /* Parcours les param�tes CGI */
    if(Taille())
    {
		// Variables temporaires de travail
        int len = Taille();
		char *key, *value, *end;
		char buffer[TAILLE_MAX_REQUETE];
        
		// R�duction � la taille standard le cas �ch�ant
		if (len >= TAILLE_MAX_REQUETE)
			len = TAILLE_MAX_REQUETE-1;

		// Copie de la requ�te �ventuellement tronqu�e dans un buffer
        memset(buffer, 0, TAILLE_MAX_REQUETE);
        strncpy(buffer, _Element, len); /* on est certain d'avoir un 0 en fin de buffer */
        
		// Parcours int�gral
		key = value = end = buffer; 
        while(end < buffer+len)
        {
			// Recherche du caract�re d'affectation pour obtenir la cl�
            key = end;
            while (end < buffer+len && *end != '&' && *end != 0 && *end != '=')
				end++;

			// Caract�re d'affectation non trouv�, param�tre ignor�
            if(end >= buffer+len || *end != '=')
				break;

			// Marquage de la fin de la cl�
            *end = 0;
			end++;

			// Recherche de la fin du param�tre pour obtenir la valeur
            value = end;
            while (end < buffer+len && *end != '&' && *end != 0 && *end != '\r' && *end != '\n')
				end++;
            
			// En cas de d�passement de la longueur de la chaine, effacement du dernier caract�re
			if (end > buffer+len)
				end--;

			// Marquage de la fin de la valeur
            *end = 0;
			end++;

			// Enregistrement des r�sultats
            params[key] = value;
        }
    }
}


/*!	\brief Test d'existence de param�tre
	\param nomParam Nom du param�tre � trouver
	\return true si le param�tre est d�crit par la requ�te, false sinon
	\author Christophe Romain
	\date 2005
*/
bool cTexteRequeteSYNTHESE::checkParam(const char * nomParam) const
{
    return params.find(nomParam) != params.end();
} 



/*!	\brief Accesseur � un param�tre de type date
	\param __NomParam Nom du param�tre � trouver
	\return La valeur date du param�tre si d�crit, INCONNU sinon
	\author Hugues Romain
	\date 2005
*/
cDate cTexteRequeteSYNTHESE::getDate(const char* __NomParam)
{
	cDate __Date;
	
	if (checkParam(__NomParam))
		__Date = getTexte(__NomParam);
	else
		__Date.setDate(TEMPS_INCONNU);	
	
	return __Date;
}



/*!	\brief Accesseur � un param�tre de type moment
	\param __NomParam Nom du param�tre � trouver
	\return La valeur moment du param�tre si d�crit, INCONNU sinon
	\author Hugues Romain
	\date 2005
*/
cMoment cTexteRequeteSYNTHESE::getMoment(const char* __NomParam)
{
	cMoment __Moment;
	
	if (checkParam(__NomParam))
		__Moment = getTexte(__NomParam);
	else
		__Moment.setMoment(TEMPS_INCONNU);	
	
	return __Moment;
}



/*!	\brief Accesseur � un param�tre de type bool�en
	\param nomParam Nom du param�tre � trouver
	\return FAUX si le param�tre vaut 0, INCONNU si le param�tre est vide ou vaut -1, VRAI sinon
	\author Christophe Romain
	\author Hugues Romain
	\date 2005
*/
tBool3 cTexteRequeteSYNTHESE::getBool3(const char * nomParam)
{
	if (checkParam(nomParam))
	{
		switch (atoi(params[nomParam].c_str()))
		{
		case 0:
			return Faux;
		case -1:
			return Indifferent;
		default:
			return Vrai;
		}
	}

	return Indifferent;
}



/*!	\brief Accesseur � un param�tre de type texte
	\param nomParam Nom du param�tre � trouver
	\return La valeur texte du param�tre si d�crit, une cha�ne vide sinon
	\author Christophe Romain
	\date 2005
*/
cTexte cTexteRequeteSYNTHESE::getTexte(const char * nomParam)
{
    if (checkParam(nomParam))
    {
        return cTexte(params[nomParam].c_str());
    }
    return cTexte();
}



/*!	\brief Accesseur � un param�tre de type num�rique
	\param nomParam Nom du param�tre � trouver
	\return La valeur num�rique du param�tre si d�crit, INCONNU sinon
	\author Christophe Romain
	\date 2005
*/
int cTexteRequeteSYNTHESE::getInt(const char * nomParam)
{
    if (checkParam(nomParam))
        return atoi(params[nomParam].c_str());
    
    return INCONNU;
}



tIndex cTexteRequeteSYNTHESE::ConvertChar(const char c, char* Tampon) const
{
	switch (c)
	{
		case '+':	if (Tampon) strncpy(Tampon," ", 1); return 1;
		default:	if (Tampon) *Tampon = c; return 1;
	}
}


