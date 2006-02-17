/*! \file cTrain.cpp
	\brief Impl�mentation classe cTrain
	\author Hugues Romain
	\date 2000-2003
*/

#include "cTrain.h"


/*! \brief Constructeur
*/
cTrain::cTrain()
{
	vEtalementCadence = 0;
	vAttente = 0;
	vEstCadence = false;
}


/*! \brief Destructeur
*/
cTrain::~cTrain()
{
}

/*! \brief V�rification de crit�res d'utilisation du service par rapport � ses modalit�s de r�servation
	\param __MomentDepart Heure du d�part souhait�
	\param __MomentCalcul Moment du calcul pris comme r�f�rence pour le calcul du d�lai de r�servation
	\return true si le service peut �tre r�serv� : 
		- soit la ligne n'a pas de modalit� de r�servation
		- soit la modalit� de r�servation accepte les conditions
*/
bool cTrain::ReservationOK(const cMoment& __MomentDepart, const cMoment& __MomentCalcul) const
{
	return this->getLigne()->GetResa() == NULL
		? true
		: this->getLigne()->GetResa()->circulationPossible(this, __MomentCalcul, __MomentDepart);
}



void cTrain::setNumero(const cTexte& newVal)
{
	vNumero.Vide();
	vNumero << newVal;
}

void cTrain::setServiceContinu()
{
	vEstCadence = true;
}

cJC* cTrain::setJC(cJC* newVal)
{
	vCirculation = newVal;
	return(newVal);
}

cJC* cTrain::getJC() const
{
	return(vCirculation);
}

bool cTrain::EstCadence() const
{
	return(vEstCadence);
}

const tDureeEnMinutes& cTrain::EtalementCadence() const
{
	return(vEtalementCadence);
}

const tDureeEnMinutes& cTrain::Attente() const
{
	return(vAttente);
}

void cTrain::setAttente(const tDureeEnMinutes& newVal)
{
	vAttente = newVal;
}

void cTrain::setAmplitudeServiceContinu(const tDureeEnMinutes& newVal)
{
	//#ifndef OS_LINUX
	//_ASSERTE(newVal >= 0 && newVal <= MinutesParJour);
	//#endif
	vEtalementCadence = newVal;
}

const cTexte& cTrain::getNumero() const
{
	return(vNumero);
}

cLigne* cTrain::getLigne() const
{
	return(vLigne);
}

void cTrain::setLigne(cLigne* newVal)
{
	vLigne = newVal;
}

/*!	\brief Horaire de d�part de la circulation
	\author Hugues Romain
	\date 2005
	\return Horaire de d�part de la circulation (premier d�part pour un service continu)
*/
const cHoraire* cTrain::getHoraireDepartPremier() const
{
	return _HoraireDepart;
}

void cTrain::setHoraireDepart(cHoraire* Ptr)
{
	_HoraireDepart = Ptr;
}


/*!	\brief Calcul de la circulation du train le jour donn�
	\param __DateDepart Date de d�part du voyageur
	\param __JPlus Nombre de jours calendaires s�parant la date de d�part du voyageur de la date de d�part de l'origine du train (r�f�rence du calendrier de circulation)
*/
bool cTrain::Circule(const cDate& __DateDepart, tDureeEnJours __JPlus) const
{
	if (__JPlus > 0)
	{
		cDate __DateDepartOrigine;
		__DateDepartOrigine = __DateDepart;
		__DateDepartOrigine -= __JPlus;
		return vCirculation->Circule(__DateDepartOrigine);
	}
	else
		return vCirculation->Circule(__DateDepart);
}