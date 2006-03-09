
#include "cProfil.h"

bool cProfil::ResaAutorisee( const cReservation& __Reservation ) const
{
    return _ResaAll || _ResaPerso && __Reservation.GetPersonne() == this;
}



std::vector<cReservation> cProfil::ListeReservations( bool _DontPassees ) const
{
    std::vector<cReservation> __Reservations;

    /*!
    \todo CRO REQUETE MNESIA LISTE RESA PRISES PAR LE PROFIL, AVEC DATE DEPART PASSEE = _DontPassees
    */

    //__Reservations.Trie(cReservation::MomentDepart);
    //! \todo Instaurer un tri standard dans cTableauDynamique et l'appliquer ici sur les moments de départ

    return __Reservations;
}




bool cProfil::SetLogin( const std::string& __User, const std::string& __Password )
{
    _User = __User;
    _Password = __Password;
    return _User.size () == __User.size () && _Password.size () == __Password.size ();
}

bool cProfil::GestionPersonneAutorisee() const
{
    return _GestionPersonne;
}

bool cProfil::GestionProfilNonPersoAutorisee() const
{
    return _GestionProfilNonPerso;
}

cProfil::cProfil()
{
    _Enregistre = false;

    _ResaPerso = false;
    _ResaAll = false;
    _GestionPersonne = false;
    _GestionProfilNonPerso = false;
    _GestionProfilAdmin = false;
    _ViewDetailResaSansCode = false;
}

bool cProfil::PasswordCorrect( const std::string& __Password ) const
{
    return _Password == __Password;
}

