/** Implémentation classe de gestion des requetes (une instance par thread).
	@file cTexteRequeteSYNTHESE.cpp
*/


#include <cstdlib>
using namespace std;

#include "cTexteRequeteSYNTHESE.h"
#include "Parametres.h"
#include "cDate.h"
#include "cMoment.h"


/*!	\brief Parsage de la requête pour exploitation ultérieure
	\author Christophe Romain
	\author Hugues Romain
	\date 2005
	\warning Conformément aux règles standard, la taille d'une requête est limitée à 4096 octets.
*/
void cTexteRequeteSYNTHESE::Finalise()
{
	// Finalisation standard de chaine de caractère
	cTexte::Finalise();

    /* Parcours les paramètes CGI */
    if(Taille())
    {
		// Variables temporaires de travail
        int len = Taille();
		char *key, *value, *end;
		char buffer[TAILLE_MAX_REQUETE];
        
		// Réduction à la taille standard le cas échéant
		if (len >= TAILLE_MAX_REQUETE)
			len = TAILLE_MAX_REQUETE-1;

		// Copie de la requête éventuellement tronquée dans un buffer
        memset(buffer, 0, TAILLE_MAX_REQUETE);
        strncpy(buffer, _Element, len); /* on est certain d'avoir un 0 en fin de buffer */
        
		// Parcours intégral
		key = value = end = buffer; 
        while(end < buffer+len)
        {
			// Recherche du caractère d'affectation pour obtenir la clé
            key = end;
            while (end < buffer+len && *end != '&' && *end != 0 && *end != '=')
				end++;

			// Caractère d'affectation non trouvé, paramètre ignoré
            if(end >= buffer+len || *end != '=')
				break;

			// Marquage de la fin de la clé
            *end = 0;
			end++;

			// Recherche de la fin du paramètre pour obtenir la valeur
            value = end;
            while (end < buffer+len && *end != '&' && *end != 0 && *end != '\r' && *end != '\n')
				end++;
            
			// En cas de dépassement de la longueur de la chaine, effacement du dernier caractère
			if (end > buffer+len)
				end--;

			// Marquage de la fin de la valeur
            *end = 0;
			end++;

			// Enregistrement des résultats
            params[key] = value;
        }
    }
}


/*!	\brief Test d'existence de paramètre
	\param nomParam Nom du paramètre à trouver
	\return true si le paramètre est décrit par la requête, false sinon
	\author Christophe Romain
	\date 2005
*/
bool cTexteRequeteSYNTHESE::checkParam(const char * nomParam) const
{
    return params.find(nomParam) != params.end();
} 



/*!	\brief Accesseur à un paramètre de type date
	\param __NomParam Nom du paramètre à trouver
	\return La valeur date du paramètre si décrit, INCONNU sinon
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



/*!	\brief Accesseur à un paramètre de type moment
	\param __NomParam Nom du paramètre à trouver
	\return La valeur moment du paramètre si décrit, INCONNU sinon
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



/*!	\brief Accesseur à un paramètre de type booléen
	\param nomParam Nom du paramètre à trouver
	\return FAUX si le paramètre vaut 0, INCONNU si le paramètre est vide ou vaut -1, VRAI sinon
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



/*!	\brief Accesseur à un paramètre de type texte
	\param nomParam Nom du paramètre à trouver
	\return La valeur texte du paramètre si décrit, une chaîne vide sinon
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



/*!	\brief Accesseur à un paramètre de type numérique
	\param nomParam Nom du paramètre à trouver
	\return La valeur numérique du paramètre si décrit, INCONNU sinon
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


