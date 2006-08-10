/*!	\file cTexte.cpp
	\brief Impl�mentation classes de gestion avanc�e du texte
	\author Hugues Romain
	\date 2000-2003
*/

#include "cTexte.h"

#ifdef DMALLOC
	#include "dmalloc.h"
#endif



cTexte::cTexte(tIndex __TailleMax, bool __AugmentationParDoublement) : cTableauDynamique<char>(__TailleMax, __AugmentationParDoublement)
{ }
	


/*!	\brief Constructeur par copie d'une cha�ne de caract�res classique
	\param Chaine Tableau de carac�tres termin� par NULL (format classique langage C)
*/
cTexte::cTexte(const char* Chaine) : cTableauDynamique<char>()
{
	Copie(Chaine);
}


/*!	\brief Constructeur par copie d'une cha�ne de caract�res
	\param Chaine Chaine de caract�re
*/
cTexte::cTexte(const cTexte& Chaine) : cTableauDynamique<char>()
{
	Copie(Chaine);
}



/*!	\brief Constructeur par copie d'une cha�ne de caract�res String
	\param Chaine Chaine de caract�re String (format classique langage C++)
*/
cTexte::cTexte(string Chaine) : cTableauDynamique<char>()
{
	Copie(Chaine.c_str());
}



/*!	\brief Destructeur
*/
cTexte::~cTexte()
{
}




/*!
\deprecated Finir de tout mettre dans cFichier pour supprimer cette m�thode
*/
bool cTexte::ProchaineSection(ifstream& __Fichier, TypeSection TS)
{
	TypeSection tempTS = TypeSectionTampon();
	if (tempTS == TYPEVide)
		tempTS = LireLigne(__Fichier);

	while (1)
	{
		if (tempTS == TS)
			return true;
		if (tempTS == TYPEVide)
			return false;
		tempTS = LireLigne(__Fichier);
	}
}

/*!
\deprecated Finir de tout mettre dans cFichier pour supprimer cette m�thode
*/
bool cTexte::RechercheSection(ifstream& __Fichier, const cTexte& Intitule, char CarEntreeSection)
{
	while (ProchaineSection(__Fichier, CarEntreeSection))
	{
		if (Compare(Intitule, 0, 1))
			return true;
		Vide();
	}
	return false;
}


/*!	\todo Verifier le fonctionnement de cette fonction
	\deprecated Finir de tout mettre dans cFichier pour supprimer cette m�thode
*/
TypeSection cTexte::LireLigne(ifstream& __Fichier)
{
	int Car;			// R�ceptionne la valeur de get.
	TypeSection TS = TYPEVide;	// Pas encore de ligne utile rencontr�e.
	tIndex vPosition = 0;
	
	Vide();
	setTampon();
	
	while (1)
	{
		// Lecture du premier caract�re de la ligne
		Car = __Fichier.get();

		// Lecture du premier caract�re - Type de section
		switch (Car)
		{
		case -1:
			if (!Taille())
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

		// D�tection des lignes d'espaces
		while (Car == ' ')
		{
			AlloueSiBesoin(1, true);
			_Element[vPosition] = ' ';
			vPosition++;

			Car = __Fichier.get();

			if ((Car == '\n') || (Car == -1) || (Car =='\r'))
			{
				vPosition = 0;
				break;
			}
		}

		// SET PORTAGE LINUX
		// D�tection des caract�res CONTROL-M
		while (Car == '\r')
		{
			Car = __Fichier.get();
		}
		//END PORTAGE 

		// Lecture de la ligne
		while ((Car != '\n') && (Car != '\r') && (Car != -1))
		{
			if (TS != TYPECommentaire)
			{
				AlloueSiBesoin(1, true);
				_Element[vPosition] = Car;
				vPosition++;
			}
			Car = __Fichier.get();
		}

		// Chaine non nulle
		if (vPosition)
		{
			while (_Element[vPosition-1] == ' ')
				vPosition--;
			Vide(vPosition);
			
			vPosition = 0;
			return(TS);
		}

		// Fin de fichier atteinte sans caract�re stock�
		if (Car == -1)
			return(TYPEVide);
	}
}



cTexte& cTexte::Copie(const char* Source)
{
	if (Source==NULL || Source[0]==0)
		return *this;
		
	tIndex vPosition = Taille();
	_Taille += TailleApresCodage(Source);
	AlloueSiBesoin(_Taille);

	for (tIndex i=0; Source[i]; i++)
		vPosition += ConvertChar(Source[i], _Element+vPosition);

	Vide(vPosition);
	
	Finalise();
	return(*this);
}



cTexte& cTexte::Copie(const char* Source, const tIndex TailleMax)

{
	if (Source==NULL || Source[0]==0 || TailleMax==0)
		return *this;
	
	tIndex vPosition = _Taille;
	_Taille += TailleApresCodage(Source);
	if (_Taille > TailleMax)
		_Taille = TailleMax;
	
	AlloueSiBesoin(_Taille);
	
	for (; *Source!=0; Source++)
	{
		if (vPosition + ConvertChar(*Source) > _Taille)
			break;
		vPosition += ConvertChar(*Source, _Element + vPosition);
	}
	Vide(vPosition);	
	
	Finalise();
	return *this;
}



cTexte& cTexte::Copie(const cTexte& Source)
{
	if (!Source._Taille)
		return *this;
		
	tIndex vPosition = _Taille;
	_Taille += TailleApresCodage(Source._Element);
	AlloueSiBesoin(_Taille);
	
	for (tIndex vvPosition=0; Source._Element[vvPosition]!=0; vvPosition++)
		vPosition += ConvertChar(Source._Element[vvPosition], _Element+vPosition);
	
	Vide(vPosition);
	Finalise();
	return(*this);
}



cTexte& cTexte::Copie(const cTexte& Source, const tIndex LongueurMax)
{
	if (!Source._Taille || LongueurMax==0)
		return *this;
		
	tIndex vPosition = _Taille;
	_Taille += TailleApresCodage(Source._Element);
	if (_Taille > LongueurMax)
		_Taille = LongueurMax;
	AlloueSiBesoin(_Taille);
	
	tIndex Position;
	for (Position=0; Source._Element[Position]!=0; Position++)
	{
		if (vPosition + ConvertChar(Source._Element[Position]) > LongueurMax)
			break;
		vPosition += ConvertChar(Source._Element[Position], _Element +vPosition);
	}
	Vide(vPosition);	
	
	Finalise();
	return(*this);
}


cTexte& cTexte::Copie(int Source, tIndex NombreChiffres)
{
	tIndex vPosition = _Taille;

	if (NombreChiffres == 0)
	{
		// Faire mieux si possible...
		AlloueSiBesoin(11, true);
		sprintf(_Element +vPosition, "%d", Source);
		_Taille = (tIndex) strlen(_Element);
	}
	else
	{
		AlloueSiBesoin(NombreChiffres, true);
		
		int NombreTemp;
		_Element[vPosition+NombreChiffres]=0;
		_Taille = vPosition + NombreChiffres;
		for (; NombreChiffres; NombreChiffres--)
		{
			NombreTemp = Source / 10;
			_Element[vPosition+NombreChiffres-1] = '0' + (Source - NombreTemp * 10);
			Source = NombreTemp;
		}
	}
	
	Finalise();
	return(*this);
}



/*! 	\brief Calcul de la taille d'une cha�ne de caract�res apr�s conversion dans le format courant
	\author Hugues Romain
	\date 2005
*/
tIndex cTexte::TailleApresCodage(const char* Tampon) const
{
	tIndex Resultat = 0;
	for (tIndex i=0; Tampon[i]; i++)
		Resultat+= ConvertChar(Tampon[i], NULL);
	return(Resultat);	
}


cTexte& cTexte::ChangeTexte(char* newTexte, tIndex newTailleMax)
{
	free(_Element);

	_Element = newTexte;
	_Taille = (tIndex) strlen(_Element);

	if (newTailleMax)
		_TailleMax = newTailleMax;
	else
		_TailleMax = _Taille;

	return(*this);
}

cTexte& cTexte::ChangeTexte(cTexte& newTexte)
{
	if (_Element != NULL)
		free(_Element);

	_Element = newTexte._Element;
	_Taille = newTexte._Taille;
	_TailleMax = newTexte._TailleMax;
	newTexte._Element = NULL;

	delete &newTexte;

	return(*this);
}


/*!	\brief Comparaison simple de deux textes selon le format de l'objet
	\param Autre Texte � comparer avec l'objet
	\param NombreChiffres Nombre maximal de caract�res � comparer (l'�ventuelle suite des textes est ignor�e)
	\param Position1 Position du d�but de la comparaison pour l'objet
	\param Position2 Position du d�but de la comparaison pour le texte � comparer
	\return true si les NombreChiffres caract�res situ�s apr�s le Position1-�me sur le texte objet sont �gaux aux NombreChiffres caract�res situ�s apr�s le Position2-�me sur le texte � comparer sont �gaux, apr�s conversion au format de l'objet. false sinon.
*/
bool cTexte::Compare(const cTexte& Autre, tIndex NombreChiffres, tIndex Position1, tIndex Position2) const
{
	if (Position1 >= _Taille && Position2 >= Autre._Taille)
		return(true);

	if (Position1 >= _Taille || Position2 >= Autre._Taille)
		return(false);

	if (NombreChiffres == 0 && _Taille - Position1 != Autre._Taille - Position2)
		return(false);

	for (; Position1!=_Taille && Position2!=Autre._Taille; Position1++)
	{
		if (!CompareChar(_Element[Position1], Autre._Element[Position2]))
			return false;

		if (NombreChiffres)
		{
			NombreChiffres--;
			if (!NombreChiffres)
				return true;
		}
		
		Position2++;
	}
	return true;
}


/** Conversion de texte en nombre entier.
	@param NombreChiffres Nombre maximal de chiffres � lire
	@param Position Position � partir de laquelle lire le texte
	@return Nombre entier lu sur le texte
*/
int cTexte::GetNombre(tIndex NombreChiffres, tIndex Position) const
{
	int Resultat=0;

	// Gestion du signe
	int __Signe = (((Position < _Taille) && _Element[Position] == '-') ? -1 : 1);
	if (__Signe == -1) Position++;
		
	for (; Position< _Taille; Position++)
	{
		if ((_Element[Position] >= '0') && (_Element[Position] <= '9'))
		{ // Nombre
			Resultat = Resultat * 10;
			Resultat = Resultat + _Element[Position] - '0';

			if (NombreChiffres)
			{
				NombreChiffres--;
				if (!NombreChiffres)
					break;
			}
		}
		else
			break;

	}

	return __Signe * Resultat;
}





tIndex cTexte::RechercheOccurenceGauche(char curChar, tIndex NombreOccurences, tIndex Position) const
{
	for (; NombreOccurences && Position< _Taille; Position++)
		if (_Element[Position]==curChar)
			NombreOccurences--;
	return(Position-1);
}



/*!	\todo CRO
*/
cTexte& cTexte::fAdresseComplete(const cTexte& AdresseBase)
{
	int PosDernierSlash=0;
	int Position;
	int TailleMax;
	char* newTexte;

	// Extraction du repertoire de l'adresse de base
	for (Position = 0; Position < AdresseBase.Taille(); Position++)
		if (AdresseBase._Element[Position]=='/' || AdresseBase._Element[Position]=='\\')
			PosDernierSlash = Position;

	// Pas de slash rencontr�: on ajoute un slash en debut
	if (PosDernierSlash==0)
	{
		TailleMax = Taille() + 1;
		newTexte = (char *) malloc((TailleMax+1) * sizeof(char));
		newTexte[0]=SEPARATEUR_REPERTOIRE_CHAR;
	}
	else
	{
		TailleMax = PosDernierSlash + 1 + Taille();
		newTexte = (char *) malloc((TailleMax+1) * sizeof(char));

		for (Position=0; Position<=PosDernierSlash; Position++)
			if (AdresseBase._Element[Position] == '/' || AdresseBase._Element[Position] == '\\')
				newTexte[Position] = SEPARATEUR_REPERTOIRE_CHAR;
			else
				newTexte[Position] = AdresseBase._Element[Position];
	}



	// Copie de la fin du nom du fichier avec correction des slash �ventuelle
	//SET PORTAGE LINUX Gestion du '\r' en fin de ligne
	//for (Position = 0; vTexte[Position] != 0; Position++)
	for (Position = 0; (_Element[Position] != 0) && (_Element[Position] != '\r'); Position++)
	//END PORTAGE
		if (_Element[Position] == '/' || _Element[Position] == '\\')
			newTexte[PosDernierSlash + Position + 1] = SEPARATEUR_REPERTOIRE_CHAR;
		else
			newTexte[PosDernierSlash + Position + 1] = _Element[Position];

	newTexte[PosDernierSlash + Position + 1] = 0;
	ChangeTexte(newTexte, TailleMax);

	return(*this);

}






char* cTexte::Texte() const
{
	if (_Taille)
		return _Element;
	else
		return "";
}



/*!	\brief Type de section d'un Tampon
	\author Hugues Romain 
	\date 2000-2001
*/
TypeSection cTexte::TypeSectionTampon() const
{
	switch (_Element[0])
	{
	case TYPESection:
	case TYPESousSection:
	case TYPELien:
	case TYPECommentaire:
		return((TypeSection) _Element[0]);

	case 0:
		return(TYPEVide);

	default:
		return(TYPEAutre);
	}
}



/*!	\brief Extraction d'une sous cha�ne de caract�res
	\param __Position D�but de l'extraction
	\param __Longueur Longueur de l'extraction (d�faut = totalit�)
	\return Un objet cTexte contenant la cha�ne extraite
	\author Hugues Romain
	\date 2000-2005
	\warning Contr�ler l'allocation effective avec cTexte::GetTaille() pour v�rifier que la m�moire �tait bien disponible.
*/
cTexte cTexte::Extrait(tIndex __Position, int __Longueur) const
{
	// Si la position est apr�s la longueur, retour d'une chaine vide
	if (__Position >= _Taille)
		return cTexte();
	
	// Taille allou�e
	tIndex __LongueurAllouee;
	if (__Longueur == INCONNU || __Longueur + __Position > _Taille)
		__LongueurAllouee = _Taille - __Position;
	else 
		__LongueurAllouee = __Longueur;
	
	// Cr�ation de l'objet
	cTexte __Texte(__LongueurAllouee);
	
	// Copie du contenu
	__Texte.Copie(_Element + __Position, __LongueurAllouee);

	// Sortie	
	return __Texte;
}





cTexte& cTexte::setTampon()
{
	AlloueSiBesoin(TAILLETAMPON);
	return *this;
}

cTexte& cTexte::Repertoire()
{
	int PosDernierSlash=0;
	int i=0;
	char *Pointeur = _Element;

	while (*Pointeur!=0)
	{
		if (*Pointeur=='/' || *Pointeur=='\\')
			PosDernierSlash = i;
		i++;
		Pointeur++;
	}

	if (PosDernierSlash!=0)
		Vide(PosDernierSlash+1);

	return(*this);
}

cTexte& cTexte::Trim()
{
	if (_Taille == 0)
		return *this;
	
	int debut=0;
	int fin= _Taille;
	char *Pointeur = _Element;
	
	// recherche du premier caractere non espace
	while (*Pointeur!=0 && *Pointeur==' ')
	{		
		debut++;
		Pointeur++;
	}

	// premier caract�re trouv�, on cherche le dernier carct�re
	if (*Pointeur!=0)
	{
		Pointeur = _Element + _Taille;
		while (fin>0 && *Pointeur==' ')
		{		
			fin--;
			Pointeur--;
		}

		strncpy(_Element, _Element + debut, _Taille-fin);
	}
	else
		strcpy(_Element,"");
	
	_Taille = (tIndex) strlen(_Element);
	
	//pas compos� uniquement d'espace
	return *this;
}



/*!	\brief Comparaison de deux caract�res
*/
bool cTexte::CompareChar(const char c1, const char c2) const
{
	return (c1 == c2);
}



/*!	\brief Comparaison de deux caract�res
	\param c1 Caract�re du m�me format que l'objet
	\param c2 Caract�re de format quelconque � comparer � c1 une fois converti au m�me format
	\return true si les deux caract�res sont �gaux une fois convertis au format de l'objet
*/
bool cTexteSansAccent::CompareChar(const char c1, const char c2) const
{
	char Tampon[2];
	ConvertChar(c2, Tampon);
	return (c1 == Tampon[0]);
}



/*!	\brief Conversion d'un caract�re standard en caract�re HTML (simulation si pas de cha�ne fournie)
	\param c Caract�re � convertir
	\retval Tampon char* o� �crire le r�sultat (NULL = pas d'�criture)
	\return Longueur de la cha�ne convertie
	\author Hugues Romain
	\date 2005
	\todo Terminer l'impl�mentation des cas les plus courants
*/
tIndex cTexteHTML::ConvertChar(const char c, char* Tampon) const
{
	switch (c)
	{
		case '�':  if (Tampon) strncpy(Tampon,"&agrave;",8); return(8);
		case '�':  if (Tampon) strncpy(Tampon,"&egrave;",8); return(8);
		// REMPLIR ICI
		default:  if (Tampon) *Tampon = c; return(1);
	}
}



/*!	\brief Conversion d'un caract�re standard en caract�re standard (simulation si pas de cha�ne fournie)
	\param c Caract�re � convertir
	\retval Tampon char* o� �crire le r�sultat (NULL = pas d'�criture)
	\return Longueur de la cha�ne convertie
	\author Hugues Romain
	\date 2005
*/
tIndex cTexte::ConvertChar(const char c, char* Tampon) const
{
	if (Tampon) 
		*Tampon = c;
	return(1);
}



/*!	\brief Conversion d'un caract�re standard en caract�re non accentu� (simulation si pas de cha�ne fournie)
	\param c Caract�re � convertir
	\retval Tampon char* o� �crire le r�sultat (NULL = pas d'�criture)
	\return Longueur de la cha�ne convertie
	\author Hugues Romain
	\date 2005
	\todo Terminer l'impl�mentation des cas les plus courants
*/
tIndex cTexteSansAccent::ConvertChar(const char c, char* Tampon) const
{
	if (Tampon)
		switch (c)
		{
		case '�':
		case '�':
		case '�':
		case '�':
			*Tampon = 'e';
			break;
		case '�':
		case '�':
		case '�':
		case '�':
			*Tampon = 'u';
			break;
		case '�':
		case '�':
		case '�':
		case '�':
			*Tampon = 'a';
			break;
		case '�':
		case '�':
		case '�':
		case '�':
			*Tampon = 'i';
			break;
		case '�':
		case '�':
		case '�':
		case '�':
			*Tampon = 'o';
			break;
		case '�':
			*Tampon = 'c';
			break;
		case '(':
		case ')':
			*Tampon = ' ';
			break;
		
		default:
			*Tampon = c;
		}
	return 1;
}


/*!	\brief Conversion d'un caract�re standard en caract�re PostScript (simulation si pas de cha�ne fournie)
	\param c Caract�re � convertir
	\retval Tampon char* o� �crire le r�sultat (NULL = pas d'�criture)
	\return Longueur de la cha�ne convertie
	\author Hugues Romain
	\date 2005
	\todo Faire imprimer les accents plutot que les enlever
*/
tIndex cTextePostScript::ConvertChar(const char c, char* Tampon) const
{
	switch (c)
	{
	case '�':
	case '�':
	case '�':
	case '�':
		if (Tampon) 
			*Tampon = 'e';
		break;
	case '�':
	case '�':
	case '�':
	case '�':
		if (Tampon) 
			*Tampon = 'u';
		break;
	case '�':
	case '�':
	case '�':
	case '�':
		if (Tampon) 
			*Tampon = 'a';
		break;
	case '�':
	case '�':
	case '�':
	case '�':
		if (Tampon) 
			*Tampon = 'i';
		break;
	case '�':
	case '�':
	case '�':
	case '�':
		if (Tampon) 
			*Tampon = 'o';
		break;
	case '�':
		if (Tampon) 
			*Tampon = 'c';
		break;
	case '(':
	case ')':
		if (Tampon) 
			*Tampon = ' ';
		break;
	
	default:
		if (Tampon) 
			*Tampon = c;
	}
	return 1;
}



/*!	\brief Conversion d'un caract�re standard en caract�re en minuscules (simulation si pas de cha�ne fournie)
	\param c Caract�re � convertir
	\retval Tampon char* o� �crire le r�sultat (NULL = pas d'�criture)
	\return Longueur de la cha�ne convertie
	\author Hugues Romain
	\date 2005

La fonction de conversion agit comme suit :
 - Un caract�res en majuscules est converti en minuscules
 - Un caract�re en minuscules ou num�rique ou un espace est conserv� en l'�tat
 - Tout autre caract�re est converti en espace
*/
tIndex cTexteMinuscules::ConvertChar(const char c, char* Tampon) const
{
	if (Tampon)
	{
		cTexteSansAccent::ConvertChar(c, Tampon);
		if (c >= 'A' && c <= 'Z')
			*Tampon = c - 'A' + 'a';
		else if (c >= '0' && c <= '9' || c >= 'a' && c <= 'z')
			*Tampon = c;
		else
			*Tampon = ' ';
	}
	return 1;
}



/*!	\brief Constructeur
	\author Hugues Romain
	\date 2005
*/
cTexteRecherche::cTexteRecherche() : cTexte()
{
	_Mots = NULL;
	_NombreMots = 0;
}



/*!	\brief Destructeur
	\author Hugues Romain
	\date 2005
*/
cTexteRecherche::~cTexteRecherche()
{
	delete _Mots;
}


/*!	\brief Traitement et indexation des mots contenu dans la cha�ne de recherche
	\author Hugues Romain
	\date 2005

Le fonctionnement du traitement des mots est le suivant :
 - st est remplac� par saint

Un mot est une suite de caract�res alphanum�riques. Les mots sont s�par�s par un ou plusieurs espaces.
L'indexation consiste � placer dans l'ordre chaque mot une fois trait�s dans le tableau _Mot. 
Le dernier �l�ment est suivi d'un enregistrement NULL pour permettre la d�tection de la fin de la liste.

\todo Mettre la liste des traitements dans un fichier
*/
void cTexteRecherche::Finalise()
{
	// Compteur de mots : � chaque apparition d'alphanum�rique � la suite d'un ou plusieurs espaces, un mot est compt�
	bool Separateur = true;
	_NombreMots = 0;
	tIndex vPosition;

	// Finalisation standard de chaine de caract�re
	cTexte::Finalise();
			
	for (vPosition = 0; vPosition < _Taille; vPosition++)
	{
		if (Separateur && _Element[vPosition] != ' ')
		{
			Separateur = false;
			_NombreMots++;
		}
		if (!Separateur && _Element[vPosition] == ' ')
		{
			Separateur = true;
		}
	}
	
	// Allocation des mots
	delete[] _Mots;
	_Mots = new cTexteMinuscules[_NombreMots];
	
	// Ecriture des mots
	tIndex iMot = 0;
	int vPositionDebutMot;
	cTexteMinuscules Tampon;
	Tampon.setTampon();
	for (vPosition = 0; vPosition < _Taille; vPosition++)
	{
		if (_Element[vPosition] != ' ')
		{
			vPositionDebutMot = vPosition;
			for (; vPosition < _Taille && _Element[vPosition] != ' '; vPosition++)
			{}
			Tampon.Vide();
			Tampon.Copie(_Element+vPositionDebutMot, vPosition-vPositionDebutMot+1);
			
			// Traitement du mot et �criture
			if (Tampon.Compare("st"))
				_Mots[iMot] << "saint";
			else if (Tampon.Compare("ste"))
				_Mots[iMot] << "sainte";
			else
				_Mots[iMot] << Tampon;
			iMot++;
		}
	}
}



/*!	\brief Op�rateur d'affectation
	\param Op2 Cha�ne de caract�res � copier
	\return L'objet modifi�
	\author Hugues Romain
	\date 2001-2005
*/
cTexte& cTexte::operator=(const cTexte& Op2)
{
	Vide();
	return Copie(Op2);
}



/*!	\brief Op�rateur de comparaison entre deux textes
	\param __Objet l'objet � comparer
	\return true si les deux textes sont strictement identiques
	\author Hugues Romain
	\date 2005
*/
int cTexte::operator==(const char* __Objet) const
{
	return !strcmp(_Element, __Objet);
}
