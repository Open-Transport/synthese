#define SYNTHESE_CFICHIER_CPP


#include "cFichier.h"





/*!	\brief Constructeur
  \param CheminFichier Chemin d'accès au fichier
*/
cFichier::cFichier(const cTexte& CheminFichier, const cFormatFichier* FormatFichier)
{
    _Chemin << CheminFichier;
    _Format = FormatFichier;
}


/*!	\brief Destructeur
  \author Hugues Romain
  \date 2005
*/
cFichier::~ cFichier()
{
    Fermer();
    delete _Format;
}


/*!	\todo Verifier le fonctionnement de cette fonction
 */
TypeSection cFichier::LireLigne(cTexte& Tampon)
{
    int Car;			// Réceptionne la valeur de get.
    TypeSection TS = TYPEVide;	// Pas encore de ligne utile rencontrée.
    tIndex vPosition = 0;

    Tampon.Vide();
    Tampon.setTampon();
	
    while (1)
    {
	// Lecture du premier caractère de la ligne
	Car = _Fichier.get();

	// Lecture du premier caractère - Type de section
	switch (Car)
	{
	case -1:
	    if (!Tampon.Taille())
		return(TYPEVide);
	    else
		return(TS);

	case TYPESection:
	case TYPESousSection:
	case TYPELien:
	case TYPECommentaire:
	    TS = Car;
	    break;
	default:
	    TS = TYPEAutre;
	}

	// Détection des lignes d'espaces
	while (Car == ' ')
	{
	    Tampon.SetElement(' ', vPosition);
	    vPosition++;

	    Car = _Fichier.get();

	    if ((Car == '\n') || (Car == -1) || (Car =='\r'))
	    {
		vPosition = 0;
		break;
	    }
	}

	// Détection des caractères CONTROL-M
	while (Car == '\r')
	{
	    Car = _Fichier.get();
	}

	// Lecture de la ligne
	while ((Car != '\n') && (Car != '\r') && (Car != -1))
	{
	    if (TS != TYPECommentaire)
	    {
		Tampon.SetElement(Car, vPosition);
		vPosition++;
	    }
	    Car = _Fichier.get();
	}

	// Chaine non nulle
	if (vPosition)
	{
	    while (Tampon[vPosition-1] == ' ')
		vPosition--;
	    Tampon.Vide(vPosition);
			
	    vPosition = 0;
	    return(TS);
	}

	// Fin de fichier atteinte sans caractère stocké
	if (Car == -1)
	    return(TYPEVide);
    }
}



/*!	\brief Parcourt le fichier pour obtenir le numéro maximal d'élément
  \return Le plus grand numéro d'objet si applicable au format. Il s'agit du plus grand numéro présent sous la forme [xxxx
  \param Position position du numéro dans le label (défaut = 1 : juste après le [. Si Position = INCONNU (-1) alors on ne fait que compter les objets)
*/
int cFichier::NumeroMaxElement(int Position)
{
    int NumeroMax = 0;
    int curNumero;
    TypeSection TS;
    cTexte Tampon;

    ifstream __Fichier;
    __Fichier.open(_Chemin.Texte());
	
//	if (!Ouvrir())
//		return false;
		
    while (true)
    {
	TS = Tampon.LireLigne(__Fichier);
	if (TS == TYPEVide)
	    break;
		
	if (TS == TYPESousSection)
	{
	    if (Position != INCONNU)
	    {
		curNumero = Tampon.GetNombre(6, Position);
		if (curNumero > NumeroMax)
		    NumeroMax = curNumero;
	    }
	    else
		NumeroMax++;
	}
    }	
    __Fichier.close();
    return NumeroMax;
}


bool cFichier::ProchaineSection(cTexte& Tampon, TypeSection TS)
{
    TypeSection tempTS = Tampon.TypeSectionTampon();
    if (tempTS == TYPEVide)
	tempTS = LireLigne(Tampon);

    while (1)
    {
	if (tempTS == TS)
	    return true;
	if (tempTS == TYPEVide)
	    return false;
	tempTS = LireLigne(Tampon);
    }
}



bool cFichier::RechercheSection(cTexte& Tampon, const cTexte& Intitule, char CarEntreeSection)
{
    while (ProchaineSection(Tampon, CarEntreeSection))
    {
	if (Tampon.Compare(Intitule, 0, 1))
	    return true;
	Tampon.Vide();
    }
    return false;
}


/*!	\brief Ouverture du fichier si besoin et positionnement au début
 */
bool cFichier::Ouvrir()
{
    if (_Fichier.is_open())
	_Fichier.seekg(0, ios::beg);
    else
	_Fichier.open(_Chemin.Texte());
    return _Fichier.is_open();
}

void cFichier::Fermer()
{
    if (_Fichier.is_open())
	_Fichier.close();
}









/*!	\brief Lecture d'une ligne du fichier et renvoi de l'identificateur du champ
 */
int cFichier::LireLigneFormat(cTexte& Texte)
{
    return _Format->LireFichier(_Fichier, Texte);
}




tIndex cFichier::NombreElementsAAllouer()
{
    return NumeroMaxElement() + 1;
}

