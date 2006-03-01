
#ifndef SYNTHESE_CTEXTESQL_H
#define SYNTHESE_CTEXTESQL_H

#include "cTexte.h"
#include "04_time/Date.h"

/*! \brief Classe de texte destinée à recevoir des éléments de requête SQL
 
Le but de cette classe est de gérer les problèmes de caractères interdits (guillemets...) ainsi que les tentatives de détournement possibles (insertion d'un ; et d'une seconde requete nuisible en suivant)
 @ingroup m02
*/
class cTexteSQL : public cTexte
{
    protected:
        int ConvertChar( const char c, char* Tampon = NULL ) const;

    public:
        //! \name Constructeurs et destructeurs
        //@{
        cTexteSQL() : cTexte() {}
        cTexteSQL( const cTexte& t ) : cTexte( t ) {}
        //@}
}
;




/*! \brief Codage SQL d'une date
 \author Hugues Romain
 \date 2005
*/
/*  MJ review
template <>
inline cTexteSQL& operator<< <cTexteSQL> ( cTexteSQL& flux, const synthese::time::Date& Obj )
{
    flux << Obj.getYear () << "-" << Obj.getMonth () << "-" << Obj.getDay();
    return flux;
}
*/
#endif
