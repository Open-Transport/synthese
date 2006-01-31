/** En-tête classes Temps.
	@file Temps.h
*/

#ifndef SYNTHESE_TEMPS_H
#define SYNTHESE_TEMPS_H

// Inclusions bibliotheques
#include <iostream>
#include "Parametres.h"
#include "cTexte.h"

/** @defgroup m04 04 Gestion du temps
	@{
*/

//! @name Caractères de codage de commandes
//@{
#define TEMPS_MIN		'm'
#define TEMPS_MAX		'M'
#define TEMPS_INCHANGE	'_'
#define TEMPS_IDEM		'I'
#define TEMPS_INCONNU	'?'
#define TEMPS_ACTUEL	'A'
//@}

//! @name Constantes
//@{
#define ANNEE_MAX			9999
#define MOIS_PAR_AN			12
#define MINUTES_PAR_HEURE	60
#define HEURES_PAR_JOUR		24
#define MINUTES_PAR_JOUR	1440
#define JOURS_PAR_SEMAINE	7
//@}

typedef int			tDureeEnJours;
typedef int 		tDureeEnMinutes;
typedef short int	tJour;
typedef short int	tMois;
typedef short int	tAnnee;
typedef short int	tMinute;
typedef short int	tHeure;
class cAnnee;
class cMois;
class cMoment;
class cDureeEnMinutes;
class cDate;
class cHoraire;
class cHeure;

/** @} */

// Inclusion des classes de temps
#include "cDureeEnMinutes.h"
#include "cMoment.h"


#endif
