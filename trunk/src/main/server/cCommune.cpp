/*!	\file cCommune.cpp
	\brief Impl�mentation classe Commune
*/

#include "cCommune.h"



/*!	\brief Constructeur
	\param newNom Nom de la commune
	\param newNumero Num�ro de la commune
	\author Hugues Romain
	\date 2001-2005
*/
cCommune::cCommune(const cTexte& newNom)
{
	vNom << newNom;
	_AccesPADeToutLieu = NULL;
}



/*! \brief Recherche des points d'arret en fonction d'une Entree.
	\param n nombre de points d'arret en sortie en cas de doute non control�
	\param Entree Texte sur lequel fonder la recherche
	\author Hugues Romain
	\date 2003
	\warning Le tableau retourn� doit �tre d�sallou� apr�s utilisation.
*/
cAccesPADe** cCommune::textToPADe(const cTexte& Entree, size_t n) const
{
	// Variables
	cAccesPADe** tbAccesPADe = (cAccesPADe**) calloc(n+2, sizeof(cAccesPADe*));
	tIndex iIndexPADe;
	
	cTexteMinuscules EntreeFiltree;
	EntreeFiltree << Entree;
	
	if (n)
	{
		size_t nbAccesPADe = 0;
		size_t j;
		
		tIndex LongueurComparaison = EntreeFiltree.Taille();
		while (true) 
		{
			for (iIndexPADe = 1; _AccesPADe.IndexValide(iIndexPADe) != NULL; iIndexPADe++)
			{
				if (!LongueurComparaison || EntreeFiltree.Compare(_AccesPADe[iIndexPADe]->getNom(), LongueurComparaison))
				{
					if (nbAccesPADe == n)
						break;

					// Pour eviter de retrouver une commune deja trouvee avec LongueurComparaison superieur
					for (j=1; j <= nbAccesPADe; j++)
						if (tbAccesPADe[j] == _AccesPADe[iIndexPADe])
							break;
					
					if (j > nbAccesPADe)
					{
						tbAccesPADe[nbAccesPADe + 1] = _AccesPADe[iIndexPADe];
						nbAccesPADe++;
						if (!tbAccesPADe[0] && EntreeFiltree.Taille() == _AccesPADe[iIndexPADe]->getNom().Taille())
							tbAccesPADe[0] = tbAccesPADe[1];
					}

				}
			}
			
			if (!tbAccesPADe[0] && nbAccesPADe == 1)
				tbAccesPADe[0] = tbAccesPADe[1];

			// Sortie si n atteint
			if (nbAccesPADe == n || !LongueurComparaison)
				break;

			LongueurComparaison--;
		}
	}
	else
	{
		for (iIndexPADe = 1; _AccesPADe[iIndexPADe] != NULL; iIndexPADe++)
			if (EntreeFiltree.Compare(_AccesPADe[iIndexPADe]->getNom()))
				break;
		if (_AccesPADe[iIndexPADe] != NULL)
			tbAccesPADe[0] = _AccesPADe[iIndexPADe];
	}

	return tbAccesPADe;
}



/*
// cCommune 2.0 - CreeDescriptionOD 1
// ____________________________________________________________________________
//
// Construit une Description OD � partir du point d'arr�t d�sign� en AccesPADe
// Ajoute syst�matiquement le point d'arret "tout lieu" s'il existe.
// ____________________________________________________________________________ 
cDescriptionOD* cCommune::CreeDescriptionOD(cAccesPADe* AccesPADe)
{
	if (ToutLieu == NULL)
		cDescriptionOD* curDescriptionOD = new cDescriptionOD(1);
	else
	{
		cDescriptionOD* curDescriptionOD = new cDescriptionOD(2);
		curDescriptionOD->Remplit(1, ToutLieu);
	}
	curDescriptionOD->Remplit(0, AccesPADe);
	return(curDescriptionOD);
}
// � Hugues Romain 2002
// ____________________________________________________________________________ 




// cCommune 2.0 - CreeDescriptionOD 2
// ____________________________________________________________________________
//
// Construit une Description OD � partir du point d'arr�t d�sign� en ArretLogique
// Ajoute syst�matiquement le point d'arret "tout lieu" s'il existe.
// ____________________________________________________________________________ 
cDescriptionOD* cCommune::CreeDescriptionOD(cArretLogique* ArretLogique)
{
	if (ToutLieu == NULL)
		cDescriptionOD* curDescriptionOD = new cDescriptionOD(1);
	else
	{
		cDescriptionOD* curDescriptionOD = new cDescriptionOD(2);
		curDescriptionOD->Remplit(1, ToutLieu->ArretLogique);
	}
	curDescriptionOD->Remplit(0, AccesPADe);
	return(curDescriptionOD);
}
// � Hugues Romain 2002
// ____________________________________________________________________________ 
*/



/*!	\brief Enregistrement d'une d�signation de point d'arr�t dans la commune
	\param Objet La d�signation
	\return true si l'enregistrement a �t� effectu� avec succ�s
	\author Hugues Romain
	\date 2000-2005
	\todo Finaliser la d�finition de lieux publics

Pour plus d'informations sur les concepts, voir cAccesPADe

Lors de l'int�gration d'une d�signations � la commune, les op�rations suivantes sont r�alis�es :
 - Si tout lieu existe et d�signation principale, mise � jour de la d�signation de l'arr�t par chainage du tout lieu en fin de liste
 - Insertion de la d�signation dans le tableau ordonn� des d�signations
 - Si d�signation de type principale, ajout d'une copie de la d�signation � la d�signation principale de la commune
 - Si d�signation de type tout lieu, mise � jour du pointeur vers tout lieu, ainsi que de toutes les d�signations existances de point d'arr�ts r�ellement situ�s sur la commune
 
\warning On ne peut lier qu'un seul arr�t tout lieu par commune. Toute tentative nouvelle sera ignor�e.
*/
bool cCommune::addDesignation(cAccesPADe* newAccesPADe, tTypeAccesPADe TypeAcces)
{
	// Locales
	tIndex iIndexDesignation, jIndexDesignation;
	
	// Tout lieu � chainer
	if (_AccesPADeToutLieu != NULL && newAccesPADe->numeroDesignation() == 0)
		newAccesPADe->Chaine(_AccesPADeToutLieu);
		
	// Placement de la d�signation dans l'ordre alphab�tique
	for (iIndexDesignation = 1; iIndexDesignation < _AccesPADe.Taille(); iIndexDesignation++)
	{
		cTexteMinuscules txt1;
		txt1 << _AccesPADe[iIndexDesignation]->getNom();
		cTexteMinuscules txt2;
		txt2 << newAccesPADe->getNom();
		if (strcmp(txt1.Texte(), txt2.Texte()) > 0)
			break;
	}
	
	// D�calage des d�signations suivantes dans l'ordre alpha
	for (jIndexDesignation = _AccesPADe.Taille(); jIndexDesignation > iIndexDesignation; jIndexDesignation--)
		_AccesPADe.CopieElement(jIndexDesignation - 1, jIndexDesignation);
	
	_AccesPADe.SetElement(newAccesPADe, iIndexDesignation);

	// Contr�le des allocations
	if (!_AccesPADe.Taille())
		return false;
	
	// Cas type princpal
	if (TypeAcces == ePrincipale)
	{
		cAccesPADe* curPADePrincipale = new cAccesPADe(newAccesPADe);
		curPADePrincipale->setSuivant(_AccesPADe[0]);
		_AccesPADe.SetElement(curPADePrincipale, 0);
	}
	
	// Cas type tout lieu
	if (TypeAcces == eToutLieu)
	{
		if (_AccesPADeToutLieu != NULL)
			return false;
		else
		{
			_AccesPADeToutLieu = newAccesPADe;
			for (jIndexDesignation = 1; _AccesPADe[jIndexDesignation] != NULL; jIndexDesignation++)
				if (_AccesPADe[jIndexDesignation]->numeroDesignation() == 0)
					_AccesPADe[jIndexDesignation]->Chaine(newAccesPADe);
		}
	}
	
	// Sortie
	return true;
}

