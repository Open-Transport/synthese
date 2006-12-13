
#include "cProfil.h"

/** User session.
 @ingroup m70
*/
class cSession
{
        std::string _ID;
        tAdresseIP _IP;
        cProfil* _Proprietaire;
        cCalculateur* _CalculateurOccupe;
        bool _CouperCalcul;
        cSite* _Site;

    public:
        const cProfil* getProprietaire();

        //! Injecter ce test à chaque recursion de calculitineraire
        bool CalculPeutContinuer();

        /*! Controle si un calculateur est occupe si oui coupe le calcul */
        bool ExecuteRequete( const std::stringRequete& );


        //! \name Actions
        bool ModifieProfil( cProfil*, std::string __User, std::string __Password );

        bool AnnuleReservation( cReservation& );

};

bool cSession::ExecuteRequete( const std::stringRequete& __Requete, tAdresseIP __IP )
{
    if ( __IP != _IP )
        return false;

    _Site->ExecuteRequete( __Requete );
}

