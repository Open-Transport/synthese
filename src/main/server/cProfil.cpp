
#include "cProfil.h"

bool cProfil::ResaAutorisee(const cReservation& __Reservation) const
{
	return _ResaAll || _ResaPerso && __Reservation.GetPersonne() == this;
}

cTableauDynamiqueObjets<cReservation> cProfil::ListeReservations(bool _DontPassees) const
{
	cTableauDynamiqueObjets<cReservation> __Reservations;

	/*!
	\todo CRO REQUETE MNESIA LISTE RESA PRISES PAR LE PROFIL, AVEC DATE DEPART PASSEE = _DontPassees
	*/

	//__Reservations.Trie(cReservation::MomentDepart);
	//!	\todo Instaurer un tri standard dans cTableauDynamique et l'appliquer ici sur les moments de départ
	
	return __Reservations;
}

bool cProfil::SetLogin(const cTexte& __User, const cTexte& __Password)
{
	_User = __User;
	_Password = __Password;
	return _User.Taille() == __User.Taille() && _Password.Taille() == __Password.Taille();
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

bool cProfil::PasswordCorrect(const cTexte& __Password) const
{
	return _Password.Compare(__Password);
}

