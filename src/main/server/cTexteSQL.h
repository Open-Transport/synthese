
#ifndef SYNTHESE_CTEXTESQL_H
#define SYNTHESE_CTEXTESQL_H

#include "cTexte.h"
#include "cDate.h"

/*!	\brief Classe de texte destinée à recevoir des éléments de requête SQL

Le but de cette classe est de gérer les problèmes de caractères interdits (guillemets...) ainsi que les tentatives de détournement possibles (insertion d'un ; et d'une seconde requete nuisible en suivant)
	@ingroup m02
*/
class cTexteSQL : public cTexte
{
protected:
	tIndex ConvertChar(const char c, char* Tampon=NULL) const;
	
public:
	//! \name Constructeurs et destructeurs
	//@{
	cTexteSQL() : cTexte() {}
	cTexteSQL(const cTexte& t) : cTexte(t) {}
	//@}
	
};




/*!	\brief Codage SQL d'une date
	\author Hugues Romain
	\date 2005
*/
template <> 
inline cTexteSQL& operator<< <cTexteSQL> (cTexteSQL& flux, const cDate& Obj)
{
	flux << Obj.Annee() << "-" << Obj.Mois() << "-" << Obj.Jour();
	return flux;
}

#endif
