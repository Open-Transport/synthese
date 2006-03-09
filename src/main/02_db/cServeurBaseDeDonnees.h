
#ifndef SYNTHESE_CSERVEURBASEDEDONNEES_H
#define SYNTHESE_CSERVEURBASEDEDONNEES_H

#include <vector>

#include "cResultatBaseDeDonnees.h"


/** @ingroup m02 */
class cServeurBaseDeDonnees
{
    public:
        cServeurBaseDeDonnees();
        virtual int Select( const std::string& __Table, const std::vector<std::string>& __Champs, const std::string& __Where, const std::vector<std::string>& __Ordre, int __Limite, cResultatBaseDeDonnees& __Resultat );
        virtual bool Insert( const std::string& __Table, const std::string& __Valeurs, bool __Replace );
        virtual int Update( const std::string& __Table, const std::vector<std::string>& __Champs, const std::string& __Valeurs, const std::string& __Where, int __Limite );
        virtual bool Delete( const std::string& __Table, const std::string& __Where, const std::vector<std::string>& __Ordre, int __Limite );
        virtual int Count( const std::string& __Table, const std::string& __Where );
        virtual int Max( const std::string& __Table, const std::string& __Champ, const std::string& __Where );
        virtual int NextID( const std::string& __Table );
};

#endif
