/*! \file Environnement.cpp
  \brief Impl�mentation classe cEnvironnement : chargement de tous les fichiers
  \author Hugues Romain
  \date 2000-2001
*/

#include "cEnvironnement.h"
#include "cModaliteReservationEnLigne.h"
#include <string.h>
#include "cCommune.h"
#include "cIndicateurs.h"
#include "cAxe.h"
#include "cLigne.h"
#include "cPhoto.h"
#include "cArretPhysique.h"
#include "cReseau.h"
#include "cHandicape.h"
#include "cVelo.h"
#include "cTarif.h"
#include "cTrain.h"
#include "cModaliteReservation.h"
#include "cModaliteReservationEnLigne.h"

#include "04_time/Schedule.h"


#include <iostream>

using namespace std;
using namespace synmap;


#ifdef UNIX
#include <pthread.h>
pthread_mutex_t mutex_environnement = PTHREAD_MUTEX_INITIALIZER;
#endif

/** Constructeur.
    \param __NombreCalculateurs Nombre d'espaces de calcul � allouer
    \author Hugues Romain
    \date 2000-2001
 
    Les objets environnement sont initialis�s par les valeurs par d�faut suivantes :
    - Tableau des calendriers de circulation vide
    - Plage de date de couverture de l'environnement de taille nulle
 
    Les �l�ments suivants sont d'ores et d�j� initialis�s :
    - Cr�ation d'espaces de calcul pour threads.
 
    @todo Generer une exception si fichier pas ouvert (remplacer les envOk = false)
*/
cEnvironnement::cEnvironnement( string directory, string pathToFormat, size_t id )
  : Code( id )
  , _path( directory )
{
  // Plage de date de taille nulle
  vDateMin.updateDate( synthese::time::TIME_MAX );
  vDateMax.updateDate( synthese::time::TIME_MIN );


}



/** Destructor.
    \author Hugues Romain
    @author Marc Jambert
    \date 2000-2006
*/
cEnvironnement::~cEnvironnement()
{
  for ( ReservationRulesMap::const_iterator iter = vResa.begin();
	iter != vResa.end(); ++iter )
    delete iter->second;
  vResa.clear();

  for ( NetworksMap::const_iterator iter = vReseau.begin();
	iter != vReseau.end(); ++iter )
    delete iter->second;
  vReseau.clear();

  for ( BikeCompliancesMap::const_iterator iter = vVelo.begin();
	iter != vVelo.end(); ++iter )
    delete iter->second;
  vVelo.clear();

  for ( HandicappedCompliancesMap::const_iterator iter = vHandicape.begin();
	iter != vHandicape.end(); ++iter )
    delete iter->second;
  vHandicape.clear();

  for ( FaresMap::const_iterator iter = vTarif.begin();
	iter != vTarif.end(); ++iter )
    delete iter->second;
  vTarif.clear();

  for ( RollingStockMap::const_iterator iter = vMateriel.begin();
	iter != vMateriel.end(); ++iter )
    delete iter->second;
  vMateriel.clear();

}



/*! Recherche de commune par nom.
  \param name Nom a rechercher
  \param n Nombre de communes � retourner
  \return Liste de communes pouvant correspondre
 
  @todo HR terminer l'implémentation
*/
vector<cCommune*> cEnvironnement::searchTown( const std::string& name, size_t n ) const
{
  // Recherche
  set
    <interpretor::Result> matches = _towns.search( name, n );

  // METTRE ICI UNE DISCUSSION SUR L'AMBIGUITE AVEC CRITERES METIER

  // Sortie
  vector<cCommune*> result;
  // for (size_t i=0; i<matches.size(); i++)
  //  result.push_back(getTown(matches[i]));
  return result;
}



/*! Recherche de commune dans l'environnement avec création si non trouvée.
  \param Entree Nom de la commune � trouver
  \return La commune trouv�e ou cr��e
  \author Hugues Romain
  \date 2001-2006
*/ 
/*cCommune* cEnvironnement::GetCommuneAvecCreation(const std::string& name)
  {
  for (Interpretor::Index i=0; i<_towns.size(); i++)
  if (getCommune(i)->getName() == name)
  return getCommune(i);
 
  return addTown(new cCommune(INCONNU, name));
  }
*/


/*! \brief Accesseur point d'arr�t, avec cr�ation de point d'arr�t si index non existant
  \param __NiveauCorrespondance Type de correspondances autoris�es � l'arr�t
  \param __Index Num�ro de point d'arr�t
  \return Pointeur sur l'objet trouv� ou cr��
*/ 
/*LogicalPlace* cEnvironnement::GetGareAvecCreation(tNiveauCorrespondance __NiveauCorrespondance, int __Index)
  {
  // Recherche arr�t existant
  if (__Index != INCONNU || !GetArretLogique(__Index))
  {  
  // Correction �ventuelle de l'index
  if (__Index < 0)
  __Index = INCONNU;
 
  _ArretLogique.SetElement(new LogicalPlace(__Index, __NiveauCorrespondance), __Index);
  }
 
  return getArretLogique(__Index);
  }
*/



/*
  cAccesPADe** cEnvironnement::CommuneTextToPA(tNumeroCommune NumeroCommune, char* Entree, size_t n)
  {
  // Variables
  cCommune* curCommune = Commune[NumeroCommune];
  cAccesPADe** tbPADe = (cAccesPADe**) malloc((n+1)*sizeof(cAccesPADe*));
  size_t nbPA;
  size_t j;
  char* Tampon = (char*) malloc(TAILLETAMPON*sizeof(char));
  for (nbPA = 0; nbPA<=n; nbPA++)
  tbPADe[nbPA] = NULL;
  nbPA=0;
 
  // Pas d'entr�e = liste de tous les PA
  if (Entree[0]==0)
  for (cAccesPADe* curAccesPADe=curCommune->PremiereGA; curAccesPADe!=NULL; curAccesPADe=curAccesPADe->Suivant)
  {
  tbPADe[nbPA] = curAccesPADe;
  nbPA++;
  if (nbPA == n)
  break;
  }
  else // Entree: liste des PA correspondant
  for (size_t LongueurComparaison=strlen(Entree); LongueurComparaison>0; LongueurComparaison--)
  {
  for (cAccesPADe* curAccesPADe=curCommune->PremiereGA; curAccesPADe!=NULL; curAccesPADe=curAccesPADe->Suivant)
  if (CompareChaine(Entree, curAccesPADe->Nom(), LongueurComparaison))
  {
  for (j=0; j<nbPA; j++)
  if (tbPADe[j] == curAccesPADe)
  break;
  if (j == nbPA)
  {
  tbPADe[nbPA] = curAccesPADe;
  nbPA++;
  if (strlen(curAccesPADe->Nom()) == LongueurComparaison)
  return(tbPADe);
  }
 
  if (nbPA == n)
  return(tbPADe);
  }
  if ((nbPA == 1) && (LongueurComparaison == strlen(Entree)))
  return(tbPADe);
  }
  return(tbPADe);
  }
*/ 
/*----------------*
  |                |
  |    Horaires    |
  |                |
  *----------------*/







      // cEnvironnement - NomLibreUnique
      // ____________________________________________________________________________
      // Fournit un nom unique de ligne sous la forme
      // NomBase/Cxx
      // Si NomBase contient deja /Cxx, xx sera remplac� par un code donnant
      // l'unicit�.
      // Gere les noms de lignes termin�s par ___

      // ERR: xx limit� � 99 voir si cela pose probleme

      // ERR: Pas de controle de xx si /Cxx trouv�
      // ____________________________________________________________________________
      /*void cEnvironnement::NomLigneUnique(std::string& NomBase) const
        {
	int DernierNumeroTrouve;
	int CodeTeste;
         
	// Extraction du nom de la ligne
	if (NomBase.Compare("/C", 2, NomBase.GetTaille() - 4))
	DernierNumeroTrouve = NomBase.GetNombre(0, NomBase.GetTaille() - 2);
	else
	{
	DernierNumeroTrouve = 0;
	NomBase << "/C00";
	}
         
	// Parcours des lignes pour obtenir le bon num�ro
	for (cLigne* curLigne = PremiereLigne(); curLigne!=NULL; curLigne = curLigne->Suivant())
	{
	if (curLigne->getCode().GetTaille() == NomBase.GetTaille() && curLigne->getCode().Compare(NomBase, NomBase.GetTaille() - 2))
	{
	CodeTeste = curLigne->getCode().GetNombre(0, NomBase.GetTaille() - 2);
	if (CodeTeste > DernierNumeroTrouve)
	DernierNumeroTrouve = CodeTeste;
	}
	}
         
	// Ecriture du nom definitif
	DernierNumeroTrouve++;
	NomBase.vPosition = NomBase.GetTaille() - 2;
	NomBase.Copie(DernierNumeroTrouve, 2, false);
        }*/ 
      // � Hugues Romain 2002
      // ____________________________________________________________________________







      /*! \brief Recherche d'une ligne � partir de son code
	\param code Code � comparer
	\author Hugues Romain
	\date 2000
      */
      cLigne* cEnvironnement::GetLigne( const string& code ) const
        {
	  LinesMap::const_iterator iter = _lines.find( code );
	  return iter == _lines.end() ? NULL : iter->second;
        }



      void cEnvironnement::ConstruitColonnesIndicateur( cIndicateurs* curIndicateur )
        {
	  /* // Declarations
             bool LigneSelectionnee;
             bool ZoneDepartTerminee;
             bool PassagePasse;
             cColonneIndicateurs* curCI;
             cGareIndicateurs* curGI;
             cGareIndicateurs* GIDepart=NULL;
             cGareLigne* curGareLigne=NULL;
             cGareLigne* GareLigneArrivee=NULL;
             cGareLigne* GareLigneDepart=NULL;
             cGareLigne* GareLigneDepart2=NULL;
             cLigne* curLigne;
             size_t NumeroGare;
             tNumeroService iNumeroService;
             curIndicateur->Reset();
             
             // A: Pr�paration des colonnes
             for (curLigne = vPremiereLigne; curLigne!=NULL; curLigne = curLigne->Suivant())
             {
	     if (curLigne->AAfficherSurIndicateurs())
	     {
	     LigneSelectionnee = false;
	     PassagePasse = false;
             
	     // A1: S�lection de la ligne: Un d�part parmi les d�parts de l'indicateur
	     for (curGI = curIndicateur->getPremiereGI(); curGI!=NULL; curGI = curGI->getSuivant())
	     {
	     if (curGI->EstDepart())
	     {
	     for (curGareLigne = curLigne->PremiereGareLigne(); curGareLigne!=NULL; curGareLigne = curGareLigne->Suivant())
	     {
	     if (curGareLigne->EstDepart() && curGareLigne->ArretLogique() == curGI->getArretLogique())
	     {
	     LigneSelectionnee = true;
	     GareLigneDepart = curGareLigne;
	     GareLigneDepart2 = curGareLigne;
	     if (curGI->EstArrivee() || curGI->Obligatoire() == PassageSuffisant)
	     PassagePasse = true;
	     break;
	     }
	     }
	     if (LigneSelectionnee)
	     {
	     GIDepart = curGI;
	     break;
	     }
	     }
	     }
             
             
             
	     // A2: S�lection de la ligne: Une arriv�e apr�s le d�part
             
	     if (LigneSelectionnee)
	     {
	     LigneSelectionnee = false;
             
	     for (curGI = GIDepart->getSuivant(); curGI != NULL; curGI = curGI->getSuivant())
	     {
	     if ((curGI->EstArrivee() && (PassagePasse || curGI->Obligatoire()==PassageSuffisant)) || (curGI->EstDepart() && curGI->EstArrivee()))
	     {
	     for (curGareLigne = GareLigneDepart->getArriveeSuivante(); curGareLigne != NULL; curGareLigne = curGareLigne->getArriveeSuivante())
	     {
	     if (curGareLigne->ArretLogique() == curGI->getArretLogique())
	     {
	     LigneSelectionnee = true;
	     break;
	     }
	     }
	     if (LigneSelectionnee)
	     break;
	     }
	     }
	     }
             
	     // A3: S�lection de la ligne par les jours de circulation (gain de temps)
	     if (LigneSelectionnee)
	     if (!curIndicateur->getJC().UnPointCommun(curLigne->GetCirculation()))
	     LigneSelectionnee = false;
             
             
	     // B Cr�ation de la donn�e
	     // B1: Remplissage des colonnes
             
	     if (LigneSelectionnee)
	     {
	     // Parcours colonne par colonne
	     for (iNumeroService = 0; iNumeroService < curLigne->getServices().size(); iNumeroService++)
	     {
	     // Test de JC et rejet des cadences
	     if (curLigne->GetTrain(iNumeroService)->getJC()->UnPointCommun(curIndicateur->getJC()) && !curLigne->GetTrain(iNumeroService)->EstCadence())
	     {
	     // Allocation
	     // Renvoi: on stocke uniquement le JC pour un traitement ult�rieur
	     // (pour permettre de fusionner deux colonnes, et pour gloabliser le code renvoi)
	     curCI = new cColonneIndicateurs(curIndicateur->NombreGares(), curLigne, curLigne->GetTrain(iNumeroService)->getJC());
             
	     // Passages
	     NumeroGare = 0;
	     ZoneDepartTerminee = false;
	     GareLigneDepart = GareLigneDepart2;
             
	     //SET PORTAGE LINUX
	     //curCI->CopiePostScript("[");
	     curCI->CopiePostScript(std::string("["));
	     //END PORTAGE
             
	     for (curGI = curIndicateur->getPremiereGI(); curGI != NULL; curGI = curGI->getSuivant())
	     {
	     for (curGareLigne = GareLigneDepart; curGareLigne != NULL; curGareLigne = curGareLigne->Suivant())
	     {
	     if (curGareLigne->ArretLogique() == curGI->getArretLogique() && (curGareLigne->EstDepart() == curGI->EstDepart() || curGareLigne->EstArrivee() == curGI->EstArrivee()))
	     {
	     if (curGI->EstArrivee())
	     ZoneDepartTerminee = true;
	     if (curGareLigne->EstDepart() && curGI->EstDepart())
	     {
	     curCI->setColonne(NumeroGare, &curGareLigne->getHoraireDepartPremier(iNumeroService));
	     GareLigneArrivee = curGareLigne;
	     GareLigneDepart = curGareLigne->Suivant();
	     break;
	     }
	     else
	     {
	     curCI->setColonne(NumeroGare, &curGareLigne->getHoraireArriveePremier(iNumeroService)); // IL Y AVAIT DEPART
	     GareLigneArrivee = curGareLigne;
	     GareLigneDepart = curGareLigne->Suivant();
	     break;
	     }
	     }
	     }
	     if (curGareLigne == NULL)
	     curCI->setColonne(NumeroGare, NULL);
	     NumeroGare++;
	     }
             
	     curCI->CopiePostScript(std::string("]"));
             
	     if (GareLigneDepart2->ArretLogique() == curLigne->PremiereGareLigne()->ArretLogique())
	     curCI->setOrigineSpeciale(Terminus);
	     if (GareLigneArrivee->ArretLogique() == curLigne->PremiereGareLigne()->Destination()->ArretLogique())
	     curCI->setDestinationSpeciale(Terminus);
             
	     // B2: Stockage des colonnes et classement
	     curIndicateur->Add(curCI, curLigne->GetTrain(iNumeroService)->getJC());
             
	     }
	     }
	     }
	     }
             }
             curIndicateur->ConstruitRenvois();
	  */
        }

      bool cEnvironnement::ConstruitIndicateur()
        {
	  /*
	  //    char* Tampon = (char*) malloc(1000*sizeof(char));
	  // cCommune* curCommune = NULL;
	  char c;
	  size_t NumeroColonne;
	  signed int HDispo;
	  signed int HRequis;
	  size_t NombreTableaux;
	  size_t NombreTableauxAEcrire;
	  size_t NombreColonnes;
	  bool RenvoisEcrits;
	  // size_t NumeroPageAbsolu;
	  size_t NumeroPageRelatif = 0;

	  ofstream FichierSortie;
	  // SET PORTAGE LINUX
	  FichierSortie.open(NOMFICHIERINDICATEURS.Texte());
	  if (!FichierSortie.is_open())
	  {
	  cout << "*** ERREUR Impossible d'ouvrir le fichier " << NOMFICHIERINDICATEURS << "\n";
	  return false;
	  }
	  //END PORTAGE

	  ifstream FichierModele;

	  // SET PORTAGE LINUX
	  FichierModele.open(NOMFICHIERMODELE.Texte());
	  if (!FichierModele.is_open())
	  {
	  cout << "*** ERREUR Impossible d'ouvrir le fichier " << NOMFICHIERMODELE << "\n";
	  return false;
	  }
	  //END PORTAGE LINUX

	  while (!FichierModele.eof())
	  {
	  c = (char) FichierModele.get();
	  if (FichierModele.eof())
	  break;
	  FichierSortie.put(c);
	  }

	  HDispo = IndicateursHaut;

	  FichierSortie << "\n";

	  for (tNumeroIndicateur iIndicateur=0; iIndicateur< vNombreIndicateurs; iIndicateur++)
	  {
	  if (vIndicateurs[iIndicateur]->CommencePage())
	  {
	  HDispo = IndicateursHaut;
	  FichierSortie << "showpage\n";
	  }

	  // APPEL A B
	  ConstruitColonnesIndicateur(vIndicateurs[iIndicateur]);



	  // Comptage des �l�ments � �crire

	  NombreColonnes = vIndicateurs[iIndicateur]->NombreColonnes();
	  if (NombreColonnes == 0)
	  FichierSortie << "newpath (" << vIndicateurs[iIndicateur]->getTitre() << " VIDE) 50 50 moveto show\n";
	  else
	  {
	  NombreTableaux = ((NombreColonnes-1) / IndicateursNombreColonnesMax) + 1;
	  if (vIndicateurs[iIndicateur]->NombreRenvois())
	  RenvoisEcrits = false;
	  else
	  RenvoisEcrits = true;
	  NumeroPageRelatif = 0;
	  NumeroColonne = 0;

	  // Ecriture jusqu'a avoir tout ecrit
	  while (NombreTableaux != 0 || RenvoisEcrits == false)
	  {
	  // Tentative d'�criture totale
	  if (vIndicateurs[iIndicateur]->NombreRenvois())
	  HRequis = IndicateursMarge + IndicateursMarge + IndicateursDistanceTableauHaut + (IndicateursNombreLignesSupplementaires + vIndicateurs[iIndicateur]->NombreGares() + 1) * IndicateursHauteurLigne * NombreTableaux + (vIndicateurs[iIndicateur]->NombreRenvois()+1) * IndicateursHauteurLigne;
	  else
	  HRequis = IndicateursMarge + IndicateursMarge + IndicateursDistanceTableauHaut + (IndicateursNombreLignesSupplementaires + vIndicateurs[iIndicateur]->NombreGares() + 1) * IndicateursHauteurLigne * NombreTableaux;

	  if (HRequis < HDispo)
	  {
	  vIndicateurs[iIndicateur]->EcritTableaux(HDispo, NumeroColonne, NombreTableaux, true, NumeroPageRelatif, FichierSortie);
	  RenvoisEcrits = true;
	  NombreTableaux = 0;
	  HDispo -= (HRequis - IndicateursMarge);
	  }
	  else // Tentative d'�criture partielle
	  {
	  RenvoisEcrits = false;
	  for (NombreTableauxAEcrire = NombreTableaux; NombreTableauxAEcrire != 0; NombreTableauxAEcrire--)
	  {
	  HRequis = IndicateursMarge + IndicateursMarge + IndicateursDistanceTableauHaut + (IndicateursNombreLignesSupplementaires + vIndicateurs[iIndicateur]->NombreGares() + 1) * IndicateursHauteurLigne * NombreTableauxAEcrire;
	  if (HRequis < HDispo)
	  break;
	  }

	  // Echec: nouvelle page
	  if (NombreTableauxAEcrire == 0)
	  {
	  HDispo = IndicateursHaut;
	  FichierSortie << "showpage\n";
	  }
	  else
	  {
	  if (NumeroPageRelatif == 0)
	  NumeroPageRelatif = 1;
	  vIndicateurs[iIndicateur]->EcritTableaux(HDispo, NumeroColonne, NombreTableauxAEcrire, false, NumeroPageRelatif, FichierSortie);
	  NombreTableaux -= NombreTableauxAEcrire;
	  HDispo -= (HRequis - IndicateursMarge);
	  NumeroColonne += NombreTableauxAEcrire * IndicateursNombreColonnesMax;
	  NumeroPageRelatif++;
	  }
	  }
	  }
	  }
	  }
	  // Validation derniere page
	  FichierSortie << "showpage\n";

	  FichierSortie.close();
	  FichierModele.close();
	  */ 
	  return ( true );
        }









      /*
         
      // cEnvironnement - ConstruitLigne � partir de la liste de gares INUTILISE
      // ____________________________________________________________________________
      // L'axe n'est pas document�. Il DOIT �tre renseign� par la suite.
      // ____________________________________________________________________________
      cLigne* cEnvironnement::ConstruitLigne(LogicalPlace** tbGares, char* newCode, cMateriel* newMateriel)
      {
      cLigne* curLigne = new cLigne(newCode, NULL, this);
      curLigne->Materiel = newMateriel;
         
      cGareLigne* DepartPrecedent = NULL;
      cGareLigne* DepartCorrespondancePrecedent = NULL;
      cGareLigne*  PileDep[NOMBREGARESPILE];
      int    TaillePileDep=0;
         
      for (int i=0; tbGares[i]!=NULL; i++)
      {
      cGareLigne* curGareLigne = new cGareLigne(curLigne, 0, 1, Passage, tbGares[i], DepartPrecedent, DepartCorrespondancePrecedent, true);
      if (i==0)
      curLigne->PremiereGareLigne = curGareLigne;
      if (DepartPrecedent != NULL)
      DepartPrecedent->ArriveeSuivante = curGareLigne;
      DepartPrecedent = curGareLigne;
      if (tbGares[i]->CorrespondanceAutorisee())
      {
      for (; TaillePileDep==0; TaillePileDep--)
      PileDep[TaillePileDep]->ArriveeCorrespondanceSuivante = curGareLigne;
      DepartCorrespondancePrecedent = curGareLigne;
      }
      TaillePileDep++;
      PileDep[TaillePileDep]=curGareLigne;
      }
         
      return(curLigne);
      }
         
      // � Hugues Romain 2002
         
      // ____________________________________________________________________________
      */




      cJC* cEnvironnement::GetJC( const cJC::Calendar& MasqueAIdentifer, const cJC& JCBase ) const
        {
	  cJC * curJC;
	  cJC* JCIdentifie = NULL;

	  for ( CalendarsMap::const_iterator iter = _JC.begin();
		iter != _JC.end();
		++iter )
            {
	      curJC = iter->second;
	      //  if ((curJC->Categorie >= Categorie) && (TousPointsCommuns(curJC->JoursAnnee, Masque)))
	      if ( JCBase.TousPointsCommuns( *curJC, MasqueAIdentifer ) )
		if ( JCIdentifie == NULL )
		  JCIdentifie = curJC;
		else
		  if ( curJC->Categorie() < JCIdentifie->Categorie() )
		    JCIdentifie = curJC;
		  else
		    if ( curJC->Categorie() == JCIdentifie->Categorie() && JCBase.Card( *curJC ) < JCBase.Card( *JCIdentifie ) )
		      JCIdentifie = curJC;
            }
	  return ( JCIdentifie );
        }




      /*! \brief M�thode d'enregistrement d'un calendrier de circulation
	\param JCAAjouter Le calendrier � ajouter � l'environnement
	\return false si OK, true si un JC a été écrasé
	\todo Placer une section critique pour un usage temps r�el
	\author Hugues Romain
	\date 2003-2005
         
        Pour rappel si besoin, anciens param�tres suppl�mentaires (code en commentaires) :
	Sauvegarde        : Vrai  -> effectue la sauvegarde m�moire morte
	ContenuSauvegarde : Vide  -> sauvegarde les jours circul�s un par un dans un
	sous-fichier.
	Sinon -> sauvegarde le contenu de la variable sur le
	fichier
      */
      bool cEnvironnement::Enregistre( cJC* JCAAjouter )
        {
	  // Se r�server la main
#ifdef UNIX
	  pthread_mutex_lock( &mutex_environnement );
#endif

	  // Search of calendar with same id
	  CalendarsMap::const_iterator iter = _JC.find( JCAAjouter->getId() );

	  // deletion of old calendar if exists
	  if ( iter != _JC.end() )
	    delete iter->second;

	  // link to the new calendar
	  _JC[ JCAAjouter->getId() ] = JCAAjouter;

	  // Sauvegarde m�moire morte
	  /* if (Sauvegarde)
             {
	     // Ouverture fichier
	     cSauvegarde* curSauvegarde = new cSauvegarde(vNomFichierJC);
	     curSauvegarde->CopieJusqueA(Code, 4);
             
	     // Sauvegarde en tete JC
	     *curSauvegarde << FINL;
	     *curSauvegarde << "[" << TXT2(JCAAjouter->Code(), TAILLECODEJC) << JCAAjouter << FINL;
             
	     // Contenu
	     // SET PORTAGE LINUX
	     //if (ContenuSauvegarde.GetTaille())
	     if (!strcmp(ContenuSauvegarde,""))
	     //END PORTAGE LINUX
	     *curSauvegarde << ContenuSauvegarde;
	     else
	     {
	     std::string NomFichierCommandes;
	     NomFichierCommandes << vNomRepEnv;
	     NomFichierCommandes.Repertoire();
	     NomFichierCommandes << "JoursCirculation\\JC" << TXT(Code) << JCEXTENSION;
	     cSauvegarde* curSauvegarde2 = new cSauvegarde(NomFichierCommandes);
	     synthese::time::Date DateDebut;
	     DateDebut.setDate(1,1,vPremiereAnnee);
	     synthese::time::Date DateFin;
	     DateFin.setDate(31,12,vDerniereAnnee);
	     for (; DateDebut <= DateFin; DateDebut++)
	     if (JCAAjouter->Circule(DateDebut))
	     {
	     *curSauvegarde2 << "+";
	     DateDebut.toFormatInterne(*curSauvegarde2);
	     *curSauvegarde2 << FINL;
	     }
	     curSauvegarde2->SauteObjet();
	     delete curSauvegarde2;
	     #ifdef ClasseAdmin
             // ATTENTION La desactivation de cette partie dans la version non Admin
             // implique la non conservation des infos d'importation en cas de
             
             // sauvegarde � l'aide de cette fonction !
             
             // R�gler cette question avant utilisation de la fonction addJC
             // sur la version SEMVAT
	     if (JCAAjouter->ImportSource())
	     {
	     *curSauvegarde << "I" << TXT2(JCAAjouter->ImportSource()->Code(), 6);
	     JCAAjouter->ImportChaineCle(*curSauvegarde);
	     *curSauvegarde << FINL;
	     }
	     #endif
             
	     *curSauvegarde << "+>JoursCirculation\\JC" << TXT(Code) << FINL;
	     }
             
	     // Fermeture du fichier
	     delete curSauvegarde;
             }*/

	  // Rendre la main
#ifdef UNIX

	  pthread_mutex_unlock( &mutex_environnement );
#endif

	  // Sortie
	  return iter != _JC.end();
        }



      void cEnvironnement::JCSupprimerInutiles( bool Supprimer )
        {
	  /* // Allocation
             bool* JCUtile = (bool*) calloc(vNombreJC, sizeof(bool));
             
             // D�termination des JC utiles dans les horaires
             for (cLigne* curLigne = PremiereLigne(); curLigne != NULL; curLigne = curLigne->Suivant())
	     for (tNumeroService iNumeroService = 0; iNumeroService != curLigne->getServices().size(); iNumeroService++)
	     JCUtile[curLigne->getCirculation(iNumeroService).Index()] = true;
             
             // D�termination des JC utiles dans les d�finitions de JC
             ChargeFichierJC(false, JCUtile);
             
             // Affichage des JC inutiles
             ofstream FichierListe;
             FichierListe.open(NOMFICHIERJCINUTILES.Texte());
             if (!FichierListe.is_open())
             {
	     cout << "*** ERREUR Impossible d'ouvrir le fichier " << NOMFICHIERJCINUTILES << "\n";
	     return;
             }
             
             int iNumeroJC;
             for (iNumeroJC = 0; iNumeroJC < vNombreJC; iNumeroJC++)
             {
	     if (!JCUtile[iNumeroJC] && GetJC(iNumeroJC) != NULL)
	     {
	     FichierListe << "JC Inutile - " << iNumeroJC << FINL;
	     }
             }
             FichierListe.close();
             
             // Suppression des JC
             if (Supprimer)
             {
	     for (iNumeroJC = 0; iNumeroJC < vNombreJC; iNumeroJC++)
	     if (!JCUtile[iNumeroJC] && GetJC(iNumeroJC) != NULL)
	     supprimerJC(getJC(iNumeroJC));
             }
	  */
        }




      /*! \brief Compte le nombre de lignes dont le code correspond au masque fourni
	\param MasqueCode Masque de comparaison du code de ligne
	\author Hugues Romain
	\date 2003
         
	- Si le masque est vide, toutes les lignes sont compt�es
	- Si le masque est non vide, toutes les lignes dont le d�but du code correspond au masque sont compt�es
	- Si le masque est non vide et compos� de plusieurs codes s�par�s par des points virgules, alors les lignes sont successivement compar�es � chaque code fourni
      */ 
      /*
        size_t cEnvironnement::NombreLignes(const std::string& MasqueCode) const
        {
	// Variables locales
	int lNombreLignes = 0;
	int lLongueur = 0;
	int lPosition = 0;
         
	// Cas 1: Code fourni vide: comptage de toutes les lignes
	if (!MasqueCode.Taille())
	return _lines.size();
         
	// Cas 2: Un seul code fourni
	else
	{
	while (lPosition < MasqueCode.Taille())
	{
	// Extraction de la prochaine d�sgnation de ligne (s�parateur ;)
	lLongueur = MasqueCode.RechercheOccurenceGauche(';', 1, lPosition) - lPosition;
	if (lLongueur == MasqueCode.Taille() - lPosition - 1)
	lLongueur += 1;
         
	// Boucle sur les lignes de l'environnement
	for (LinesMap::const_iterator iter = _lines.begin(); iter != _lines.end(); ++iter)
	{
	// Correspondance au code fourni si non nul, comptage syst�matique sinon
	if (!MasqueCode.Taille() || iter->second->getCode().substr(0,  == .Compare(MasqueCode, lLongueur, 0, lPosition))
	lNombreLignes++;
	}
         
	// Avancement de la position
	lPosition += lLongueur;
	lPosition++;
	}
	}
         
	// Sortie
	return(lNombreLignes);
        }
      */


      /*! \brief Date interpr�t�e en fonction d'un texte descriptif et des donn�es de l'environnement
	\param Texte Texte contenant la date ou la commande
	\return La date interpr�t�e d'apr�s le texte
	\author Hugues Romain
	\date 2005
	\warning Les param�tres ne sont pas contr�l�s
         
        La date retourn�e est la suivante, selon le texte entr� :
	- date au format texte interne : date transcrite (pas de contr�le) (ex : 20070201 => 1/2/2007)
	- commande de date classique (synthese::time::TIME_MIN ('m'), synthese::time::TIME_MAX ('M'), synthese::time::TIME_CURRENT ('A'), synthese::time::TIME_UNKNOWN ('?')) : la date correspondante (voir synthese::time::Date::setDate())
	- texte vide : identique � synthese::time::TIME_CURRENT
	- TEMPS_MIN_ENVIRONNEMENT ('e') : Date minimale possible dans les calendriers de l'environnement (1/1/premi�re ann�e) (voir cEnvironnement::DateMinPossible())
	- TEMPS_MAX_ENVIRONNEMENT ('E') : Date maximale possible dans les calendriers de l'environnement (31/12/derni�re ann�e) (voir cEnvironnement::DateMaxPossible())
	- synthese::time::TIME_MIN_CIRCULATIONS ('r') : Premi�re date o� circule au moins un service (voir cEnvironnement::DateMinReelle())
	- TEMPS_MAX_CIRCULATIONS ('R') : Derni�re date o� circule au moins un service (voir cEnvironnement::DateMaxReelle())
         
        La relation suivante est v�rifi�e en fonctionnement nominal : \f$ TEMPS_{INCONNU}<=TEMPS_{MIN}<=TEMPS_{MIN ENVIRONNEMENT}<=TEMPS_{MIN CIRCULATIONS}<=TEMPS_{ACTUEL}<=TEMPS_{MAX CIRCULATIONS}<=TEMPS_{MAX ENVIRONNEMENT}<=TEMPS_{MAX} \f$.
      */
      synthese::time::Date cEnvironnement::dateInterpretee( const std::string& Texte ) const
        {
	  synthese::time::Date tempDate;
	  if ( !Texte.size () )
	    tempDate.updateDate( synthese::time::TIME_CURRENT );
	  else if ( Texte.size () == 1 )
	    switch ( Texte[ 0 ] )
	      {
	      case TEMPS_MIN_ENVIRONNEMENT:
		tempDate = DateMinPossible();
		break;
	      case TEMPS_MAX_ENVIRONNEMENT:
		tempDate = DateMaxPossible();
		break;
	      case TEMPS_MIN_CIRCULATIONS:
		tempDate = DateMinReelle();
		break;
	      case TEMPS_MAX_CIRCULATIONS:
		tempDate = DateMaxReelle();
		break;
	      default:
		tempDate.updateDate( Texte[ 0 ] );
	      }
	  else
	    tempDate = Texte;
	  return tempDate;
        }


      bool cEnvironnement::ControleNumerosArretCommuneDesignation( int nA, int nC, const std::string& txtA ) const
        {
	  return ( getLogicalPlace( nA ) != NULL
		   && getLogicalPlace( nA ) ->getTown() ->getId() == nC
		   && getLogicalPlace( nA ) ->getName() == txtA 
                   );
        }



      /*! \brief M�thode d'enregistrement d'une ligne dans un environnement
	\author Hugues Romain
	\date 2001-2005
	\param Obj Ligne � int�grer � l'environnement
	\return true si l'op�ration a �t� effectu�e avec succ�s
         
        Cette m�thode effectue l'initialisation des param�tres d'une ligne en fonction de l'environnement, ainsi que le cha�nage n�cessaire :
	- La modalit� de r�servation par d�faut de la ligne est la r�servation impossible
	- La modalit� de prise en charge des v�los par d�faut de la ligne est l'interdiction
	- La modalit� d'acceptation des handicap�s par d�faut est la non acceptation
	- Le calendrier de circulation de la ligne est param�tr� sur les ann�es de d�but et de fin de p�riode couverte par l'environnement
	- La ligne suivante de la ligne � rajouter est l'ancienne premier ligne de l'environnement
	- La ligne � ajouter devient la premi�re ligne de l'environnement
         
        \warning En raison des �critures de valeurs par d�faut, et des besoins de conna�tre ces valeurs pour compl�ter l'objet ligne, cette m�thode doit �tre appel�e le plus t�t possible apr�s la cr�ation de la ligne. En particulier, aucun service ne doit �tre ajout� � la ligne avant le lancement de cette m�thode.
      */
      bool cEnvironnement::Enregistre( cLigne* const Obj )
        {
	  cLigne * old = GetLigne( Obj->getCode() );
	  delete old;

	  _lines[ Obj->getCode() ] = Obj;

	  return old != NULL;
        }






      bool cEnvironnement::ControleNumeroTexteCommune( int nC, const std::string& txtC ) const
        {
	  return ( getTown( ( size_t ) nC ) != NULL
		   && getTown( nC ) ->getName() == txtC
                   );
        }




      cJC* cEnvironnement::GetJC( size_t n ) const
        {
	  CalendarsMap::const_iterator iter = _JC.find( n );
	  return iter == _JC.end() ? NULL : iter->second;
        }


      /*! \brief D�livre le premier index de calendrier de circulation libre dans l'environnement
	\return le premier index de calendrier de circulation libre dans l'environnement
	\author Hugues Romain
	\date 2001
      */
      size_t cEnvironnement::ProchainNumeroJC() const
        {
	  size_t expectedNumber = 1;
	  for ( CalendarsMap::const_iterator iter = _JC.begin();
		iter != _JC.end();
		++iter )
            {
	      if ( iter->first > expectedNumber )
		break;
	      ++expectedNumber;
            }

	  return expectedNumber;
        }



      /*! \brief Date maximale possible dans un calendrier de l'environnement
	\return La date maximale possible dans un calendrier de l'environnement
	\author Hugues Romain
	\date 2001-2005
         
        La date maximale possible correspond � la date cod�e par le dernier bit valide des tableaux de donn�es pr�sents dans les calendriers de circulation (voir classe cJourCirculation).
         
        Il s'agit donc du 31 d�cembre de la derni�re ann�e d�clar�e dans le fichier principal des calendriers de circulation (commande "Service")
      */
      synthese::time::Date cEnvironnement::DateMaxPossible() const
        {
	  synthese::time::Date tempDate;
	  tempDate.updateDate( synthese::time::TIME_MAX, synthese::time::TIME_MAX, vDerniereAnnee );
	  return ( tempDate );
        }


      /*! \brief Date minimale possible dans un calendrier de l'environnement
	\return La date minimale possible dans un calendrier de l'environnement
	\author Hugues Romain
	\date 2001-2005
         
        La date minimale possible correspond � la date cod�e par le premier bit valide des tableaux de donn�es pr�sents dans les calendriers de circulation (voir classe cJourCirculation).
         
        Il s'agit donc du 1 janvier de la premi�re ann�e d�clar�e dans le fichier principal des calendriers de circulation (commande "Service")
      */
      synthese::time::Date cEnvironnement::DateMinPossible() const
        {
	  synthese::time::Date tempDate;
	  tempDate.updateDate( synthese::time::TIME_MIN, synthese::time::TIME_MIN, vPremiereAnnee );
	  return ( tempDate );
        }


      cMateriel* cEnvironnement::GetMateriel( size_t n ) const
        {
	  RollingStockMap::const_iterator iter = vMateriel.find( n );
	  return iter == vMateriel.end() ? NULL : iter->second;
        }




      /** Enregistrement d'un materiel dans l'environnement
	  \param newVal Materiel a ajouter
	  @return false if OK (id was free), true else
	  \author Hugues Romain
	  \date 2001-2006
      */
      bool cEnvironnement::Enregistre( cMateriel* const materiel )
        {
	  cMateriel * old = GetMateriel( materiel->Code() );
	  delete old;
	  vMateriel[ materiel->Code() ] = materiel;
	  return old != NULL;
        }



      /*! \brief M�thode d'enregistrement d'une modalit� de r�servation dans l'environnement
	\param newVal Modalit� de r�servation � ajouter
	\return false if OK the index was free, true else
	\author Hugues Romain
	\date 2001-2005
      */
      bool cEnvironnement::Enregistre( cModaliteReservation* newVal )
        {
	  cModaliteReservation * old = getResa( newVal->Index() );
	  delete old;
	  vResa[ newVal->Index() ] = newVal;
	  return old != NULL;
        }



      /*! \brief M�thode d'enregistrement d'un objet tarification dans l'environnement
	\param newVal La tarification � ajouter � l'environnement
	\param newNum L'index de la tarification dans l'environnement
	\return true si un tarif a été écrasé, false sinon
	\author Hugues Romain
	\date 2001-2005
      */
      bool cEnvironnement::Enregistre( cTarif* const newVal )
        {
	  cTarif * old = getTarif( newVal->getNumeroTarif() );
	  delete old;
	  vTarif[ newVal->getNumeroTarif() ] = newVal;
	  return old != NULL;
        }




      bool cEnvironnement::Enregistre( cHandicape * const newVal )
        {
	  cHandicape * old = getHandicape( newVal->getId() );
	  delete old;
	  vHandicape[ newVal->getId() ] = newVal;
	  return old != NULL;
        }


      bool cEnvironnement::Enregistre( cVelo * const newVal )
        {
	  cVelo * old = getVelo( newVal->getId() );
	  delete old;
	  vVelo[ newVal->getId() ] = newVal;
	  return old != NULL;
        }

      bool cEnvironnement::Enregistre( LogicalPlace* const item )
        {
	  LogicalPlace * old = getLogicalPlace( item->getId() );
	  delete old;
	  _logicalPlaces[ item->getId() ] = item;
	  return old != NULL;
        }


      bool cEnvironnement::Enregistre( cReseau *newVal )
        {
	  cReseau * old = getReseau( newVal->getId() );
	  delete old;
	  vReseau[ newVal->getId() ] = newVal;
	  return old != NULL;
        }






      void cEnvironnement::addTown( cCommune* const town )
        {
	  _towns.add( town->getName(), town, town->getId() );
        }

      cCommune* cEnvironnement::getTown( size_t id ) const
        {
	  TownsMap::MapType::const_iterator iter = _towns.getMap().find( id );
	  return iter == _towns.getMap().end() ? NULL : iter->second;
        }

      LogicalPlace* cEnvironnement::getLogicalPlace( size_t id ) const
        {
	  LogicalPlacesMap::const_iterator iter = _logicalPlaces.find( id );
	  return iter == _logicalPlaces.end() ? NULL : iter->second;
        }



      /** Recherche de commune par nom, en correspondance exacte.
	  @param name Nom de la commune à chercher
	  @return Commune trouvée NULL sinon
      */
      cCommune* cEnvironnement::getTown( const std::string& name ) const
        {
	  for ( TownsMap::MapType::const_iterator iter = _towns.getMap().begin();
		iter != _towns.getMap().end();
		++iter )
	    if ( iter->second->getName() == name )
	      return iter->second;
	  return NULL;
        }






      int cEnvironnement::NombreAnnees() const
        {
	  return ( vDerniereAnnee - vPremiereAnnee );
        }

      int cEnvironnement::NombreAnnees( int AutreAnnee ) const
        {
	  return ( AutreAnnee - vPremiereAnnee );
        }

      int cEnvironnement::PremiereAnnee () const
        {
	  return ( vPremiereAnnee );
        }

      int cEnvironnement::DerniereAnnee () const
        {
	  return ( vDerniereAnnee );
        }



      cDocument* cEnvironnement::GetDocument( size_t n ) const
        {
	  DocumentsMap::const_iterator iter = _Documents.find( n );
	  return iter == _Documents.end() ? NULL : iter->second;
        }


      const std::string& cEnvironnement::getNomRepertoireHoraires() const
        {
	  return ( vNomRepertoireHoraires );
        }


      cModaliteReservation* cEnvironnement::getResa( size_t n ) const
        {
	  ReservationRulesMap::const_iterator iter = vResa.find( n );
	  return iter == vResa.end() ? NULL : iter->second;
        }


      cReseau* cEnvironnement::getReseau( size_t n ) const
        {
	  NetworksMap::const_iterator iter = vReseau.find( n );
	  return iter == vReseau.end() ? NULL : iter->second;
        }



      cVelo* cEnvironnement::getVelo( size_t n ) const
        {
	  BikeCompliancesMap::const_iterator iter = vVelo.find( n );
	  return iter == vVelo.end() ? NULL : iter->second;
        }

      cHandicape* cEnvironnement::getHandicape( size_t n ) const
        {
	  HandicappedCompliancesMap::const_iterator iter = vHandicape.find( n );
	  return iter == vHandicape.end() ? NULL : iter->second;
        }


      cTarif* cEnvironnement::getTarif( size_t n ) const
        {
	  FaresMap::const_iterator iter = vTarif.find( n );
	  return iter == vTarif.end() ? NULL : iter->second;
        }


      const synthese::time::Date& cEnvironnement::DateMinReelle() const
        {
	  return ( vDateMin );
        }

      const synthese::time::Date& cEnvironnement::DateMaxReelle() const
        {
	  return ( vDateMax );
        }



      /*! \brief Modificateur de la premi�re date o� circule au moins un service de l'environnement
	\param newDate Date de circulation d'un service � prendre en compte
	\author Hugues Romain
	\date 2005
         
        Cette m�thode met � jour la premi�re date o� circule au moins un service de l'environnement, si la date fournie est ant�rieure � la premi�re date connue.
      */
      void cEnvironnement::SetDateMinReelle( const synthese::time::Date& newDate )
        {
	  if ( newDate < vDateMin )
	    vDateMin = newDate;
        }



      /*! \brief Modificateur de la derni�re date o� circule au moins un service de l'environnement
	\param newDate Date de circulation d'un service � prendre en compte
	\author Hugues Romain
	\date 2005
         
        Cette m�thode met � jour la derni�re date o� circule au moins un service de l'environnement, si la date fournie est post�rieure � la derni�re date connue.
      */
      void cEnvironnement::SetDateMaxReelle( const synthese::time::Date& newDate )
        {
	  if ( newDate > vDateMax )
	    vDateMax = newDate;
        }




      /*! \brief Accesseur Index
	\return L'index de l'environnement dans la base SYNTHESE
	\author Hugues Romain
	\date 2005
      */
      const size_t& cEnvironnement::Index() const
        {
	  return Code;
        }


      void cEnvironnement::SetDatesService( int __AnneeMin, int __AnneeMax )
        {
	  vPremiereAnnee = __AnneeMin;
	  vDerniereAnnee = __AnneeMax;
        }


      bool cEnvironnement::ControleDate( const synthese::time::Date& __Date ) const
        {
	  return vDateMin <= __Date && __Date <= vDateMax;
        }

      
