/*! \file cLigne.cpp
	\brief Impl�mentation classe cLigne
*/

#include "cGareLigne.h"
#include "cArretPhysique.h"
#include "LogicalPlace.h"
#include "cTrain.h"
#include "cMateriel.h"
#include "cLigne.h"
#include "cEnvironnement.h"

/*! \brief Destructeur
	\author Hugues Romain
	\date 2002
*/
cLigne::~cLigne()
{
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
	\param environment Environnement auquel appartiendra la ligne
	\author Hugues Romain
	\date 2002
	*/
cLigne::cLigne(const string& newCode, cAxe* const newAxe, cEnvironnement* const environment)
: vAxe(newAxe)
, vCirculation(environment->PremiereAnnee(), environment->DerniereAnnee(), 0, "")
, vResa(environment->getResa(0))
, vVelo(environment->getVelo(0))
, vHandicape(environment->getHandicape(0))
, vCode(newCode)
{	
	// Valeurs par d�faut
	vReseau = NULL;
	vMateriel = NULL;
	vTarif =NULL;
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



bool cLigne::allowAddServiceHoraire(const cHoraire* tbHoraires) const
{
	bool ControleDepassementPositif = true;
	bool ControleDepassementNegatif = true;
	
	// D�termination du numero de service futur
	// Insertion du service: d�termination du rang
	
	for (size_t iNumeroService=0; iNumeroService< vTrain.size(); iNumeroService++)
		if (tbHoraires[1] < _lineStops.front() ->getHoraireDepartPremier(iNumeroService))
			break;
	//END PORTAGE LINUX
	if (iNumeroService== vTrain.size())
		ControleDepassementPositif = false;
	if (iNumeroService==0)
		ControleDepassementNegatif = false;

	size_t i=0;
	for (LineStops::const_iterator iter = _lineStops.begin();
			iter != _lineStops.end();
			++iter)
	{
		cGareLigne* curGareLigne = *iter;
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
	}
	return true;
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
void cLigne::addGareLigne(cGareLigne* newGareLigne)
{
	_lineStops.push_back(newGareLigne);

	if (_lineStops.size() > 1)
	{
		// Chainages d�part/arriv�e
		for (LineStops::reverse_iterator riter = _lineStops.rbegin();
			(riter != _lineStops.rend())
				&& (
					(*riter)->getArriveeSuivante() == NULL 
					|| (*riter)->getArriveeCorrespondanceSuivante() == NULL 
					|| newGareLigne->getDepartPrecedent() == NULL 
					|| newGareLigne->getDepartCorrespondancePrecedent() == NULL
				);
			++riter)
		{
			cGareLigne* __GL = *riter;
			// On chaine uniquement les relations entre A et A, D et D, A et D si arr�ts diff�rents, D et A si arr�ts diff�rents
			if (__GL->ArretPhysique()->getLogicalPlace() != newGareLigne->ArretPhysique()->getLogicalPlace() || __GL->TypeDA() == newGareLigne->TypeDA())
			{
				// Chainage arriv�es suivantes
				if (__GL->getArriveeSuivante() == NULL && newGareLigne->EstArrivee())
					__GL->setArriveeSuivante(newGareLigne);
				if (__GL->getArriveeCorrespondanceSuivante() == NULL && newGareLigne->ArretPhysique()->getLogicalPlace()->CorrespondanceAutorisee())
					__GL->setArriveeCorrespondanceSuivante(newGareLigne);

				if (__GL->EstDepart() && newGareLigne->getDepartPrecedent() == NULL)
					newGareLigne->setDepartPrecedent(__GL);
				if (__GL->EstDepart() && newGareLigne->getDepartCorrespondancePrecedent() == NULL && __GL->ArretPhysique()->getLogicalPlace()->CorrespondanceAutorisee())
					newGareLigne->setDepartCorrespondancePrecedent(__GL);
			}
		}
	}
}


/*! \brief Cloture d'une ligne
	\author Hugues Romain
	\date 2002
*/
void cLigne::Ferme()
{
	// Suppression du statut arret de d�part de pas d'arriv�e � desservir et vv
	for (LineStops::const_iterator iter = _lineStops.begin();
		iter != _lineStops.end();
		++iter)
	{
		cGareLigne* __GL = *iter;
		if (__GL->getArriveeSuivante() == NULL)
			__GL->setTypeDA(cGareLigne::Arrivee);
		if (__GL->getDepartPrecedent() == NULL)
			__GL->setTypeDA(cGareLigne::Depart);
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
	cGareLigne* lastGL = _lineStops.back();
	for (size_t iNumeroService=0; iNumeroService< vTrain.size(); iNumeroService++)
	{
		if (lastGL->getHoraireArriveeDernier(iNumeroService).JPlus() != _lineStops.front()->getHoraireDepartPremier(iNumeroService).JPlus())
		{
			vCirculation.RAZMasque(true);
			break;
		}
		vTrain.at(iNumeroService)->getJC()->SetInclusionToMasque(vCirculation);
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
cTrain* cLigne::getTrain(size_t __NumeroService) const
{
	return (__NumeroService < vTrain.size()) ? vTrain.at(__NumeroService) : NULL;
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

bool cLigne::SetAUtiliserDansCalculateur(bool __Valeur)
{
	_AUtiliserDansCalculateur = __Valeur;
	return true;
}

void cLigne::setImage(const cTexte& newImage)
{
	vImage.Vide();
	vImage << newImage;
}

void cLigne::setLibelleComplet(const cTexte& newLibelleComplet)
{
	vLibelleComplet.Vide();
	vLibelleComplet << newLibelleComplet;
}
	
void cLigne::setAAfficherSurTableauDeparts(bool newVal)
{
	vAAfficherSurTableauDeparts = newVal;
}

bool cLigne::AAfficherSurTableauDeparts() const
{
	return(vAAfficherSurTableauDeparts);
}

void cLigne::setResa(cModaliteReservation* newVal)
{
	vResa = newVal;
}

void cLigne::setVelo(cVelo* newVal)
{
	vVelo = newVal;
}

void cLigne::setGirouette(const cTexte& newGirouette)
{
	vGirouette.Vide();
	vGirouette << newGirouette;
}
	
// SET Gestion du filtre Velo
cVelo* cLigne::getVelo() const
{
	return(vVelo);
}

void cLigne::setHandicape(cHandicape* newVal)
{
	vHandicape = newVal;
}

cHandicape* cLigne::getHandicape() const
{
	return(vHandicape);
}

void cLigne::setTarif(cTarif* newVal)
{
	vTarif = newVal;
}

const cTarif* cLigne::getTarif() const
{
	return(vTarif);
}

const cAlerte& cLigne::getAlerte() const
{
	return vAlerte;
}

//END SET

void cLigne::setMateriel(cMateriel* newVal)
{
	vMateriel = newVal;
}


cAxe* cLigne::Axe() const
{
	return(vAxe);
}

cMateriel* cLigne::Materiel() const
{
	return(vMateriel);
}

const std::string& cLigne::getCode() const
{
	return(vCode);
}

const cTexte& cLigne::getNomPourIndicateur() const
{
	return(vNomPourIndicateur);
}


void cLigne::setStyle(const cTexte& newStyle)
{
	vStyle.Vide();
	vStyle << newStyle;
}

void cLigne::setLibelleSimple(const cTexte& newNom)
{
	vLibelleSimple.Vide();
	vLibelleSimple << newNom;
	if (!vNomPourIndicateur.Taille())
	{
		vNomPourIndicateur.Vide();
		vNomPourIndicateur << vLibelleSimple;
	}
}

void cLigne::setNomPourIndicateur(const cTexte& newNom)
{
	vNomPourIndicateur.Vide();
	vNomPourIndicateur << newNom;
}


void cLigne::setReseau(cReseau* newReseau)
{
	vReseau = newReseau;
}


cModaliteReservation* cLigne::GetResa() const
{
	return(vResa);
}

bool cLigne::AAfficherSurIndicateurs() const
{
	return(vAAfficherSurIndicateurs);
}

void cLigne::setAAfficherSurIndicateurs(bool newVal)
{
	vAAfficherSurIndicateurs = newVal;
}

cReseau* cLigne::getReseau() const
{
	return(vReseau);
}

/*!	\brief Accesseur libell� complet de la ligne
	\return Si un libell� complet est d�fini, renvoie le libell� complet de la ligne. Sinon renvoie le libell� simple de la ligne.
	\author Hugues Romain
	\date 2005
*/
const cTexte& cLigne::getLibelleComplet() const
{
	return vLibelleComplet;
}



/*!	\brief Accesseur libell� simple de la ligne
	\return Le libell� simple de la ligne.
	\author Hugues Romain
	\date 2005
*/
const cTexte& cLigne::getLibelleSimple() const
{
	return vLibelleSimple;
}



/*!	\brief Accesseur style CSS de la ligne
	\return R�f�rence constante vers le style de la ligne
	\author Hugues Romain
	\date 2005
*/
const cTexte& cLigne::getStyle() const
{
	return vStyle;
}



/*!	\brief Accesseur logo image de la ligne
	\return R�f�rence constante vers le chemin d'acc�s g�n�rique au logo image de la ligne
	\author Hugues Romain
	\date 2005
*/
const cTexte& cLigne::getImage() const
{
	return vImage;
}



/*!	\brief Code de la ligne sur nouvelle chaine de caract�res HTML
	\author Hugues Romain
	\date 2005
*/ /*
inline cTexte cLigne::Code() const
{
	cTexte tResultat;
	Code(tResultat);
	return tResultat;
} */


void cLigne::addService(cTrain* const service)
{
	vTrain.push_back(service);
}

cAlerte& cLigne::getAlerteForModification()
{
	return vAlerte;
}