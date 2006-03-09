
#ifndef SYNTHESE_CBASERESERVATIONS_H
#define SYNTHESE_CBASERESERVATIONS_H

class cBaseReservation;

#include "cReservation.h"
#include "cServeurBaseDeDonnees.h"

/** Base de r�servations pour transport � la demande
 @ingroup m31
*/
class cBaseReservation
{
        //! \name Param�tres
        //@{
        cServeurBaseDeDonnees* _Serveur;   //!< Serveur de base de donn�es � utiliser
        std::string _TableReservations; //!< Nom de la table des r�servations
        std::string _TableCompteur;  //!< Nom de la table des compteurs de r�servations
        std::string _NomChampID;  //!< Nom du champ d'identification de la r�servation
        //@}

    public:
        //! name Calculateurs
        //@{
        bool EnregistrerReservation( cReservation& ) const;
        bool LireReservation( cReservation& ) const;
        cTableauDynamique<cReservation> LireReservations( const cTrain*, const synthese::time::DateTime& ) const;
        //@}
};

#endif
