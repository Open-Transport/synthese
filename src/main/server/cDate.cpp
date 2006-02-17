/** Impl�mentation classe Date
	@file cDate.cpp
*/

#include "Temps.h"
#include <time.h>
#include <math.h>

// La doc se trouve dans le .h car Doxygen ne la lit pas sinon
cDate& cDate::setDate(tJour Jour, tMois Mois, tAnnee Annee)
{
	// Variables locales
	time_t rawtime;
	struct tm * timeinfo = NULL;
	
	// Collecte de la date du jour si besoin
	if (Jour == TEMPS_ACTUEL || Mois == TEMPS_ACTUEL || Annee == TEMPS_ACTUEL)
	{
		time ( &rawtime );
  		timeinfo = localtime ( &rawtime );
	}

	// R�glage du mois
	if (Mois == TEMPS_ACTUEL || Mois == TEMPS_IDEM && Jour == TEMPS_ACTUEL)
		_Mois = (*timeinfo).tm_mon+1;	
	else if (Mois == TEMPS_MAX || Mois == TEMPS_IDEM && Jour == TEMPS_MAX)
		_Mois = MOIS_PAR_AN;
	else if (Mois == TEMPS_MIN || Mois == TEMPS_IDEM && Jour == TEMPS_MIN)
		_Mois = 1;
	else if (Mois == TEMPS_INCONNU || Mois == TEMPS_IDEM && Jour == TEMPS_INCONNU)
		_Mois = INCONNU;
	else if (Mois != TEMPS_INCHANGE && (Mois != TEMPS_IDEM || Jour != TEMPS_INCHANGE) && Mois >= 1 && Mois <= MOIS_PAR_AN)
		_Mois = Mois;
	
	// R�glage de l'ann�e
	if (Annee == TEMPS_ACTUEL || Annee == TEMPS_IDEM && Jour == TEMPS_ACTUEL)
		_Annee= (*timeinfo).tm_year+1900;
	else if (Annee == TEMPS_MAX || Annee == TEMPS_IDEM && Jour == TEMPS_MAX)
		_Annee = ANNEE_MAX;
	else if (Annee == TEMPS_MIN || Annee == TEMPS_IDEM && Jour == TEMPS_MIN)
		_Annee = 1;
	else if (Annee == TEMPS_INCONNU || Annee == TEMPS_IDEM && Jour == TEMPS_INCONNU)
		_Annee = INCONNU;
	else if (Annee != TEMPS_INCHANGE && (Annee != TEMPS_IDEM || Jour != TEMPS_INCHANGE) && Annee >= 0 && Annee <= ANNEE_MAX)
		_Annee = Annee;
	
	// R�glage du jour
	if (Jour == TEMPS_ACTUEL)
		_Jour = (tJour)((*timeinfo).tm_mday);	
	else if (Jour == TEMPS_MAX)
		_Jour = _Mois.NombreJours(_Annee);
	else if (Jour == TEMPS_MIN)
		_Jour = 1;
	else if (Jour == TEMPS_INCONNU)
		_Jour = INCONNU;
	else if (Jour != TEMPS_INCHANGE && Jour >= 1 && Jour <= 31)
		_Jour = Jour;
	
	// Sortie
	return *this;
}



/*!	\brief Ajour d'un jour � la date
	\return La date modifi�e
	\author Hugues Romain
	\date 2001
*/
cDate& cDate::operator++(int)
{
	_Jour++;
	if (Jour() > _Mois.NombreJours(_Annee))
	{
		_Jour = 1;
		_Mois++;
		if (Mois() > MOIS_PAR_AN)
		{
			_Mois = 1;
			_Annee++;
		}
	}
	return(*this);
}



/*!	\brief D�cr�mentation d'un jour � la date
	\return La date modifi�e
	\author Hugues Romain
	\date 2001
*/
cDate& cDate::operator--(int)
{
	_Jour--;
	if (Jour() == 0)
	{
		_Mois--;
		if (Mois() == 0)
		{
			_Annee--;
			_Mois = 12;
		}
		_Jour = _Mois.NombreJours(_Annee);
	}
	return *this;
}




/*!	\brief Ajout de jours � la date
	\param JoursSupp Nombre de jours � ajouter
	\return La date modifi�e
	\author Hugues Romain
	\date 2001
*/
cDate& cDate::operator+=(tDureeEnJours JoursSupp)
{
	// A optimiser
	for (; JoursSupp!=0; JoursSupp--)
		operator++(0);
	return(*this);
}

cDate& cDate::operator-=(tDureeEnJours JoursSupp)
{
	// A optimiser
	for (; JoursSupp!=0; JoursSupp--)
		operator--(0);
	return(*this);
}

/*!	\brief Modification de la date � partir d'une cha�ne de caract�res
	\param __Valeur La cha�ne de caract�res contenant la valeur � donner � la date (format interne)
	\return R�f�rence vers l'objet
	\author Hugues Romain
	\date 2000-2005
	
- Si __Valeur est vide alors on stocke la date inconnue (-1/-1/-1)
- Si __Valeur est une commande de date elle est int�rpr�t�e (\ref cDate::setDate() )
- Si __Valeur est une date au codage interne (8 caract�res) alors la date est modifi�e selon son contenu
*/
cDate& cDate::operator = (const cTexte& __Valeur)
{
	switch (__Valeur.Taille())
	{
	case 0:
		setDate(TEMPS_INCONNU);
		break;
		
	case 1:
		setDate(__Valeur[0]);
		break;
		
	default:
		setDate(__Valeur.GetNombre(2, 6), __Valeur.GetNombre(2, 4), __Valeur.GetNombre(4));
		break;
	}
	return *this;
}


tDureeEnJours cDate::operator - (const cDate& Op2) const
{
	if (*this < Op2)
		return(-(Op2 - *this));

	// PROVISOIRE
	if ((Mois() != Op2.Mois()) || (Annee() != Op2.Annee()))
		return	_Mois.NombresJoursJusquaFinMois(Jour(), _Annee) 
			+ 	Op2._Mois.NombresJourJusquAMois(Op2._Annee,_Mois, _Annee) 
			+	Op2.Jour();
	else
		return Jour() - Op2.Jour();
}

/*!	\brief Calcule le jour de la semaine de la date
	\return 0=Dimanche, 1=Lundi, ... , 6=Samedi
 */
char cDate::JourSemaine() const
{
	int mz = Mois() - 2;
	int az = Annee();
	if (mz <= 0)
	{
		mz += MOIS_PAR_AN;
		az --;
	}
	int s = az / 100;
	int e = az % 100;
	
	int J = Jour() + (int) floor(2.6 * mz - 0.2);
	J += e + (e/4) + (s / 4) - 2 * s;
	if (J >= 0)
		J %= 7;
	else
	{
		J = (-J) % 7;
		if (J > 0)
			J = 7 - J;
	}
	return J;
}
