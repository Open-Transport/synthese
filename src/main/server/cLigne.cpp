/*! \file cLigne.cpp
	\brief Impl�mentation classe cLigne
*/

#include "cLigne.h"

/*! \brief Destructeur
	\author Hugues Romain
	\date 2002
*/
cLigne::~cLigne()
{
	delete vPremiereGareLigne;
	delete[] vTrain;
}




/*! \brief Constructeur avec copie d'une autre ligne
	\author Hugues Romain
	\date 2002
	\param LigneACopier Ligne servant de mod�le � la copie
	\param Environnement Environnement auquel appartient la ligne r�sultat
*/
/*#ifdef ClasseAdmin
cLigne::cLigne(cLigne* LigneACopier, cEnvironnement* Environnement, cImport* Import)
#else
cLigne::cLigne(cLigne* LigneACopier, cEnvironnement* Environnement)
#endif
{
	operator =(*LigneACopier);
	vAxe = LigneACopier->vAxe;
	vCode.Vide();
	vCode << LigneACopier->vCode;
	Environnement->NomLigneUnique(vCode);
	
	// Ligne � chainer
	vSuivant = Environnement->PremiereLigne();
	Environnement->setPremiereLigne(this);

	// Initialisation
	vCirculation.setAnnees(Environnement->PremiereAnnee(), Environnement->DerniereAnnee());
	vPremiereGareLigne = NULL;
	vTarif =NULL;
	vVelo=NULL;
	vResa=NULL;
	vHandicape=NULL;
	vNombreServices = 0;
	vAAfficherSurIndicateurs = true;

	// Construction de la ligne
	for (cGareLigne* curGareLigne=LigneACopier->PremiereGareLigne(); curGareLigne!=NULL; curGareLigne=curGareLigne->Suivant())
		addGareLigne(new cGareLigne(this, *curGareLigne));
	Ferme();

}*/


/*! \brief Constructeur de base avec nom de code
	\param newCode Code de la ligne r�sultat
	\param newAxe Axe auquel appartiendra la ligne
	\param Environnement Environnement auquel appartiendra la ligne
	\author Hugues Romain
	\date 2002
	*/
cLigne::cLigne(const cTexte& newCode, cAxe* newAxe)
{
	// Valeurs transmises
	vCode << newCode;
	vAxe = newAxe;
	
	// Valeurs par d�faut
	vReseau = NULL;
	vMateriel = NULL;
	vPremiereGareLigne = NULL;
	vDerniereGareLigne = NULL;
	vTarif =NULL;
	vVelo=NULL;
	vHandicape=NULL;
	vResa=NULL;
	vNombreServices=0;
	vAAfficherSurTableauDeparts = true;
	vAAfficherSurIndicateurs = true;
}



/*! \brief Sauvegarde dans le fichier d'origine les donn�es de la ligne
	\todo A rendre compatible avec les FormatFichier pour rendre l'evolutivite.
	\todo G�rer tous les champs
	\todo GERER LES NOUVELLES LIGNES A PARTIR DE L'AXE OU NON
	\author Hugues Romain
	\date 2001
	*/
bool cLigne::Sauvegarde() const
{
/*	ifstream FichierActuel;
	ofstream FichierFutur;
	cTexte NomFichierTemp(vAxe->getNomFichier().GetTaille() + 5);
	NomFichierTemp << vAxe->getNomFichier() << ".temp";
	cTexte Tampon(TAILLETAMPON, true);
	bool AxeTrouve = false;
	bool LigneTrouvee = false;
	bool FichierTermine = false;

	// Ouverture des fichiers	
	FichierActuel.open(vAxe->getNomFichier().Texte());
	//Sauvegarde = new cSauvegarde(Axe->NomFichier);
	
	//Sauvegarde->CopieJusqueA(Axe->Code, Code);

	// Si le fichier existe: Copie de la partie non utile
	if (FichierActuel.is_open())
	{
		FichierFutur.open(NomFichierTemp.Texte());
		while (!FichierActuel.eof() && !FichierTermine)
		{
			switch (Tampon.LireLigne(FichierActuel))
			{
			case TYPEVide:
				FichierTermine = true;
				break;

			case TYPESection:
				// Recherche de l'axe
				if (!AxeTrouve)
				{
					if (vAxe->Libre() && Tampon.Compare("##", 2))
						Tampon.vPosition = 2;
					else
						Tampon.vPosition = 1;
					if (Tampon.Compare(vAxe->getCode(), 0, Tampon.vPosition))
						AxeTrouve = true;
				}
				else
					FichierTermine = true;
				break;

			case TYPESousSection:
				// Recherche de la ligne
				if (AxeTrouve && Tampon.Compare(vCode, 0, 1))
				{
					FichierFutur << Tampon << endl;
					Tampon.Vide();
					for (Tampon.LireLigne(FichierActuel); Tampon[0]!= '[' && Tampon[0] != '#'; )
						if(Tampon.LireLigne(FichierActuel)==TYPEVide) 
							break;
					
					LigneTrouvee = true;
					FichierTermine = true;
					break;
				}

				// Controle de sortie
				if (AxeTrouve && !Tampon.Compare("#", 1))
					break;
			}
			if (FichierTermine)
				break;

			// Copie de la chaine
			FichierFutur << Tampon << endl;
		}
	}
	else
	{
		FichierFutur.open(vAxe->getNomFichier().Texte());
	}

	if (!AxeTrouve)
	{
		FichierFutur << endl << "#";
		if (vAxe->Libre())
			FichierFutur << "#";
		FichierFutur << vAxe->getCode() << "\n";
	}
	else
		FichierFutur << FINL;

	if (!LigneTrouvee)
		FichierFutur << "[" << vCode << "\n";

	// Ecriture des donn�es de la ligne	
	FichierFutur << "RES              000000\n"; // PROVISOIRE
	if (vLibelleSimple.GetTaille())
		FichierFutur << "CAR              " << vLibelleSimple << endl;
	if (vLibelleComplet.GetTaille())
		FichierFutur << "DES              " << vLibelleComplet << endl;
	if (vImage.GetTaille())
		FichierFutur << "IMG              " << vImage << endl;
	if (vStyle.GetTaille())
		FichierFutur << "STY              " << vStyle << endl;
	FichierFutur << "IND              " << vNomPourIndicateur << endl;
	FichierFutur << "MAT              " << TXT2(vMateriel->Code(),6) << "\n";
	FichierFutur << "RSA              000000\n"; // PROVISOIRE
	FichierFutur << "VEL              000000\n"; // PROVISOIRE
	FichierFutur << "TAR              000000\n"; // PROVISOIRE
#ifdef ClasseAdmin
	// ATTENTION La desactivation de cette partie dans la version non Admin
	// implique la non conservation des infos d'importation en cas de
	// sauvegarde � l'aide de cette fonction !
	// R�gler cette question avant utilisation de la fonction Sauvegarde
	// sur la base SEMVAT
	if (vImportSource != NULL)
		FichierFutur << "IMP              " << NombreFormate(vImportSource->Code(),6) << "\n";
#endif
	
	FichierFutur << "CIS              ";
	// SET PORTAGE LINUX
	tNumeroService iNumeroService=0;
	for (; iNumeroService!=vNombreServices; iNumeroService++)
		FichierFutur << TXT2(vTrain[iNumeroService].getJC()->Index(),6);
	
	//END PORTAGE LINUX
	FichierFutur << "\n";
	
	FichierFutur << "ATT              ";
	for (iNumeroService=0; iNumeroService!=vNombreServices; iNumeroService++)
		if (vTrain[iNumeroService].EstCadence())
			FichierFutur << TXT2(Attente(iNumeroService).Valeur(), 6);
		else
			FichierFutur << "      ";
	FichierFutur << "\n";

	FichierFutur << "FIN              ";
	for (iNumeroService=0; iNumeroService!= vNombreServices; iNumeroService++)
		if (vTrain[iNumeroService].EstCadence())
		{
			cTexteCodageInterne Parametre;
			Parametre << vPremiereGareLigne->getHoraireDepartDernier(iNumeroService);
			FichierFutur << Parametre;
		}
		else
			FichierFutur << "      ";
	FichierFutur << "\n";

#ifdef ClasseAdmin
	// ATTENTION La desactivation de cette partie dans la version non Admin
	// implique la non conservation des infos d'importation en cas de
	// sauvegarde � l'aide de cette fonction !
	// R�gler cette question avant utilisation de la fonction Sauvegarde
	// sur la base SEMVAT
	if (vImportSource != NULL)
	{
		FichierFutur << "COR              ";
		for (iNumeroService=0; iNumeroService!= vNombreServices; iNumeroService++)
			FichierFutur << NombreFormate(vTrain[iNumeroService].CodeBaseTrains(),6);
		FichierFutur << "\n";
	}
#endif
	
	FichierFutur << "NUM              ";
	for (iNumeroService=0; iNumeroService!= vNombreServices; iNumeroService++)
	{
		FichierFutur << vTrain[iNumeroService].getNumero();
		//SET PORTAGE LINUX
		//for (int i=0; i<(6 - vTrain[iNumeroService].getNumero().GetTaille()); i++)
		for (unsigned int i=0; i<(6 - vTrain[iNumeroService].getNumero().GetTaille()); i++)
		//END PORTAGE
			FichierFutur << " ";
	}
	FichierFutur << "\n";

	for (cGareLigne* curGareLigne= vPremiereGareLigne; curGareLigne!=NULL; curGareLigne=curGareLigne->Suivant())
	{
		if (curGareLigne->EstArrivee())
		{
			FichierFutur << TXT2(curGareLigne->PH(),6);
			FichierFutur << "*";
			FichierFutur << TXT2(curGareLigne->ArretLogique()->Index(),6);
			FichierFutur << "A";
			FichierFutur << TXT2(curGareLigne->ArretPhysique(), 2);
			if (curGareLigne->HorairesSaisis())
				for (iNumeroService=0; iNumeroService!= vNombreServices; iNumeroService++)
				{
					cTexteCodageInterne Parametre;
					Parametre << curGareLigne->getHoraireArriveePremier(iNumeroService);
					FichierFutur << " " << Parametre;
				}
			FichierFutur << endl;
		}

		if (curGareLigne->EstDepart())
		{
			FichierFutur << TXT2(curGareLigne->PH(),6);
			FichierFutur << "*";
			FichierFutur << TXT2(curGareLigne->ArretLogique()->Index(),6);
			FichierFutur << "D";
			FichierFutur << TXT2(curGareLigne->ArretPhysique(), 2);
			if (curGareLigne->HorairesSaisis())
				for (iNumeroService=0; iNumeroService!= vNombreServices; iNumeroService++)
				{
					cTexteCodageInterne Parametre;
					Parametre << curGareLigne->getHoraireDepartPremier(iNumeroService);
					FichierFutur << " " << Parametre;
				}
			FichierFutur << endl;
		}
	}

	// Si le fichier existe: Copie de la partie non utile
	if (FichierActuel.is_open())
	{
		if (!FichierActuel.eof())
			FichierFutur << endl << Tampon << endl;
		while (!FichierActuel.eof())
		{
			if (Tampon.LireLigne(FichierActuel) == TYPEVide)
				break;
			FichierFutur << Tampon << endl;
		}
		FichierActuel.close();
		FichierFutur.close();
		// SET PORTAGE LINUX
	*/	/*if (CopyFile(NomFichierTemp.Texte(), vAxe->getNomFichier().Texte(), false))
			DeleteFile(NomFichierTemp.Texte());
		else
			return(false);
		*/
/*		//END PORTAGE LINUX
	}
	else
	{
		FichierFutur.close();
	}

*/	return(true);
}



// cLigne - Identifie
// ____________________________________________________________________________
// 
// 
// ____________________________________________________________________________ 
bool cLigne::Identifie(const LogicalPlace** tbGares, const cMateriel* testMateriel) const
{
	// D�faut de mat�riel si sp�cifi�
	if (vMateriel != NULL && testMateriel!=NULL && vMateriel != testMateriel)
		return(false);

	cGareLigne* curGareLigne = vPremiereGareLigne;
	for (size_t i=0; tbGares[i]!=NULL; i++)
	{
		// Arret commun ou avec horaire non commun suivant
		for (; curGareLigne!=NULL && !curGareLigne->HorairesSaisis(); curGareLigne=curGareLigne->Suivant())
		{ }

		// Est on arriv� en fin de ligne pr�matur�ment ?
		if (curGareLigne==NULL)
			return(false);

		// Arret non commun
#ifdef ClasseAdmin
		if (!curGareLigne->ArretLogique()->CompareCodeRIHO(*tbGares[i]))
#else
		if (curGareLigne->ArretLogique() != tbGares[i])
#endif
			return(false);

		curGareLigne = curGareLigne->Suivant();
	}
	
	// A t on parcouru tous les points d'arret ?
	if (curGareLigne!=NULL)
		return(false);
	else
		return(true);
}






bool cLigne::allowAddServiceHoraire(const cHoraire* tbHoraires) const
{
	bool ControleDepassementPositif = true;
	bool ControleDepassementNegatif = true;
	
	// D�termination du numero de service futur
	// Insertion du service: d�termination du rang
	
	// SET PORTAGE LINUX
	tNumeroService iNumeroService=0;
	for (; iNumeroService!= vNombreServices; iNumeroService++)
		if (tbHoraires[1] < vPremiereGareLigne->getHoraireDepartPremier(iNumeroService))
			break;
	//END PORTAGE LINUX
	if (iNumeroService== vNombreServices)
		ControleDepassementPositif = false;
	if (iNumeroService==0)
		ControleDepassementNegatif = false;

	size_t i=0;
	for (cGareLigne* curGareLigne= vPremiereGareLigne; curGareLigne!=NULL; curGareLigne=curGareLigne->Suivant())
		if (curGareLigne->HorairesSaisis())
		{
			// Depassement negatif
			if (ControleDepassementNegatif)
			{
				if (curGareLigne->EstArrivee() && curGareLigne->getHoraireArriveeDernier(iNumeroService-1) >= tbHoraires[i])
					return(false);
				if (curGareLigne->EstDepart() && curGareLigne->getHoraireDepartDernier(iNumeroService-1) >= tbHoraires[i+1])
					return(false);
			}
			if (ControleDepassementPositif)
			{
				if (curGareLigne->EstArrivee() && curGareLigne->getHoraireArriveePremier(iNumeroService) <= tbHoraires[i])
					return(false);
				if (curGareLigne->EstDepart() && curGareLigne->getHoraireDepartPremier(iNumeroService) <= tbHoraires[i+1])
					return(false);
			}
			i += 2;
		}
	return(true);
}


/*! \brief Ajout d'un point d'arr�t en fin de ligne
	\param GLigneACopier Objet � copier (NULL = cr�ation d'un nouvel objet vide)
	\param newPM Point m�trique du nouvel arr�t
	\param newArretPhysique Num�ro de l'arr�t physique au sein de l'arr�t loguque
	\param newType Type de desserte � l'arr�t (d�part et/ou arriv�e)
	\param newPA Arr�t logique desservi
	\param newHorairesSaisis Indique si les horaires sont fournis dans le fichier source
	\param Route ???
	\warning Cette fonction ne peut �tre utilis�e que si la ligne ne poss�de aucune circulation
	\author Hugues Romain
	\date 2002

	En cas de succession d'arr�ts identiques, la relation entre les deux arr�ts n'est pas possible.
*/
cGareLigne* cLigne::addGareLigne(const cGareLigne* GLigneACopier, tDistanceM newPM, cArretPhysique* newArretPhysique, tTypeGareLigneDA newType, bool newHorairesSaisis, bool Route)
{
	cGareLigne* newGareLigne;
	if (GLigneACopier == NULL)
	{
		newGareLigne = new cGareLigne(this);
		newGareLigne->setPM(newPM);
		newGareLigne->setArretPhysique(newArretPhysique);
		newGareLigne->setArretLogique(newPA);
		newGareLigne->setTypeDA(newType);
		if (!Route)
		{
			if (newGareLigne->EstDepart())
				newGareLigne->setPADepartSuivant();
			if (newGareLigne->EstArrivee())
				newGareLigne->setPAArriveeSuivant();
		}
		newGareLigne->setHorairesSaisis(newHorairesSaisis);
	}
	else
		newGareLigne = new cGareLigne(this, *GLigneACopier);


	if (vPremiereGareLigne == NULL)
	{
		vPremiereGareLigne = newGareLigne;
		newGareLigne->setTypeDA(Depart);
	}
	else
	{
		// Chainages d�part/arriv�e
		for (cGareLigne* __GL = vDerniereGareLigne;
			__GL && (
				__GL->getArriveeSuivante() == NULL 
				|| __GL->getArriveeCorrespondanceSuivante() == NULL 
				|| newGareLigne->getDepartPrecedent() == NULL 
				|| newGareLigne->getDepartCorrespondancePrecedent() == NULL
			);
			__GL=__GL->getPrecedent())
		{
			// On chaine uniquement les relations entre A et A, D et D, A et D si arr�ts diff�rents, D et A si arr�ts diff�rents
			if (__GL->ArretLogique() != newGareLigne->ArretLogique() || __GL->TypeDA() == newGareLigne->TypeDA())
			{
				// Chainage arriv�es suivantes
				if (__GL->getArriveeSuivante() == NULL && newGareLigne->EstArrivee())
					__GL->setArriveeSuivante(newGareLigne);
				if (__GL->getArriveeCorrespondanceSuivante() == NULL && newGareLigne->ArretLogique()->CorrespondanceAutorisee())
					__GL->setArriveeCorrespondanceSuivante(newGareLigne);

				if (__GL->EstDepart() && newGareLigne->getDepartPrecedent() == NULL)
					newGareLigne->setDepartPrecedent(__GL);
				if (__GL->EstDepart() && newGareLigne->getDepartCorrespondancePrecedent() == NULL && __GL->ArretLogique()->CorrespondanceAutorisee())
					newGareLigne->setDepartCorrespondancePrecedent(__GL);
			}
		}
	}

	// Chainages pr�c�dent/suivant/dernier
	newGareLigne->setPrecedent(vDerniereGareLigne);
	if (vDerniereGareLigne != NULL)
		vDerniereGareLigne->setSuivant(newGareLigne);
	vDerniereGareLigne = newGareLigne;
	
	return newGareLigne;
}


/*! \brief Cloture d'une ligne
	\author Hugues Romain
	\date 2002
*/
void cLigne::Ferme()
{
	// Suppression du statut arret de d�part de pas d'arriv�e � desservir et vv
	for (cGareLigne* __GL = this->vPremiereGareLigne; __GL; __GL = __GL->Suivant())
	{
		if (__GL->getArriveeSuivante() == NULL)
			__GL->setTypeDA(Arrivee);
		if (__GL->getDepartPrecedent() == NULL)
			__GL->setTypeDA(Depart);
	}
}


/*! \brief Met � jour le calendrier g�n�ral de la ligne
	\author Hugues Romain
	\date 2002

	Le calendrier g�n�r� indique chaque jour o� au moins une circulation de la ligne fonctionne.
	Il tient compte des circulations passant minuit : si au moins une minute d'une journ�e est concern�e par l'une des circulations, alors la date est s�lectionn�e.
	De ce fait, si la demande de calcul se fait sur un jour non s�lectionn� dans le calendrier g�n�r�, alors la ligne peut �tre filtr�e.
*/
void cLigne::MajCirculation()
{
	vCirculation.RAZMasque();
	cGareLigne* lastGL = vPremiereGareLigne->Destination();
	for (tNumeroService iNumeroService=0; iNumeroService!= vNombreServices; iNumeroService++)
	{
		if (lastGL->getHoraireArriveeDernier(iNumeroService).JPlus() != vPremiereGareLigne->getHoraireDepartPremier(iNumeroService).JPlus())
		{
			vCirculation.RAZMasque(true);
			break;
		}
		vTrain[iNumeroService].getJC()->SetInclusionToMasque(vCirculation);
	}
}


/*! \brief Copie d'une ligne
	\author Hugues Romain
	\date 2001
	\warning Cette fonction ne copie pas les circulations, horaires, arr�ts desservis...
	\return La ligne elle-m�me
	*/
cLigne* cLigne::operator =(const cLigne& LigneACopier)
{
	// Copie des parametres
	vLibelleSimple.Vide();
	vLibelleSimple << LigneACopier.vLibelleSimple;
	vLibelleComplet.Vide();
	vLibelleComplet << LigneACopier.vLibelleComplet;
	vStyle.Vide();
	vStyle << LigneACopier.vStyle;
	vImage.Vide();
	vImage << LigneACopier.vImage;
	vNomPourIndicateur.Vide();
	vNomPourIndicateur << LigneACopier.vNomPourIndicateur;
	vResa = LigneACopier.vResa;
	vVelo = LigneACopier.vVelo;
	vMateriel = LigneACopier.vMateriel;
	vAAfficherSurTableauDeparts = LigneACopier.vAAfficherSurTableauDeparts;

	return(this);	
}



/*!	\brief Accesseur/Modificatieur Nombre de services dans la ligne
	\param newNombreArrets Nombre de services � allouer si non encore fait
	\return Le nombre de services appartenant � la ligne
	\author Hugues Romain
	\date 2001-2005
	
Si le nombre de services n'est pas configur� (valeur 0), alors newNombreArrets services sont cr��s :
 - la variable priv�e contenant le nombre de services est mise � jour
 - les services proprements dits sont allou�s dans le tableau vTrain
 - les cha�nages vers la ligne sur les services sont effectu�s

Si le nombre de services est d�j� configur�, aucune op�ration n'est effectu�e
*/
tNumeroService cLigne::NombreServices(int newNombreServices)
{
	// Services d�j� configur�s ?
	if (!vNombreServices)
	{
		// Mise � jour objet ligne
		vNombreServices = (tNumeroService) newNombreServices;
		
		// Allocation des services
		vTrain = new cTrain[vNombreServices];
		
		// Cha�nage
		for (tNumeroService i=0; i<vNombreServices; i++)
			vTrain[i].setLigne(this);
	}
	return(vNombreServices);	
}




const tDureeEnMinutes& cLigne::Attente(tNumeroService iNumeroService) const
{
	// SET PORTAGE LINUX
	//_ASSERTE(iNumeroService >= 0 && iNumeroService < vNombreServices);
	//END PORTAGE LINUX
	return(vTrain[iNumeroService].Attente());
}

void cLigne::setJC(tNumeroService i, cJC *newVal)
{
	// SET PORTAGE LINUX
	//_ASSERTE(i >= 0 && i < vNombreServices);
	//END PORTAGE LINUX
	vTrain[i].setJC(newVal);
}

void cLigne::setAttente(tNumeroService iService, const tDureeEnMinutes& newVal)
{
	// SET PORTAGE LINUX
	//_ASSERTE(iService >= 0 && iService < vNombreServices);
	//END PORTAGE LINUX
	vTrain[iService].setAttente(newVal);
}

void cLigne::setServiceContinu(tNumeroService iService)
{
	
	//_ASSERTE(iService >= 0 && iService < vNombreServices);
	
	vTrain[iService].setServiceContinu();
}

bool cLigne::EstCadence(tNumeroService iNumeroService) const
{
	
	//_ASSERTE(iNumeroService >= 0 && iNumeroService < vNombreServices);
	
	return(vTrain[iNumeroService].EstCadence());
}

void cLigne::setAmplitudeServiceContinu(tNumeroService iNumeroService, const tDureeEnMinutes& newVal)
{
	
	//_ASSERTE(iNumeroService >= 0 && iNumeroService < vNombreServices);
	
	vTrain[iNumeroService].setAmplitudeServiceContinu(newVal);
}

const tDureeEnMinutes& cLigne::EtalementCadence(tNumeroService iNumeroService) const
{
	
	//_ASSERTE(iNumeroService >= 0 && iNumeroService < vNombreServices);
	
	return(vTrain[iNumeroService].EtalementCadence());
}


/*!	\brief Accesseur service appartenant � la ligne
	\param iService Index du service au sein de la ligne
	\return Pointeur vers le service demand� s'il existe, NULL sinon
	\author Hugues Romain
	\date 2001-2005
*/
const cTrain* cLigne::GetTrain(tNumeroService iService) const
{
	if (iService >= 0 && iService < vNombreServices)
		return getTrain(iService);
	else
		return NULL;
}



/*!	\brief Modificateur des ann�es couvertes par un calendrier de circulation
	\param PremiereAnnee la premi�re ann�e couverte par le calendrier
	\param DerniereAnnee la derni�re ann�e couverte par le calendrier
	\author Hugues Romain
	\date 2001-2004
*/
void	cLigne::setAnneesCirculation(tAnnee PremiereAnnee, tAnnee DerniereAnnee)
{
	vCirculation.setAnnees(PremiereAnnee, DerniereAnnee);
}



/*!	\brief Modifie le pointeur vers l'horaire de d�part de l'origine du service s�lectionn�
	\param NumeroService Index du service au sein de la ligne
	\param Horaire	Pointeur vers l'horaire de d�part du service de son origine
	\author Hugues Romain
	\date 2005
*/
void cLigne::setHoraireDepart(tNumeroService NumeroService, cHoraire* Horaire)
{
	vTrain[NumeroService].setHoraireDepart(Horaire);
}

bool cLigne::setNumeroService(tNumeroService NumeroService, const cTexte& Texte)
{
	vTrain[NumeroService].setNumero(Texte);
	return true;
}



/*!	\brief Indique si la ligne correspond � une jonction � pied
*/
bool cLigne::EstUneLigneAPied() const
{
	return Materiel()->EstAPied();
}




/*!	\brief Accesseur Girouette
*/
const cTexte& cLigne::getGirouette() const
{
	return vGirouette;
}



/*!	\brief Accesseur train
*/
const cTrain* cLigne::getTrain(tNumeroService __NumeroService) const
{
	return vTrain + __NumeroService;
}


/*!	\brief Calcul de la possibilit� qu'au moins un service circule le jour donn�, selon le calendrier g�n�ral de la ligne
	\param __Date Date � tester
	\author Hugues Romain
	\date 2005
*/
bool cLigne::PeutCirculer(const cDate& __Date) const
{
	return vCirculation.Circule(__Date);
}
