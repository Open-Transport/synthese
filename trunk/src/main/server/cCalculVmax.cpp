/*------------------------------------*
 |                                    |
 |  APDOS / APISAPI - SYNTHESE v0.7   |
 |  � Hugues Romain 2000/2003         |
 |  cCalculVmax.cpp                   |
 |  Sources Classe Calcul des Vmax    |
 |                                    |
 *------------------------------------*/

#ifdef SUPPRIME
 
#include <iostream>

cElementTrajet** cCalculVmax::ListeDestinations(cElementTrajet* TrajetEffectue, bool EnCorrespondance, ofstream& F)
{

	// D�clarations
	cElementTrajet** TableauRetour;
	cArretLogique*			 GareOrigine = TrajetEffectue->Dernier()->getGareArrivee();
	cGareLigne*		 CurrentGLD;
	cGareLigne*		 CurrentGLA;
	cElementTrajet*	 CurrentET;
	cElementTrajet*	 PrecedentET;
	int				 NombreET = 0;
	int				 i;
	int				 n;
	DureeEnMinutes	 DureeEnMinutesCorrespondance = 0;
	DureeEnMinutes	 D;
	DureeEnMinutes	 DTest;


	// Remplissage des ET
	PrecedentET = NULL;

	// Balayage des lignes
	for (CurrentGLD = GareOrigine->PremiereGareLigneDep(); CurrentGLD != NULL; CurrentGLD = CurrentGLD->PADepartSuivant())
	{

		// La ligne est-elle utilisable ?
		if (CurrentGLD->ControleAxe(TrajetEffectue, eSansVelo, eSansEquipementHandicape, eTouteLigne, -1))
		{
			if (EnCorrespondance)
				DureeEnMinutesCorrespondance = GareOrigine->AttenteMinimale();

			for (CurrentGLA = CurrentGLD->getArriveeSuivante(); CurrentGLA != NULL; CurrentGLA = CurrentGLA->getArriveeSuivante())
			{
				D = CurrentGLD->MeilleurTempsParcours(*CurrentGLA);
				DTest = TrajetEffectue->DureeEnMinutesRoulee() + DureeEnMinutesCorrespondance + D;

				// Utilit� de la ligne POINT CLE D'ELIMINATION
				if (vMeilleureDuree[CurrentGLA->ArretLogique()->Index()] !=0 && DTest > vMeilleureDuree[CurrentGLA->ArretLogique()->Index()] + CurrentGLA->ArretLogique()->AttenteMinimale())
					break;

				if (DTest < vMeilleureDuree[CurrentGLA->ArretLogique()->Index()] || vMeilleureDuree[CurrentGLA->ArretLogique()->Index()] == 0)
				{
					vMeilleureDuree[CurrentGLA->ArretLogique()->Index()] = DTest;
					if (CurrentGLA->ArretLogique()->CorrespondanceAutorisee())
					{ 
						if (vET[CurrentGLA->ArretLogique()->Index()] == NULL)
						{
							CurrentET = new cElementTrajet;					// Allocation
							CurrentET->setSuivant(PrecedentET);				// Chainage
							PrecedentET = CurrentET;						// M�morisation pour prochain chainage
							CurrentET->setArretLogiqueArrivee(CurrentGLA->ArretLogique());		// Ecriture gare d'arriv�e
							vET[CurrentGLA->ArretLogique()->Index()] = CurrentET;				// Stockage dans la gare pour reutilisation eventuelle
							NombreET++;										// Compteur pour allocation du tableau
						}
						else
						{	// On �crase un ET d�j� cr�� dans cette fonction
							CurrentET = vET[CurrentGLA->ArretLogique()->Index()];
						}
						CurrentET->setLigne(CurrentGLD->Ligne());			// Ecriture ligne
						CurrentET->setDureeEnMinutesRoulee(DureeEnMinutesCorrespondance + D);
						//for (int iii=0; iii!=TrajetEffectue->NombreElements; iii++)
						//	F << "|";
						//F << CurrentET->GareArrivee->NomComplet() << " - " << CurrentET->Ligne->Code << "\n";
					}
				}
			}
		}
	}
    
	// Cas aucune solution
	if (NombreET == 0)
		TableauRetour = NULL;
	else
	{
		// Allocation
		TableauRetour = (cElementTrajet**) malloc ((NombreET+1)*sizeof(cElementTrajet*));

		CurrentET = PrecedentET;
		for (i=0; CurrentET!=NULL; )
		{
			vET[CurrentET->getGareArrivee()->Index()] = NULL;
			TableauRetour[i] = CurrentET;
			CurrentET = CurrentET->Suivant();
			TableauRetour[i]->setSuivant(NULL);
			i++;
		}
		TableauRetour[i]=NULL;

		// Tri des ET
		for (i--; i>=0; i--)
		{
			for (n=0; n<i; n++)
			{
				if (TableauRetour[n]->DureeEnMinutesRoulee() > TableauRetour[n+1]->DureeEnMinutesRoulee())
				{
					CurrentET = TableauRetour[n];
					TableauRetour[n] = TableauRetour[n+1];
					TableauRetour[n+1] = CurrentET;
				}
			}
		}
	}
	return(TableauRetour);
}


void cCalculVmax::CalculRecursif(cElementTrajet* TrajetEffectue, bool EnCorrespondance, ofstream& F)
{
	cElementTrajet** ListeTrajetsDirects;
	cElementTrajet*	 PointeurET;
	cElementTrajet*	 NewET;
	cElementTrajet*  AncienDernier = TrajetEffectue->Dernier();
	DureeEnMinutes	 D;

	// Sortie si trop de correspondances
	if (TrajetEffectue->NombreElements() != NMAXPROFONDEUR)
	{
		ListeTrajetsDirects = ListeDestinations(TrajetEffectue, EnCorrespondance,F);

		if (ListeTrajetsDirects != NULL)
		{
			// Ajout d'un �l�ment � trajeteffectue
			NewET = new cElementTrajet;
			NewET->setDureeEnMinutesRoulee(0);
			D = TrajetEffectue->DureeEnMinutesRoulee();
			cElementTrajet* pointeur;

			// Pointeur Suivant
			TrajetEffectue->Dernier()->setSuivant(NewET);

			// Pointeurs Dernier et Nombre d'�l�ments
			for (pointeur = TrajetEffectue; pointeur != NewET; pointeur = pointeur->Suivant())
			{
				pointeur->setDernier(NewET);
				pointeur->setNombreElements(pointeur->NombreElements() + 1);
			}

			for (; *ListeTrajetsDirects!=NULL; ListeTrajetsDirects++)
			{
				// Pr�paration de TrajetEffectue
				*NewET = **ListeTrajetsDirects;
				delete *ListeTrajetsDirects;
				TrajetEffectue->setDureeEnMinutesRoulee(D + NewET->DureeEnMinutesRoulee());

				// Contr�le de non d�passement sur modification effectuee par la recursivit�
				if (TrajetEffectue->DureeEnMinutesRoulee() <= vMeilleureDuree[NewET->getGareArrivee()->Index()])
					CalculRecursif(TrajetEffectue, true,F);
			}

			// Nettoyage de TE et destruction du NewET
			for (PointeurET=TrajetEffectue; PointeurET!=NewET; PointeurET=PointeurET->Suivant())
			{
				PointeurET->setNombreElements(PointeurET->NombreElements() - 1);
				PointeurET->setDernier(AncienDernier);
				PointeurET->setDureeEnMinutesRoulee(D);
			}
			AncienDernier->setSuivant(NULL);
			delete NewET;
		}
	}
}



void cCalculVmax::Calcule()
{
	cArretLogique* curArretLogique;
	cDistanceCarree DCarre;	// Pour contenir la distance carr�e entre chaque point test et chaque point courant
	tDistanceKM D;			// Pour contenir la distance entre chaque point test et chaque point courant
	float T;
	tCategorieDistance CD;
	tVitesseKMH V;
	tNumeroArretLogique iNumeroArretLogique2;
	ofstream F;
	F.open(NOMFICHIERVMAX.Texte());
	
	//SET PORTAGE LINUX
	if (!F.is_open())
	{
		cout << "*** ERREUR Impossible d'ouvrir le fichier " << NOMFICHIERVMAX << "\n";
		return;	
	}
	//END PORTAGE LINUX
	
	cElementTrajet* ET = new cElementTrajet;

	// Balayage direct
	for (tNumeroArretLogique iNumeroArretLogique=1; iNumeroArretLogique != vEnvironnement->NombrePointsArret(); iNumeroArretLogique++)
	{
		curArretLogique = vEnvironnement->GetArretLogique(iNumeroArretLogique);
		if (curArretLogique != NULL)
		{
			for (CD=0; CD!=NOMBREVMAX; CD++)
				curArretLogique->setVMax(CD,0);

			// Vidage des DureeMin et det
			for (iNumeroArretLogique2=1; iNumeroArretLogique2 != vEnvironnement->NombrePointsArret(); iNumeroArretLogique2++)
			{
				vMeilleureDuree[iNumeroArretLogique2] = 0;
				vET[iNumeroArretLogique2] = NULL;
			}

			// Remplissage des DureeMin
			ET->setArretLogiqueArrivee(curArretLogique);
			ET->setDureeEnMinutesRoulee(0);
			ET->setLigne(NULL);
			CalculRecursif(ET, false,F);

			// Calcul des vitesses
			for (iNumeroArretLogique2 = 1; iNumeroArretLogique2 != vEnvironnement->NombrePointsArret(); iNumeroArretLogique2++)
				if (vEnvironnement->GetArretLogique(iNumeroArretLogique2) != NULL && iNumeroArretLogique2 != iNumeroArretLogique)
				{
					DCarre.setFromPoints(curArretLogique->getPoint(), vEnvironnement->GetArretLogique(iNumeroArretLogique2)->getPoint());
					D = DCarre.Distance();
					CD = CategorieDistance(D, false);
					T = vMeilleureDuree[iNumeroArretLogique2];
					V = (tVitesseKMH) (D/T*60);
					if (V > 200)
						F << cTexte("Attention: V>200 vers le point d'arret ") << TXT(iNumeroArretLogique2) << cTexte("\n");
					if (V > curArretLogique->vitesseMax(CD))
							curArretLogique->setVMax(CD, V);
				}

			F << cTexte("[") << TXT(iNumeroArretLogique) << cTexte("\n");
			F << cTexte("VM ");
			for (CD=0; CD!=NOMBREVMAX; CD++)
			{
				if (curArretLogique->vitesseMax(CD) < 100)
					F << cTexte("0");
				if (curArretLogique->vitesseMax(CD) < 10)
					F << cTexte("0");
				F << TXT(curArretLogique->vitesseMax(CD));
			}
			F << cTexte("\n");
		}
	}
	F.close();
}

cCalculVmax::cCalculVmax(cEnvironnement* newEnv)
{
	vEnvironnement = newEnv;
	vMeilleureDuree = (DureeEnMinutes*) malloc((vEnvironnement->NombrePointsArret() + 1) * sizeof(DureeEnMinutes));
	vET = (cElementTrajet**) malloc((vEnvironnement->NombrePointsArret() + 1) * sizeof(cElementTrajet*));
}

#endif
