
#include "Temps.h"

/*! \brief Cet op�rateur remplit les variables de l'objet d'apr�s les 5 premiers caract�res de op.
	\warning l'autorisation d'acc�s aux 5 caract�res est � la charge de l'utilisateur.
	\param op  Si ="m" alors on stocke le minimum (0000...) Si ="M" alors on stocke le maximum
	*/
cHoraire& cHoraire::operator = (const cTexte& op)
{
	if (!op.Taille())
	{
		vJPlus = 0;
		vHeure = op;
	}
	else if (op[0] == 'm')
		setMinimum();
	else if (op[0] == 'M')
		setMaximum();
	else
	{
		vJPlus = (tDureeEnJours) op.GetNombre(1);
		vHeure = op.Extrait(1);
	}
	return(*this);
}



/*! \brief Retourne le nombre de minutes �coul�es entre les deux horaires

	\return Si this \< Op2, retourne la dur�e n�gative.

	\author Hugues Romain 
	\date 2000/2001
	*/
tDureeEnMinutes cHoraire::operator - (const cHoraire& Op2) const
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
	DureeCalculee += (vJPlus - Op2.vJPlus - Retenue) * MINUTES_PAR_JOUR;
	return(DureeCalculee);
}
