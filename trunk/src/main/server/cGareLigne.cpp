/*! \file cGareLigne.cpp
	\brief Impl�mentation Classe GareLigne
*/

#include "cGareLigne.h"

/** Constructeur.
	\param newLigne Ligne � laquelle appartient l'objet
	\param newPH	Positionnement lin�aire : Point hectom�trique de l'arr�t sur la ligne
	\param newArretPhysique	Num�ro de l'arr�t physique desservi par la ligne au sein de l'arr�t logique
	\param newType	Type de desserte : autorisation de d�part et/ou d'arriv�e � l'arr�t
	\param newPA	Arr�t logique desservi par la ligne
	\author Hugues Romain
	\date 2001
*/
cGareLigne::cGareLigne(cLigne* newLigne, tDistanceM newPM, tIndex newArretPhysique, tTypeGareLigneDA newType, LogicalPlace* newPA, bool newHorairesSaisis)
{
	vDepartCorrespondancePrecedent = NULL;
	vDepartPrecedent = NULL;
	vArriveeCorrespondanceSuivante = NULL;
	vArriveeSuivante = NULL;
	vPAArriveeAxeSuivante = NULL;
	vPAArriveeSuivante = NULL;
	vPADepartAxeSuivant = NULL;
	vPADepartSuivant = NULL;
	vSuivant = NULL;
	_Precedent = NULL;

	// Ligne
	
	setTypeDA(newType);		   // Type
	vPM = newPM;			  // PM
	vVoie = newArretPhysique;		 // ArretPhysique
	vArretLogique = newPA;	// Point d'arr�t	

	vHorairesSaisis = newHorairesSaisis;

	// Uniquement si ligne contenant des services (pas une route)
	if (newLigne->NombreServices())
	{
		if (EstDepart())
			setPADepartSuivant();
		if (EstArrivee())
			setPAArriveeSuivant();
		AlloueHoraires();
	}

}


/*! \brief Constructeur - Cr�e une instance de cGareLigne sur la ligne, les autres informations �tant copi�es d'apr�s une autre ligne
	\param newLigne Ligne � laquelle appartient l'objet
	\param GLACopier Objet � copier
	\author Hugues Romain
	\date 2001
*/
cGareLigne::cGareLigne(cLigne* newLigne, const cGareLigne& GLACopier)
{
	vDepartCorrespondancePrecedent = NULL;
	vDepartPrecedent = NULL;
	vArriveeCorrespondanceSuivante = NULL;
	vArriveeSuivante = NULL;
	vPAArriveeAxeSuivante = NULL;
	vPAArriveeSuivante = NULL;
	vPADepartAxeSuivant = NULL;
	vPADepartSuivant = NULL;
	vSuivant = NULL;
	_Precedent = NULL;
	vLigne = newLigne;
	vPM = GLACopier.vPM;
	vVoie = GLACopier.vVoie;
	vTypeDA = GLACopier.vTypeDA;
	vArretLogique = GLACopier.vArretLogique;
	vHorairesSaisis = GLACopier.vHorairesSaisis;
	if (newLigne->NombreServices())
	{
		if (EstDepart())
			setPADepartSuivant();
		if (EstArrivee())
			setPAArriveeSuivant();
		AlloueHoraires();
	}
}


/*! 	\brief Constructeur : cr�e une instance de cGareLigne sur la ligne fournie, sans sp�cification de point d'arr�t
	\param newLigne Ligne � laquelle appartient la gareligne
	\version 2.0
	\author Hugues Romain
	\date 2001
*/
/*cGareLigne::cGareLigne(cLigne* newLigne)
{
	vSuivant = NULL;
	_Precedent = NULL;
	vDepartCorrespondancePrecedent = NULL;
	vDepartPrecedent = NULL;
	vArriveeCorrespondanceSuivante = NULL;
	vArriveeSuivante = NULL;
	vPAArriveeAxeSuivante = NULL;
	vPAArriveeSuivante = NULL;
	vPADepartAxeSuivant = NULL;
	vPADepartSuivant = NULL;
	vPM=0;
	vLigne = newLigne;
	if (newLigne->NombreServices())
		AlloueHoraires();
}
*/

/*! \brief Destructeur
*/
cGareLigne::~cGareLigne()
{
	delete vSuivant;
	delete[] vHoraireArriveeDernier;
	delete[] vHoraireArriveePremier;
	delete[] vHoraireArriveePremierReel;
	delete[] vHoraireDepartDernier;
	delete[] vHoraireDepartPremier;
	delete[] vHoraireDepartPremierReel;
}


/*! 	\brief Remplit les horaires � partir du tampon
	\param Tampon Texte � interpr�ter
	\param Position Premier caract�re o� doit commencer l'analyse
	\param LargeurColonne Nombre de carac�res entre chaque horaire, donn�es comprises
	\param DepartDifferentPassage Indique si la fonction est appell�e une seconde fois pour d�crire les horaires de d�part apr�s avoir d�crit ceux d'arriv�e uniquement
	\version 3.0
	\author Hugues Romain
	\date 2003

	L'emploi de DepartDifferentPassage emp�che l'�crasement des horaires d'arriv�e dans le cas o� les horaires de d�part sont fournis sur une autre ligne de texte.
*/
void cGareLigne::setHoraires(const cTexte& Tampon, tIndex Position, tIndex LargeurColonne, bool DepartDifferentPassage)
{
	for (tNumeroService iService=0; iService<vLigne->NombreServices(); iService++)
	{
		vHoraireDepartPremier[iService]	    = Tampon.Extrait(Position);
		vHoraireDepartPremierReel[iService]	= vHoraireDepartPremier[iService];
		vHoraireDepartDernier[iService]		= vHoraireDepartPremier[iService];
		vHoraireDepartDernier[iService]	   += vLigne->EtalementCadence(iService);
		if (!DepartDifferentPassage)
		{
			vHoraireArriveePremier[iService]	 = vHoraireDepartPremier[iService];
			// HR ESSAI PBFREQ2
			vHoraireArriveePremier[iService] += Attente(iService);
			vHoraireArriveePremierReel[iService] = vHoraireArriveePremier[iService];
			// HR ESSAI PBFREQ2
			vHoraireArriveePremierReel[iService] += Attente(iService);
			vHoraireArriveeDernier[iService]	 = vHoraireDepartDernier[iService];
			// HR ESSAI PBFREQ2
			vHoraireArriveeDernier[iService] += Attente(iService);
		}
		
		// Ecriture de l'horaire de d�part des circulations si au d�part de la ligne
		if (Ligne()->PremiereGareLigne() == this)
			Ligne()->setHoraireDepart(iService, &vHoraireDepartPremier[iService]);
		
		Position += LargeurColonne;
	}

	EcritIndexArrivee();
	EcritIndexDepart();
}



/*!	\brief Contr�le la pertinence des horaires entr�s
	\param GareLigneAvecHorairesPrecedente Pr�c�dent arr�t de la ligne pourvu d'horaires
	\return true si aucun probl�me n'a �t� d�tect�, false sinon
	\author Hugues Romain
	\version 1.1
	\date 2001-2005
*/
bool cGareLigne::controleHoraire(const cGareLigne* GareLigneAvecHorairesPrecedente) const
{
	tNumeroService iNumeroService;

	// Test de chronologie verticale
	if (GareLigneAvecHorairesPrecedente != NULL)
	{
		for (iNumeroService = 0; iNumeroService< vLigne->NombreServices(); iNumeroService++)
			if (vHoraireDepartPremier[iNumeroService] < GareLigneAvecHorairesPrecedente->vHoraireDepartPremier[iNumeroService])
				return false;
	}

	// Test de chronologie horizontale
	for (iNumeroService = 1; iNumeroService < vLigne->NombreServices(); iNumeroService++)
		if (vHoraireDepartPremier[iNumeroService] < vHoraireDepartPremier[iNumeroService-1])
			return false;

	// Test d'existence des heurs
	for (iNumeroService = 1; iNumeroService < vLigne->NombreServices(); iNumeroService++)
		if (!vHoraireArriveePremier[iNumeroService].OK() || !vHoraireDepartPremier[iNumeroService].OK())
			return false;

	// Succ�s
	return true;
}


/*! \brief Calcule les horaires aux arr�ts o� les donn�es n'ont pas �t� fournies par le fichier de donn�es
	\version 2.0
	\author Hugues Romain
	\date 2001-2003
	\param Precedent Arr�t pr�c�dent disposant des donn�es horaires
	\param Suivant Arr�t suivant disposant des donn�es horaires
	\param Position Rang de la gareligne � l'int�rieur du segment, utilis� pour l'interpolation au cas o� les PH ne sont pas document�s
	\param Nombre Nombre total de GareLigne � l'int�rieur du segment, utilis� pour l'interpolation au cas o� les PH ne sont pas document�s
	
  La fonction calcule les heures de d�part et d'arriv�e de chaque GareLigne en interpolant � partir des GareLigne Precedent et Suivant.
*/
void cGareLigne::ChaineAvecHoraireSuivant(const cGareLigne& AHPrecedent, const cGareLigne& AHSuivant, size_t Position, size_t Nombre, tNumeroService NumeroServiceATraiter)
{
	tNumeroService iService;

	// Coefficient pour interpolation
	float CoefficientPosition;
	if (AHPrecedent.vPM == AHSuivant.vPM)
		CoefficientPosition = ((float) Position) / Nombre;
	else
		CoefficientPosition = (((float) vPM - AHPrecedent.vPM)) / (AHSuivant.vPM - AHPrecedent.vPM);

	// Ecriture des arr�ts
	float DureeAAjouter;
	if (NumeroServiceATraiter==-1)
		iService=0;
	else
		iService=NumeroServiceATraiter;

	for (; NumeroServiceATraiter!=-1 && iService==NumeroServiceATraiter || NumeroServiceATraiter==-1 && iService< vLigne->NombreServices(); iService++)
	{
		// Calcul de l'horaire
// MODIF HR PBCOLDBLE SOL 2		DureeAAjouter = CoefficientPosition * (AHSuivant.vHoraireArriveePremier[iService] - AHPrecedent.vHoraireDepartPremier[iService]);
		DureeAAjouter = CoefficientPosition * (AHSuivant.vHoraireDepartPremier[iService] - AHPrecedent.vHoraireDepartPremier[iService]).Valeur();

		vHoraireDepartPremier[iService]		= 	AHPrecedent.vHoraireDepartPremier[iService];
		vHoraireDepartPremier[iService]		+= 	cDureeEnMinutes((int) floor(DureeAAjouter));
		vHoraireDepartPremierReel[iService]	= 	vHoraireDepartPremier[iService];
		vHoraireDepartDernier[iService]		= 	AHPrecedent.vHoraireDepartDernier[iService];
		vHoraireDepartDernier[iService]		+= 	cDureeEnMinutes((int) floor(DureeAAjouter));

// MODIF HR PBCOLDBLE SOL 2
		DureeAAjouter = CoefficientPosition * (AHSuivant.vHoraireArriveePremier[iService] - AHPrecedent.vHoraireArriveePremier[iService]).Valeur();

		vHoraireArriveePremier[iService]	 	=	AHPrecedent.vHoraireArriveePremier[iService];
		vHoraireArriveePremier[iService]		+= 	cDureeEnMinutes((int) ceil(DureeAAjouter));
		vHoraireArriveePremierReel[iService] 	=	vHoraireArriveePremier[iService];
		vHoraireArriveeDernier[iService]	 	=	AHPrecedent.vHoraireArriveeDernier[iService];
		vHoraireArriveeDernier[iService]		+= 	cDureeEnMinutes((int) ceil(DureeAAjouter));
	}

	EcritIndexArrivee();
	EcritIndexDepart();
}




/*!	\brief Ecriture des index de d�part de la gare ligne, � partir des horaires des arr�ts. 
	\version 2.1
	\author Hugues Romain
	\date 2001-2004

Ecrit tous les index d'un coup (contrairement � la version 1)
*/
void cGareLigne::EcritIndexDepart()
{
	tNumeroService iNumeroService;
	tHeure iNumeroHeure;

	tHeure DerniereHeure=25; // MODIF HR
	tNumeroService NumeroServicePassantMinuit = 0; // MODIF HR

	// RAZ
	for (iNumeroHeure = 0; iNumeroHeure < HEURES_PAR_JOUR; iNumeroHeure++)
		vIndexDepart[iNumeroHeure ] = -1;
	
	// Ecriture service par service
	for (iNumeroService = 0; iNumeroService< vLigne->NombreServices(); iNumeroService++)
	{
		if (vHoraireDepartDernier[iNumeroService].Heures() < DerniereHeure)
			NumeroServicePassantMinuit = iNumeroService;
		if (vHoraireDepartDernier[iNumeroService].Heures() >= vHoraireDepartPremier[iNumeroService].Heures())
		{
			for (iNumeroHeure = 0; iNumeroHeure <= vHoraireDepartDernier[iNumeroService].Heures(); iNumeroHeure++)
				if (vIndexDepart[iNumeroHeure]==-1 || vIndexDepart[iNumeroHeure] < NumeroServicePassantMinuit)
					vIndexDepart[iNumeroHeure] = iNumeroService;
		}
		else
		{
			for (iNumeroHeure = 0; iNumeroHeure < HEURES_PAR_JOUR; iNumeroHeure++)
				if (vIndexDepart[iNumeroHeure]==-1)
					vIndexDepart[iNumeroHeure] = iNumeroService;
		}
		DerniereHeure = vHoraireDepartDernier[iNumeroService].Heures();

	}

	// Ecriture du temps r�el
	for (iNumeroHeure = 0; iNumeroHeure<24; iNumeroHeure++)
		vIndexDepartReel[iNumeroHeure] = vIndexDepart[iNumeroHeure];

}




/*! \brief Ecriture des index d'arriv�e de la gare ligne, � partir des horaires des arr�ts.
	\author Hugues Romain
	\date 2001-2004
	\version 2.1
	
Ecrit tous les index d'un coup (contrairement � la version 1)
*/
void cGareLigne::EcritIndexArrivee()
{
	tNumeroService iNumeroService;
	tHeure iNumeroHeure;

	// RAZ
	for (iNumeroHeure = 0; iNumeroHeure < HEURES_PAR_JOUR; iNumeroHeure++)
		vIndexArrivee[iNumeroHeure] = -1;

	tHeure DerniereHeure=25;
	tNumeroService NumeroServicePassantMinuit = vLigne->NombreServices();
	
	for (iNumeroService = vLigne->NombreServices() - 1; iNumeroService >= 0; iNumeroService--)
	{
		if (vHoraireArriveePremier[iNumeroService].Heures() > DerniereHeure)
			NumeroServicePassantMinuit = iNumeroService;
		if (vHoraireArriveeDernier[iNumeroService].Heures() >= vHoraireArriveePremier[iNumeroService].Heures())
		{
			for (iNumeroHeure = vHoraireArriveePremier[iNumeroService].Heures(); iNumeroHeure < HEURES_PAR_JOUR; iNumeroHeure++)
				if (vIndexArrivee[iNumeroHeure]==-1 || vIndexArrivee[iNumeroHeure]> NumeroServicePassantMinuit)
					vIndexArrivee[iNumeroHeure] = iNumeroService;
		}
		else
		{
			for (iNumeroHeure = 0; iNumeroHeure < HEURES_PAR_JOUR; iNumeroHeure++)
				if (vIndexArrivee[iNumeroHeure]==-1)
					vIndexArrivee[iNumeroHeure] = iNumeroService;
		}
		DerniereHeure = vHoraireArriveePremier[iNumeroService].Heures();
	}
	
	// Ecriture du temps r�el
	for (iNumeroHeure = 0; iNumeroHeure< HEURES_PAR_JOUR; iNumeroHeure++)
		vIndexArriveeReel[iNumeroHeure] = vIndexArrivee[iNumeroHeure];
}



/*!	\brief Fournit le prochain d�part de la ligne � l'arr�t (m�thode 1)
	\version 2.0
	\param MomentDepartMax Heure de d�part � ne pas d�passer
	\param MomentDepart  Heure de pr�sence sur le lieu de d�part
	\param NumProchainMin	 Index o� commencer la recherche de prochain service
	\param __MomentCalcul Moment du calcul (pour v�rification du d�lai de r�servation
	\return Indice du service utilis� au sein de la ligne
	INCONNU (-1) = Aucun service trouv�
	\retval MomentDepart  Moment pr�cis du d�part. Inutilisable si INCONNU retourn�
	\date 2003-2005
	\author Hugues Romain
*/
tNumeroService cGareLigne::Prochain(
	cMoment &MomentDepart
	, const cMoment& MomentDepartMax
	, const cMoment& __MomentCalcul
	, tNumeroService NumProchainMin
) const {
	tNumeroService NumProchain;
	
	// Recherche de l'horaire
	// On se positionne par rapport � un tableau d'index par heure
	NumProchain = vIndexDepart[MomentDepart.Heures()];
	if (NumProchain == INCONNU)
		NumProchain = vLigne->NombreServices();
	
	if (NumProchainMin > NumProchain)
		NumProchain = NumProchainMin;

	while (MomentDepart <= MomentDepartMax) // boucle sur les dates
	{
		// Exploration des horaires si la ligne circule
		if (vLigne->PeutCirculer(MomentDepart.getDate()))
		{
			while (NumProchain < vLigne->NombreServices()) // boucle sur les services
			{
				// TEST 12
				// Cas JPlus != en service continu
				if (vLigne->EstCadence(NumProchain) && vHoraireDepartPremier[NumProchain].JPlus() != vHoraireDepartDernier[NumProchain].JPlus())
				{
					// Si service apres momentdepart alors modification
					if (MomentDepart > vHoraireDepartDernier[NumProchain] && MomentDepart < vHoraireDepartPremier[NumProchain])
						MomentDepart = vHoraireDepartPremier[NumProchain];

					// Possibilit� de sortie
					if (MomentDepart > MomentDepartMax)
						return INCONNU;

					// on a presque valid� le service, il faut v�rifier la possiblite eventuelle de r�servation
					if (vLigne->getTrain(NumProchain)->ReservationOK(MomentDepart, __MomentCalcul))
					{
						// Si le service circule retour du service
						if (MomentDepart < vHoraireDepartDernier[NumProchain])
						{
							if (vLigne->getTrain(NumProchain)->Circule(MomentDepart.getDate(), vHoraireDepartDernier[NumProchain].JPlus()))
								return NumProchain;
						}
						else
							if (vLigne->getTrain(NumProchain)->Circule(MomentDepart.getDate(), vHoraireDepartPremier[NumProchain].JPlus()))
								return NumProchain;
					}	
				} // Fin TEST 12
				else //cas classique
				{
					// Si service trop tot, ne peut pas etre pris
					if (MomentDepart <= vHoraireDepartDernier[NumProchain])
					{
						// Si service apres moment depart alors modification, moment d�part = premier depart du service
						if (MomentDepart < vHoraireDepartPremier[NumProchain])
							MomentDepart = vHoraireDepartPremier[NumProchain];

						// Possibilit� de sortie si le d�part trouv� est sup�rieur au dernier d�part permis
						if (MomentDepart > MomentDepartMax)
							return INCONNU;

						// Si le service circule retour du service
						if (vLigne->getTrain(NumProchain)->Circule(MomentDepart.getDate(), vHoraireDepartPremier[NumProchain].JPlus()))
						{	
							// on a presque valid� le service, il faut v�rifier la possiblite eventuelle de r�servation
							// si il est trop tard pour r�server, on abandonne ce service
							if (vLigne->getTrain(NumProchain)->ReservationOK(MomentDepart, __MomentCalcul))
							{
								return NumProchain;
							}
						}
					}
				}

			NumProchain++;
			
			}//end while
		
		}//end if

		MomentDepart++;
		MomentDepart.setHeure(0,0);
// MODIF HR FIN JOUR MANQUANT		NumProchain = vIndexDepart[MomentDepart.Heures()];
		NumProchain = vIndexDepart[0];
	}

	return INCONNU;
}


/*!	\brief Fournit le prochain d�part de la ligne � l'arr�t (m�thode 2)
	\param MomentDepart Heure de pr�sence sur le lieu de d�part
	\param MomentDepartMax Heure de d�part � ne pas d�passer
	\param AmplitudeServiceContinu 
	\param NumProchainMin Index o� commencer la recherche de prochain service
	\param __MomentCalcul Moment du calcul pour validation d�lai de r�servation
	\return Indice du service utilis� au sein de la ligne. -1 = Aucun service trouv�. 
	\retval MomentDepart Moment pr�cis du d�part. Inutilisable si -1 retourn�.
	\retval AmplitudeServiceContinu Amplitude du service continu 0 = service � horaires
	\version 2.0
	\author Hugues Romain
	\date 2003
*/
tNumeroService cGareLigne::Prochain(
	cMoment&			MomentDepart
	, const cMoment&	MomentDepartMax
	, cDureeEnMinutes&	AmplitudeServiceContinu
	, tNumeroService	NumProchainMin
	, const cMoment&	__MomentCalcul
) const {
	
	tNumeroService NumProchain = Prochain(MomentDepart, MomentDepartMax, __MomentCalcul, NumProchainMin);
	
	// TEST 5
	if (NumProchain != INCONNU && vLigne->EstCadence(NumProchain))
	{
		if (MomentDepart > vHoraireDepartDernier[NumProchain])
			AmplitudeServiceContinu = 1440 - (MomentDepart.getHeure() - vHoraireDepartDernier[NumProchain].getHeure()).Valeur();
		else
			AmplitudeServiceContinu = vHoraireDepartDernier[NumProchain].getHeure() - MomentDepart.getHeure();
	} // FIN TEST 5
	else
		AmplitudeServiceContinu = 0;
	return NumProchain;
}


/*! \brief Recherche du d�part pr�c�dent pouvant �tre emprunt� sur la gare ligne
	\version 2.0
	\author Hugues Romain
	\date 2003
	\param MomentArriveeMin Heure d'arrivee � ne pas d�passer.
	\param MomentArrivee    Heure de pr�sence sur le lieu d'arrivee.
	\return Indice du service utilis� au sein de la ligne (INCONNU (-1) = Aucun service trouv�)
	\retval MomentArrivee Moment pr�cis de l'arriv�e. Inutilisable si INCONNU retourn�
	\warning La v�rification de l'utilisabilit� du service vis � vis d'un �ventuel d�lai de r�servation doit �tre ffectu�e lors de la d�termination de l'�arr�t de d�part, dont l'heure de passage fait foi pour le calcul du d�lai
*/
tNumeroService cGareLigne::Precedent(cMoment &MomentArrivee, const cMoment& MomentArriveeMin) const
{
	tNumeroService NumPrecedent;
	
	// Recherche de l'horaire
	NumPrecedent = vIndexArrivee[MomentArrivee.Heures()];

	while (MomentArrivee >= MomentArriveeMin) // boucle sur les dates
	{
		// Exploration des horaires si la ligne circule
		if (vLigne->PeutCirculer(MomentArrivee.getDate()))
			while (NumPrecedent >= 0) // boucle sur les services
			{
				// Cas JPlus != en service continu
				if (vLigne->EstCadence(NumPrecedent) && vHoraireArriveePremier[NumPrecedent].JPlus() != vHoraireArriveeDernier[NumPrecedent].JPlus())
				{
					// Si service apres momentdepart alors modification
					if (MomentArrivee > vHoraireArriveeDernier[NumPrecedent] && MomentArrivee < vHoraireArriveePremier[NumPrecedent])
						MomentArrivee = vHoraireArriveeDernier[NumPrecedent];

					// Possibilit� de sortie
					if (MomentArrivee < MomentArriveeMin)
						return INCONNU;

					// Si le service circule retour du service
					if (MomentArrivee > vHoraireDepartPremier[NumPrecedent])
					{
						if (vLigne->getTrain(NumPrecedent)->Circule(MomentArrivee.getDate(), vHoraireArriveePremier[NumPrecedent].JPlus()))
							return NumPrecedent;
					}
					else
						if (vLigne->getTrain(NumPrecedent)->Circule(MomentArrivee.getDate(), vHoraireArriveeDernier[NumPrecedent].JPlus()))
							return NumPrecedent;
				}
				else
				{
					// Si service trop tot, ne peut pas etre pris
					if (MomentArrivee >= vHoraireArriveePremier[NumPrecedent])
					{
						// Si service apres momentdepart alors modification
						if (MomentArrivee > vHoraireArriveeDernier[NumPrecedent])
							MomentArrivee = vHoraireArriveeDernier[NumPrecedent];

						// Possibilit� de sortie
						if (MomentArrivee < MomentArriveeMin)
							return INCONNU;

						// Si le service circule retour du service
						if (vLigne->getTrain(NumPrecedent)->Circule(MomentArrivee.getDate(), vHoraireArriveeDernier[NumPrecedent].JPlus()))
							return NumPrecedent;
					}
				}
				NumPrecedent--;
			}

		MomentArrivee--;
		MomentArrivee.setHeure(23,59);
// MODIF HR FIN JOURNEE NON PRESENT		NumPrecedent = vLigne->NombreServices() - 1;
		NumPrecedent = vIndexArrivee[23];
	}

	return INCONNU;
}



tNumeroService cGareLigne::Precedent(
	cMoment &			MomentArrivee
	, const cMoment&	MomentArriveeMin
	, cDureeEnMinutes&	AmplitudeServiceContinu
) const {
	
	tNumeroService NumPrecedent = Precedent(MomentArrivee, MomentArriveeMin); 
	
	if (NumPrecedent != INCONNU && vLigne->EstCadence(NumPrecedent))
	{
		if (MomentArrivee > vHoraireArriveeDernier[NumPrecedent])
			AmplitudeServiceContinu = 1440 - (MomentArrivee.getHeure() - vHoraireArriveeDernier[NumPrecedent].getHeure()).Valeur();
// HR PBFREQ ESSAI1			AmplitudeServiceContinu = 1440 - (MomentArrivee.getHeure() - vHoraireArriveeDernier[NumPrecedent].getHeure()) + Attente(NumPrecedent);
		else
			AmplitudeServiceContinu = vHoraireArriveeDernier[NumPrecedent].getHeure() - MomentArrivee.getHeure();
// HR PBFREQ ESSAI1			AmplitudeServiceContinu = (vHoraireArriveeDernier[NumPrecedent].getHeure() - MomentArrivee.getHeure()) + Attente(NumPrecedent);
	}
	else
		AmplitudeServiceContinu = 0;
	return NumPrecedent;
}


// Validit� solution
/*
void cGareLigne::ValiditeSolution(tNumeroService NumeroService, cMoment& Op)
{
	ArretDepart[NumeroService]->ValiditeSolution(Op);
}
*/
// HoraireDA

/*
bool cGareLigne::HeureArriveeAD(tNumeroService NumeroService, cMoment& MomentArrivee, cHoraire& HoraireDepart)
{
	return(vArretArrivee[NumeroService].HeureArriveeAD(MomentArrivee, HoraireDepart));
}

void cGareLigne::HeureArriveeDA(tNumeroService NumeroService, cMoment& MomentArrivee, cMoment& MomentDepart, cHoraire& HoraireDepart)
{
	vArretArrivee[NumeroService].HeureArriveeDA(MomentArrivee, MomentDepart, HoraireDepart);
}

void cGareLigne::HeureDepartAD(tNumeroService NumeroService, cMoment& MomentDepart, cMoment& MomentArrivee, cHoraire& HoraireArrivee)
{
	vArretDepart[NumeroService].HeureDepartAD(MomentDepart, MomentArrivee, HoraireArrivee);
}

bool cGareLigne::HeureDepartDA(tNumeroService NumeroService, cMoment& MomentDepart, cHoraire& HoraireDepart)
{
	return(vArretDepart[NumeroService].HeureDepartDA(MomentDepart, HoraireDepart));
}
*/

// Circule
/*
cGareLigne::Circule(tNumeroService NumeroService, tTypeGareLigneDA TypeGareLigne, cDate& DateTest)
{
	if (TypeGareLigne == Depart)
		return(ArretDepart[NumeroService]->Circule(DateTest));
	else
		return(ArretArrivee[NumeroService]->Circule(DateTest));
}
*/

// NUL
/*bool cGareLigne::EstDepart()
{
	return(vTypeDA == Depart || vTypeDA == Passage);
}

bool cGareLigne::EstArrivee()
{
	return(vTypeDA == Arrivee || vTypeDA == Passage);
}
*/



cDureeEnMinutes cGareLigne::MeilleurTempsParcours(const cGareLigne& autreGL) const
{
	cDureeEnMinutes curT;
	cDureeEnMinutes bestT;
	
	for (tNumeroService iNumeroService=0; iNumeroService!= vLigne->NombreServices(); iNumeroService++)
	{
		curT = autreGL.vHoraireArriveePremier[iNumeroService] - vHoraireDepartPremier[iNumeroService];
		if (curT.Valeur() < 1)
			curT = 1;
		if (bestT.Valeur() == 0 || curT < bestT)
			bestT = curT;
	}
	return(bestT);
}






/*!	\brief Estimation de  la coh�rence d'une succession entre deux GAreLigne en fonction des PH et des coordonn�es des points d'arret
	\param AutreGareLigne GareLigne � comparer
	\return true si les donn�es semblent coh�rentes, false sinon
	\author Hugues Romain
	\version 1
	\date 2001
*/
bool cGareLigne::CoherenceGeographique(const cGareLigne& AutreGareLigne) const
{
	tDistanceM DeltaPM;
	if (PM() > AutreGareLigne.PM())
		DeltaPM = (vPM - AutreGareLigne.vPM) / 1000;
	else
		DeltaPM = (AutreGareLigne.vPM - vPM) / 1000;
	tDistanceKM DeltaGPS = cDistanceCarree(ArretLogique()->getPoint(), AutreGareLigne.ArretLogique()->getPoint()).Distance();
	
	if (DeltaPM > 10 * DeltaGPS && DeltaPM - DeltaGPS > 1)
	{
//		cTexte MessageErreur(TAILLETAMPON);
//		MessageErreur << "Ligne: " << vLigne->getCode() << " PH: " << vPH << " PA1: " << ArretLogique()->Index() << " PA2: " << AutreGareLigne.ArretLogique()->Index() << " DeltaPH: " << DeltaPH << " DeltaGPS: " << DeltaGPS;
//		Erreur(FichierLOG, "Erreur de coh�rence g�ographique (trajet trop long)", MessageErreur, "03006");
		return false;
	}
	if  (DeltaPM < DeltaGPS && DeltaGPS - DeltaPM > 1)
	{
//		cTexte MessageErreur(TAILLETAMPON);
//		MessageErreur << "Ligne: " << vLigne->getCode() << " PH: " << vPH << " PA1: " << ArretLogique()->Index() << " PA2: " << AutreGareLigne.ArretLogique()->index() << " DeltaPH: " << DeltaPH << " DeltaGPS: " << DeltaGPS;
//		Erreur(FichierLOG, "Erreur de coh�rence g�ographique (trajet trop court)", MessageErreur, "03007");
		return false;
	}

	// Succ�s
	return true;
}


/*
tTypeRefusLigne cGareLigne::UtilePourMeilleurDepart(cElementTrajet* ET, const cMoment& MomentMin) const
{
	if (!Circule(MomentMin, ET->MomentDepart()))
		return(RefusDate);

	if (!vLigne->Axe()->ETValide(ET))
		return(RefusAxe);

	return(Accepte);
}
*/


bool cGareLigne::Circule(const cMoment& MomentDebut, const cMoment& MomentFin) const
{
	cDate DateDebut;
	for (DateDebut = MomentDebut; DateDebut <= MomentFin; DateDebut++)
		if (vLigne->PeutCirculer(DateDebut))
			return true;
	return false;
}


void cGareLigne::RealloueHoraires(tNumeroService newService)
{
	// SET PORTAGE LINUX
	//_ASSERTE(vLigne->NombreServices() > 0);
	//END PORTAGE LINUX
	
	if (vLigne->NombreServices() == 1)
		AlloueHoraires();
	else
	{
		tNumeroService iNumeroService;
		cHoraire* newHoraire;
		
		newHoraire = new cHoraire[vLigne->NombreServices()];
		for (iNumeroService = 0; iNumeroService < vLigne->NombreServices() - 1; iNumeroService++)
			if (iNumeroService >= newService)
				newHoraire[iNumeroService + 1] = vHoraireArriveeDernier[iNumeroService];
			else
				newHoraire[iNumeroService] = vHoraireArriveeDernier[iNumeroService];
		delete[] vHoraireArriveeDernier;
		vHoraireArriveeDernier = newHoraire;
		
		newHoraire = new cHoraire[vLigne->NombreServices()];
		for (iNumeroService = 0; iNumeroService < vLigne->NombreServices() - 1; iNumeroService++)
			if (iNumeroService >= newService)
				newHoraire[iNumeroService + 1] = vHoraireArriveePremier[iNumeroService];
			else
				newHoraire[iNumeroService] = vHoraireArriveePremier[iNumeroService];
		delete[] vHoraireArriveePremier;
		vHoraireArriveePremier = newHoraire;
		
		newHoraire = new cHoraire[vLigne->NombreServices()];
		for (iNumeroService = 0; iNumeroService < vLigne->NombreServices() - 1; iNumeroService++)
			if (iNumeroService >= newService)
				newHoraire[iNumeroService + 1] = vHoraireArriveePremierReel[iNumeroService];
			else
				newHoraire[iNumeroService] = vHoraireArriveePremierReel[iNumeroService];
		delete[] vHoraireArriveePremierReel;
		vHoraireArriveePremierReel = newHoraire;

		newHoraire = new cHoraire[vLigne->NombreServices()];
		for (iNumeroService = 0; iNumeroService < vLigne->NombreServices() - 1; iNumeroService++)
			if (iNumeroService >= newService)
				newHoraire[iNumeroService + 1] = vHoraireDepartDernier[iNumeroService];
			else
				newHoraire[iNumeroService] = vHoraireDepartDernier[iNumeroService];
		delete[] vHoraireDepartDernier;
		vHoraireDepartDernier = newHoraire;

		newHoraire = new cHoraire[vLigne->NombreServices()];
		for (iNumeroService = 0; iNumeroService < vLigne->NombreServices() - 1; iNumeroService++)
			if (iNumeroService >= newService)
				newHoraire[iNumeroService + 1] = vHoraireDepartPremier[iNumeroService];
			else
				newHoraire[iNumeroService] = vHoraireDepartPremier[iNumeroService];
		delete[] vHoraireDepartPremier;
		vHoraireDepartPremier = newHoraire;

		newHoraire = new cHoraire[vLigne->NombreServices()];
		for (iNumeroService = 0; iNumeroService < vLigne->NombreServices() - 1; iNumeroService++)
			if (iNumeroService >= newService)
				newHoraire[iNumeroService + 1] = vHoraireDepartPremierReel[iNumeroService];
			else
				newHoraire[iNumeroService] = vHoraireDepartPremierReel[iNumeroService];
		delete[] vHoraireDepartPremierReel;
		vHoraireDepartPremierReel = newHoraire;
	}
}


void cGareLigne::CalculeArrivee(const cGareLigne &GareLigneDepart, tNumeroService iNumeroService, const cMoment &MomentDepart, cMoment &MomentArrivee) const
{
	// TEST 5
	if (vLigne->EstCadence(iNumeroService))
	{
		MomentArrivee = MomentDepart;
// HR ESSAI PBFREQ2		MomentArrivee += vLigne->Attente(iNumeroService);
		MomentArrivee += (vHoraireArriveePremier[iNumeroService] - GareLigneDepart.vHoraireDepartPremier[iNumeroService]);
	} // FIN TEST 5
	else
	{
		MomentArrivee = vHoraireArriveePremier[iNumeroService];
		MomentArrivee.addDureeEnJours(vHoraireArriveePremier[iNumeroService].JPlus() - GareLigneDepart.vHoraireDepartPremier[iNumeroService].JPlus());
	}
}

void cGareLigne::CalculeDepart(const cGareLigne &GareLigneArrivee, tNumeroService iNumeroService, const cMoment &MomentArrivee, cMoment &MomentDepart) const
{
	// TEST 5
	if (vLigne->EstCadence(iNumeroService))
	{
		MomentDepart = MomentArrivee;
// HR ESSAI PBFREQ2		MomentDepart -= vLigne->Attente(iNumeroService);
		MomentDepart -= (GareLigneArrivee.vHoraireArriveePremier[iNumeroService] - vHoraireDepartPremier[iNumeroService]);
	} // TEST 5
	else
	{
		MomentDepart = vHoraireDepartPremier[iNumeroService];
		MomentDepart.subDureeEnJours(GareLigneArrivee.vHoraireArriveePremier[iNumeroService].JPlus() - vHoraireDepartPremier[iNumeroService].JPlus());
	}
}


/*cMoment cGareLigne::CalculeArrivee(const cDescriptionPassage* curDP) const
{
	cMoment tempMoment;
	CalculeArrivee(*curDP->getGareLigne(), curDP->NumArret(), curDP->getMomentPrevu(), tempMoment);
	return(tempMoment);
}*/

/*! 	\brief Premi�re GareLigne de la ligne
	\author Hugues Romain
	\date 2001
*/
cGareLigne* cGareLigne::Origine() const
{
	return(vLigne->PremiereGareLigne());
}


/*! \brief Derniere GareLigne de la ligne.
	\author Hugues Romain
	\date 2001
*/
cGareLigne* cGareLigne::Destination() const
{
	return(vLigne->DerniereGareLigne());
}

void cGareLigne::setPADepartSuivant()
{
//	if (!vLigne->EstUneRoute())
//	{
		vPADepartSuivant = _physicalStop->PremiereGareLigneDep();
		_physicalStop->setPremiereGareLigneDep(this);
//	}
}

void cGareLigne::setPAArriveeSuivant()
{
//	if (!vLigne->EstUneRoute())
//	{
	vPAArriveeSuivante = _physicalStop->PremiereGareLigneArr();
		_physicalStop->setPremiereGareLigneArr(this);
//	}
}

void cGareLigne::AlloueHoraires()
{
	//_ASSERTE(vLigne->NombreServices() > 0);
	
	vHoraireArriveeDernier = new cHoraire[vLigne->NombreServices()];
	vHoraireArriveePremier = new cHoraire[vLigne->NombreServices()];
	vHoraireArriveePremierReel = new cHoraire[vLigne->NombreServices()];
	vHoraireDepartDernier = new cHoraire[vLigne->NombreServices()];
	vHoraireDepartPremier = new cHoraire[vLigne->NombreServices()];
	vHoraireDepartPremierReel = new cHoraire[vLigne->NombreServices()];
}

const cDureeEnMinutes& cGareLigne::Attente(tNumeroService iNumeroService) const
{
	return(vLigne->Attente(iNumeroService));
}

const cTexte& cGareLigne::getNomArretPhysique() const
{
	return(vArretLogique->getArretPhysique(vVoie)->getNom());
}

void cGareLigne::setDepartPrecedent(cGareLigne *newVal)
{
	vDepartPrecedent = newVal;
}

const cArretPhysique* cGareLigne::getVoie() const
{
	return vArretLogique->getArretPhysique(vVoie);
}

void cGareLigne::setDepartCorrespondancePrecedent(cGareLigne* __GL)
{
	vDepartCorrespondancePrecedent = __GL;
}

void cGareLigne::setPrecedent(cGareLigne* __GL)
{
	_Precedent = __GL;
}

cGareLigne* cGareLigne::getPrecedent() const
{
	return _Precedent;
}