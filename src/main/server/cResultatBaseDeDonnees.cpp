
#include "cResultatBaseDeDonnees.h"


bool cResultatBaseDeDonnees::AddEnregistrement()
{
	tResultatBaseDeDonnees_Enregistrement __Enregistrement;
	_EnregistrementCourant = _Donnees.SetElement(__Enregistrement);
	return _EnregistrementCourant != INCONNU;
}

bool cResultatBaseDeDonnees::SetValeur(tIndex __Position, cResultatBaseDeDonnees_Cellule* __Cellule)
{
	return _Donnees.getElement(_EnregistrementCourant).SetElement(__Cellule, __Position) != INCONNU;
}

void cResultatBaseDeDonnees::Vide()
{
	_Donnees.Vide();
	_EnregistrementCourant = INCONNU;
	_PositionChamps.clear();
}

bool cResultatBaseDeDonnees::SetPositionChamp(char* __NomChamp, tIndex __Position)
{
	_PositionChamps[__NomChamp] = __Position;
	return true;
}

int cResultatBaseDeDonnees::getNombre(const cTexte& __Champ, tIndex __Index)
{
	// Index
	if (__Index == INCONNU)
		__Index = _EnregistrementCourant;
	if (!_Donnees.IndexValide(__Index))
		return INCONNU;

	// Résultat
	return _Donnees[__Index][_PositionChamps[__Champ.Texte()]]->getNombre();
}



cMoment cResultatBaseDeDonnees::getMoment(const cTexte& __Champ, tIndex __Index)
{
	// Index
	if (__Index == INCONNU)
		__Index = _EnregistrementCourant;
	if (!_Donnees.IndexValide(__Index))
		return cMoment();

	// Résultat
	return _Donnees[__Index][_PositionChamps[__Champ.Texte()]]->getMoment();
}



cTexte cResultatBaseDeDonnees::getTexte(const cTexte& __Champ, tIndex __Index)
{
	// Index
	if (__Index == INCONNU)
		__Index = _EnregistrementCourant;
	if (!_Donnees.IndexValide(__Index))
		return cTexte();

	// Résultat
	return _Donnees[__Index][_PositionChamps[__Champ.Texte()]]->getTexte();
}



tBool3 cResultatBaseDeDonnees::getBool(const cTexte& __Champ, tIndex __Index)
{
	// Index
	if (__Index == INCONNU)
		__Index = _EnregistrementCourant;
	if (!_Donnees.IndexValide(__Index))
		return Indifferent;

	// Résultat
	return _Donnees[__Index][_PositionChamps[__Champ.Texte()]]->getBool();
}


bool cResultatBaseDeDonnees::GoPremier()
{
	if (_Donnees.Taille())
	{
		_EnregistrementCourant = 0;
		return true;
	} // if (_Donnees.Taille())
	else
	{
		_EnregistrementCourant = INCONNU;
		return false;
	}
}

bool cResultatBaseDeDonnees::GoSuivant()
{
	if (_EnregistrementCourant + 1 < _Donnees.Taille())
	{
		_EnregistrementCourant++;
		return true;
	}
	else
	{
		_EnregistrementCourant = INCONNU;
		return false;
	}
}

const tPositionChamps& cResultatBaseDeDonnees::getPositionsChamps()	const
{
	return _PositionChamps;
}
