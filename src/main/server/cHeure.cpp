
#include "Temps.h"
#include <time.h>

/*! \brief Retourne le nombre de minutes écoulées entre les deux heures

	Si this \< retourne la durée négative.

	\author Hugues Romain
	\date 2000/2001
*/
tDureeEnMinutes cHeure::operator-(const cHeure &Op2) const
{
	tDureeEnMinutes DureeCalculee;
	int Retenue;
	
	// 1: Minutes
	Retenue = (MINUTES_PAR_HEURE - 1 + Op2.vMinutes - vMinutes) / MINUTES_PAR_HEURE;
	DureeCalculee += vMinutes + Retenue * MINUTES_PAR_HEURE - Op2.vMinutes;

	// 2: Heures
	DureeCalculee += MINUTES_PAR_HEURE * (vHeures - Op2.vHeures - Retenue);

	return DureeCalculee;
}

/*! \brief Ajoute "DureeEnMinutes" minutes à l'heure puis retourne le nombre de jours à ajouter si besoin (Retenue)
	\warning NE PREND QUE DES DUREES POSITIVES (facile à modifier)
	\author Hugues Romain 
	\date 2000/2001
	*/		
tDureeEnJours cHeure::operator+=(const tDureeEnMinutes& DureeEnMinutesAAjouter)
{	
	int Retenue;
	int tempCalcul;

	// 1: Minutes
	tempCalcul = vMinutes + DureeEnMinutesAAjouter.Valeur();
	Retenue = tempCalcul / MINUTES_PAR_HEURE;
	vMinutes = (tMinute) (tempCalcul % MINUTES_PAR_HEURE);
	
	// 2: Heures
	tempCalcul = vHeures + Retenue;
	Retenue = tempCalcul / HEURES_PAR_JOUR;
	vHeures = (tHeure) (tempCalcul % HEURES_PAR_JOUR);
	return(Retenue);
}

/*! \brief Retire "DureeEnMinutes" minutes à l'heure puis retourne le nombre de jours à enlever si besoin (Retenue)
	\warning NE PREND QUE DES DUREES POSITIVES (facile à modifier)
	\author Hugues Romain 
	\date 2000/2001
	*/
tDureeEnJours cHeure::operator-=(const tDureeEnMinutes& DureeEnMinutesAEnlever)
{
	int HeuresAEnlever;
	int Retenue;
	
	// 1: Minutes
	Retenue = (MINUTES_PAR_HEURE - 1 + DureeEnMinutesAEnlever.Valeur() - vMinutes) / MINUTES_PAR_HEURE;
	vMinutes = (tMinute) (Retenue * MINUTES_PAR_HEURE + vMinutes - DureeEnMinutesAEnlever.Valeur());
	HeuresAEnlever = Retenue;

	// 2: Heures
	Retenue = (HEURES_PAR_JOUR - 1 + HeuresAEnlever - vHeures) / HEURES_PAR_JOUR;
	vHeures = (tHeure) (Retenue * HEURES_PAR_JOUR + vHeures - HeuresAEnlever);
	return(Retenue);
}

/*! \param Op Si ="m" alors on stocke le minimum (0000...) Si ="M" alors on stocke le maximum
*/
cHeure& cHeure::operator = (const cTexte& op)
{
	if (!op.Taille())
	{
		vHeures = 0;
		vMinutes = 0;
	}
	else if (op.Taille() == 1)
		setHeure(op[0]);
	else
	{
		vHeures	= (tHeure)	op.GetNombre(2);
		vMinutes	= (tMinute)	op.GetNombre(2, 2);
	}
	return(*this);
}

/*!	\brief Modification de l'heure
	\param newHeure Heures (A = actuel, M=maximum, m=minimum, ?=inconnu, _=inchangé)
	\param newMinutes Minutes (A = actuel, M=maximum, m=minimum, ?=inconnu, _=inchangé, I=Comme heures)
*/
void cHeure::setHeure(tHeure newHeure, tMinute newMinutes)
{
	time_t rawtime;
	struct tm * timeinfo = NULL;
	
	if (newHeure == TEMPS_ACTUEL || newMinutes == TEMPS_ACTUEL)
	{
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
	}
	
	if (newHeure == TEMPS_ACTUEL)
		vHeures = (*timeinfo).tm_hour;
	else if (newHeure == TEMPS_MAX)
		vHeures = 23;
	else if (newHeure == 'm')
		vHeures = 0;
	else if (newHeure == '?')
		vHeures = INCONNU;
	else if (newHeure != '_')
		vHeures = newHeure;
	
	if (newMinutes == TEMPS_ACTUEL || newMinutes == 'I' && newHeure == TEMPS_ACTUEL)
		vMinutes = (*timeinfo).tm_min;
	else if (newMinutes == TEMPS_MAX || newMinutes == 'I' && newHeure == TEMPS_MAX)
		vMinutes = 59;
	else if (newMinutes == 'm' || newMinutes == 'I' && newHeure == 'm')
		vMinutes = 0;
	else if (vMinutes == '?' || newMinutes == 'I' && newHeure == '?')
		vMinutes = INCONNU;
	else if (newMinutes != '_' && (newMinutes != 'I' || newHeure != '_'))
		vMinutes = newMinutes;
}
