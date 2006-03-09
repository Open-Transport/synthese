

#include "cRequeteSQLInsert.h"

/*! \brief Constructeur
 \param nomTable Nom de la table dans laquelle se fera l'insertion
*/
cRequeteSQLInsert::cRequeteSQLInsert( const std::string& NomTable )
{
    vNomTable << NomTable;
}
