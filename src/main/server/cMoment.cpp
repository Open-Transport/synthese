
#include "cMoment.h"

/*!	\brief Crée un moment à partir d'une chaine de caractères
	\param Moment si "m" alors on stocke le minimum (0000...) si "M" alors on stocke le maximum
*/
cMoment& cMoment::operator = (const cTexte& Moment)
{
	if (!Moment.Taille())
	{
		vDate = Moment;
		vHeure = Moment;
	}
	else if (Moment[0] == '?')
		setMoment('?', '?', '?', '?', '?');
	else if (Moment[0] == 'm')
		setMoment('m', 'm', 'm', 'm', 'm');
	else if (Moment[0] == 'M')
		setMoment('M', 'M', 'M', 'M', 'M');
	else if (Moment.Taille()==19) // date SQL
	{
		cTexte temp;
		temp << Moment.Extrait(0,4) << Moment.Extrait(5,2) << Moment.Extrait(8,2);
		vDate = temp;
		temp.Vide();
		temp << Moment.Extrait(11,2) << Moment.Extrait(14,2);
		vHeure = temp;
	}
	else
	{
		vDate = Moment;
		vHeure = Moment.Taille() < 8 ? Moment : Moment.Extrait(8);
	}
	return(*this);
}


/** Durée en minutes entre deux moments.
	@param Op2 Moment le plus tôt
*/
tDureeEnMinutes cMoment::operator - (const cMoment& Op2) const
{
	tDureeEnMinutes DureeCalculee;
	int Retenue = 0;
	
	// 1: Heure
	DureeCalculee = vHeure - Op2.vHeure;
	if (DureeCalculee < 0)
	{
		Retenue = 1;
		DureeCalculee += MINUTES_PAR_JOUR;
	}

	// 2: JPlus
	DureeCalculee += ((vDate - Op2.vDate) - Retenue) * MINUTES_PAR_JOUR;
	return(DureeCalculee);
}

cMoment& cMoment::setHeure(const cTexte& __Texte)
{
	vHeure = __Texte;
	return *this;
}


cMoment& cMoment::setHeure(const cHeure& __Heure)
{
	vHeure = __Heure;
	return *this;
}
