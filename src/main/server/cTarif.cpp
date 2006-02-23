
#include "cTarif.h"

/*! \brief Constructeur
*/
cTarif::cTarif(const size_t& id)
: _id(id)
{
}

void cTarif::setTypeTarif(const tTypeTarif newVal)
{
	vTypeTarif = newVal;
}

tTypeTarif cTarif::getTypeTarif() const
{
	return(vTypeTarif);
}

void cTarif::setLibelle(const cTexte& libelle)
{
	vLibelle.Vide();
	vLibelle << libelle;
}

const cTexte& cTarif::getLibelle() const
{
	return (vLibelle);
}

size_t cTarif::getNumeroTarif() const
{
	return _id;
}

