
#ifndef SYNTHESE_CSERVEURBASEDEDONNEES_H
#define SYNTHESE_CSERVEURBASEDEDONNEES_H

#include "cTexteSQL.h"
#include "cResultatBaseDeDonnees.h"

class cServeurBaseDeDonnees
{
public:
    cServeurBaseDeDonnees();
    virtual tIndex Select(const cTexte& __Table,const cTableauDynamique<cTexte>& __Champs,const cTexte& __Where,const cTableauDynamique<cTexte>& __Ordre,tIndex __Limite,cResultatBaseDeDonnees& __Resultat);
    virtual bool Insert(const cTexte& __Table,const cTexte& __Valeurs,bool __Replace);
    virtual tIndex Update(const cTexte& __Table,const cTableauDynamique<cTexte>& __Champs,const cTexte& __Valeurs,const cTexte& __Where,tIndex __Limite);
    virtual bool Delete(const cTexte& __Table,const cTexte& __Where,const cTableauDynamique<cTexte>& __Ordre,tIndex __Limite);
    virtual tIndex Count(const cTexte& __Table,const cTexte& __Where);
    virtual tIndex Max(const cTexte& __Table,const cTexte& __Champ,const cTexte& __Where);
    virtual tIndex NextID(const cTexte& __Table);
};

#endif
