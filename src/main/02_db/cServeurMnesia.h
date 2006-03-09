
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
        ETERM *_call( const std::string& __Mod, const std::string& __Fun, const std::string& __Args );
        /* asyncronous message send */
        void _send( const std::string& __Dest, const std::string& __Msg );
        /* syncronous message receive */
        void _recv();
        /* temporary, while rcsdb is under construction */
        std::string _where2erl( const std::string& __Where );
        std::string _value2erl( const std::string& __Where );
    public:
        cServeurMnesia( const std::string& __Node, const std::string& __Cookie );
        ~cServeurMnesia();

        int Select( const std::string& __Table, const cTableauDynamique<std::string>& __Champs, const std::string& __Where, const cTableauDynamique<std::string>& __Ordre, int __Limite, cResultatBaseDeDonnees& __Resultat );
        bool Insert( const std::string& __Table, const std::string& __Valeurs, bool __Replace );
        int Update( const std::string& __Table, const cTableauDynamique<std::string>& __Champs, const std::string& __Valeurs, const std::string& __Where, int __Limite );
        bool Delete( const std::string& __Table, const std::string& __Where, const cTableauDynamique<std::string>& __Ordre, int __Limite );
        int Count( const std::string& __Table, const std::string& __Where );
        int Max( const std::string& __Table, const std::string& __Champ, const std::string& __Where );
        int NextID( const std::string& __Table );
        void SQL( const std::stringSQL& query );

};

#endif

