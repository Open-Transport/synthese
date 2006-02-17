#include "cTableauAffichage.h"

cTableauAffichage::cTableauAffichage(const cTexte& __Code)
: cSite(__Code)
{
	_NombreDeparts		= INCONNU;	// Illimit�
	_ArretLogique			= NULL;
	_OriginesSeulement	= false;
}

cTableauAffichage::~cTableauAffichage(void)
{
}


bool cTableauAffichage::SetOriginesSeulement(bool __Valeur)
{
	_OriginesSeulement = __Valeur;
	return true;
}

// Modificateur du point d'arr�t
bool cTableauAffichage::SetArretLogique(const LogicalPlace* __ArretLogique)
{
	return (_ArretLogique = __ArretLogique) != NULL;
}

// Modificateur nombre de d�parts affich�s
bool cTableauAffichage::SetNombreDeparts(tIndex __NombreDeparts)
{
	return (_NombreDeparts = __NombreDeparts) > 0;
}

// Ajout de ligne ne devant pas �tre affich�e sur le tableau
bool cTableauAffichage::AddLigneInterdte(const cLigne* __Ligne)
{
	return _LignesInterdites.SetElement(__Ligne) != INCONNU;
}

// Ajout de quai � afficher. Si aucun quai ajout� alors tous les quais du point d'arr�t sont affich�s
bool cTableauAffichage::AddArretPhysiqueAutorise(tIndex __NumeroArretPhysique)
{
	return	_ArretLogique && _ArretLogique->GetArretPhysique(__NumeroArretPhysique) 
		?	_ArretPhysiques.SetElement(_ArretLogique->GetArretPhysique(__NumeroArretPhysique)) != INCONNU 
		:	false;
}

// Accesseur Environnement
cDescriptionPassage* cTableauAffichage::Calcule(const cMoment& __MomentDebut) const
{
	// Variables
	cMoment __MomentFin = _MomentFin(__MomentDebut);
	cGareLigne* curGLD;
	tNumeroService iNumeroService;
	cDescriptionPassage* firstDP=NULL;
	cDescriptionPassage* newDP;
	cMoment tempMomentDepart;
	tIndex nDP = 0;

	// Parcours sur toutes les lignes au d�part et sur tous les services
	for (curGLD = _ArretLogique->PremiereGareLigneDep(); curGLD!= NULL; curGLD=curGLD->PADepartSuivant())
		if (_LigneAutorisee(curGLD)
	){
		// Parcours des services de la GLD
		tempMomentDepart = __MomentDebut;
		iNumeroService=-2;
		while ((iNumeroService=curGLD->Prochain(tempMomentDepart, __MomentFin, __MomentDebut, iNumeroService+1))!=INCONNU)
		{
			// 1: Cas tableau pas rempli
			if (_NombreDeparts == INCONNU || nDP < _NombreDeparts)
			{
				newDP = new cDescriptionPassage;
				nDP++;
			}
			else // 2: Cas du tableau rempli: r�cup�ration du dernier
				newDP = firstDP->GetDernierEtLibere();
			
			// 3: Ecriture des donn�es du DP
			newDP->Remplit(curGLD, tempMomentDepart, iNumeroService);

			// 4: Chainage du DP � la bonne position
			if (nDP==1)
				firstDP = newDP;
			else
				firstDP = firstDP->Insere(newDP);

			// 5: Si d�passement du nombre de DP voulus
			if (nDP == _NombreDeparts)
				__MomentFin = firstDP->MomentFin();
		}
	}
	ListeArretsAffiches(firstDP);
	return firstDP;
}

// Calcul du moment de fin d'affichage en fonction des attributs
cMoment cTableauAffichage::_MomentFin(const cMoment& __MomentDebut) const
{
	cMoment __MomentFin = __MomentDebut;
	if (_HeureBascule.EstInconnu())
		__MomentFin.addDureeEnJours();
	else
	{
		if (__MomentFin.getHeure() >= _HeureBascule)
			__MomentFin.addDureeEnJours();
		__MomentFin.setHeure(_HeureBascule);
	}

	return	__MomentFin;
}

/*!	\brief Autorisation d'afficher une ligne sur le tableau de d�parts
	\param __GareLigne GareLigne � tester

	Les tests effectu�s sont les suivants. Ils doivent tous �tre positifs :
		- Ligne non explicitement interdite d'affichage sur tout tableau de d�parts
		- Ligne non explicitement interdite d'affichage sur le tableau de d�part courant
		- Ligne au d�part d'un quai autoris� sur le tableau de d�part courant
		- Si tableau des origines seulement, ligne originaire de l'arr�t affich� sur le tableau courant
		- L'arr�t d'arriv�e suivant ou la destination doivent �tre diff�rent de l'arr�t courant
*/
bool cTableauAffichage::_LigneAutorisee(const cGareLigne* __GareLigne) const
{
	return 	__GareLigne->Ligne()->AAfficherSurTableauDeparts()
			&&	_LignesInterdites.Recherche(__GareLigne->Ligne()) == INCONNU
			&&	(!_ArretPhysiques.Taille() || _ArretPhysiques.Recherche(__GareLigne->getVoie()) != INCONNU)
			&& (!_OriginesSeulement || !__GareLigne->getDepartPrecedent())
			&& (__GareLigne->getArriveeSuivante()->ArretLogique() != _ArretLogique 
                || __GareLigne->Destination()->ArretLogique() != _ArretLogique)
	;
}


/*!	\brief Fabrication de la liste des points d'arr�t affich�s
*/
void cTableauAffichage::ListeArretsAffiches(cDescriptionPassage* __DP) const
{
	for (; __DP; __DP = __DP->Suivant())
	{
		for (cGareLigne* __GL = __DP->getGareLigne(); __GL; __GL=__GL->getArriveeSuivante())
		{
			if ((	_DestinationsAffichees.Recherche(__GL->ArretLogique()) != INCONNU 
					|| __GL->getArriveeSuivante()==NULL
					|| __GL == __DP->getGareLigne()
				) &&	__DP->_Gare.Recherche(__GL->ArretLogique()) == INCONNU
			){
				__DP->_Gare.SetElement(__GL->ArretLogique());
			}
		}
	}
}

bool cTableauAffichage::SetTitre(const cTexte&__Titre)
{
	_Titre = __Titre;
	return _Titre.Compare(__Titre);
}

tIndex cTableauAffichage::AddDestinationAffichee(const LogicalPlace* __ArretLogique)
{
	return _DestinationsAffichees.SetElement(__ArretLogique);
}

const cTexte& cTableauAffichage::getTitre() const
{
	return _Titre;
}



/** Modificateur num�ro du panneau.
	@param __NumeroPanneau Num�ro du panneau devant afficher les r�sultats
*/
void cTableauAffichage::SetNumeroPanneau(tIndex __NumeroPanneau)
{
	_NumeroPanneau = __NumeroPanneau;
}



/** Accesseur num�ro du panneau.
	@return Num�ro du panneau
*/
tIndex cTableauAffichage::getNumeroPanneau() const
{
	return _NumeroPanneau;
}
