
#ifndef SYNTHESE_CBASERESERVATIONS_H
#define SYNTHESE_CBASERESERVATIONS_H

class cBaseReservation;

#include "cReservation.h"
#include "cServeurBaseDeDonnees.h"

/** Base de réservations pour transport à la demande
 @ingroup m31
*/
class cBaseReservation
{
        //! \name Paramètres
        //@{
        cServeurBaseDeDonnees* _Serveur;   //!< Serveur de base de données à utiliser
        std::string _TableReservations; //!< Nom de la table des réservations
        std::string _TableCompteur;  //!< Nom de la table des compteurs de réservations
        std::string _NomChampID;  //!< Nom du champ d'identification de la réservation
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
