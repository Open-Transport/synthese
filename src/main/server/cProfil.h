
#ifndef SYNTHESE_CPROFIL_H
#define SYNTHESE_CPROFIL_H

class cProfil;

#include <string>
#include <vector>

#include "cReservation.h" 
//#include "cAxeTAD.h"    CRO supprim� car pas de param�tres au constructeur
#include "cAxe.h"  // CRO
#include "cTarif.h"
#include "Parametres.h"

/** @ingroup m10 */
class cProfil : public cPersonne
{
    protected:
        //! \name login
        //@{
        std::string _User;
        std::string _Password;
        //@}

        //! \name droits
        //@{
        bool _ResaPerso;     //!< Droit de prendre des r�servations pour soi
        bool _ResaAll;     //!< Droit de prendre des r�servations pour tous

        std::vector<cAxe*> _ViewResasAxe;    //!< Droit de consulter les r�servations des axes TAD
        bool _GestionPersonne;   //!< Droit d'administrer une personne
        bool _GestionProfilNonPerso;  //!< Droit d'administrer le profil d'un autre
        bool _GestionProfilAdmin;  //!< Droit d'administrer le profil d'un administrateur
        bool _ViewDetailResaSansCode; //!< Droit d'ouvrir une r�sa sans fournir le code
        bool _Enregistre;    //!< Etat enregistr� sur la base
        //@}

        //! \name Filtres par d�faut
        //@{
        tBool3 _FiltreVelo;
        tBool3 _FiltreHandicape;
        cTarif* _FiltreTarif;
        //@}

    public:
        //! \name Calculateurs controle des droits
        //@{
        bool PasswordCorrect( const std::string& ) const;
        bool ResaAutorisee( const cReservation& ) const;
        bool GestionPersonneAutorisee() const;
        bool GestionProfilNonPersoAutorisee() const;
        //@}

        //! \name Rapports
        //@{
        std::vector<cReservation> ListeReservations( bool _DontPassees = false ) const;
        //@}

        //! \name Modificateurs du profil
        //@{
        bool SetLogin( const std::string& __User, const std::string& __Password );
        //@}

        cProfil();
};

#endif

