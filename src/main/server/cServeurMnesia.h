
#ifndef SYNTHESE_CSERVEURMNESIA_H
#define SYNTHESE_CSERVEURMNESIA_H

#include "cServeurBaseDeDonnees.h"

extern "C"
{
#include <erl_interface.h>
#include <ei.h>
}

#define INCONNU -1

/** @ingroup m02 */
class cServeurMnesia : public cServeurBaseDeDonnees
{
    private:
        int _ErlFD;
        static unsigned short _NodeId;
    protected:
        /* syncronous function call */
        ETERM *_call( const cTexte& __Mod, const cTexte& __Fun, const cTexte& __Args );
        /* asyncronous message send */
        void _send( const cTexte& __Dest, const cTexte& __Msg );
        /* syncronous message receive */
        void _recv();
        /* temporary, while rcsdb is under construction */
        cTexte _where2erl( const cTexte& __Where );
        cTexte _value2erl( const cTexte& __Where );
    public:
        cServeurMnesia( const cTexte& __Node, const cTexte& __Cookie );
        ~cServeurMnesia();

        int Select( const cTexte& __Table, const cTableauDynamique<cTexte>& __Champs, const cTexte& __Where, const cTableauDynamique<cTexte>& __Ordre, int __Limite, cResultatBaseDeDonnees& __Resultat );
        bool Insert( const cTexte& __Table, const cTexte& __Valeurs, bool __Replace );
        int Update( const cTexte& __Table, const cTableauDynamique<cTexte>& __Champs, const cTexte& __Valeurs, const cTexte& __Where, int __Limite );
        bool Delete( const cTexte& __Table, const cTexte& __Where, const cTableauDynamique<cTexte>& __Ordre, int __Limite );
        int Count( const cTexte& __Table, const cTexte& __Where );
        int Max( const cTexte& __Table, const cTexte& __Champ, const cTexte& __Where );
        int NextID( const cTexte& __Table );
        void SQL( const cTexteSQL& query );

};

#endif

