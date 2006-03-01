
#ifndef SYNTHESE_CSERVEURBASEDEDONNEES_H
#define SYNTHESE_CSERVEURBASEDEDONNEES_H

#include "cTexteSQL.h"
#include "cResultatBaseDeDonnees.h"

/** @ingroup m02 */
class cServeurBaseDeDonnees
{
    public:
        cServeurBaseDeDonnees();
        virtual int Select( const cTexte& __Table, const cTableauDynamique<cTexte>& __Champs, const cTexte& __Where, const cTableauDynamique<cTexte>& __Ordre, int __Limite, cResultatBaseDeDonnees& __Resultat );
        virtual bool Insert( const cTexte& __Table, const cTexte& __Valeurs, bool __Replace );
        virtual int Update( const cTexte& __Table, const cTableauDynamique<cTexte>& __Champs, const cTexte& __Valeurs, const cTexte& __Where, int __Limite );
        virtual bool Delete( const cTexte& __Table, const cTexte& __Where, const cTableauDynamique<cTexte>& __Ordre, int __Limite );
        virtual int Count( const cTexte& __Table, const cTexte& __Where );
        virtual int Max( const cTexte& __Table, const cTexte& __Champ, const cTexte& __Where );
        virtual int NextID( const cTexte& __Table );
};

#endif
