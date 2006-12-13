
#include "cInterface_Objet_Element_Parametre_DonneeEnvironnement.h"
#include "cInterface_Objet_AEvaluer_PageEcran.h"
#include "cObjetInterface.h"

#include "01_util/Conversion.h"


/*! \brief Constructeur
 \author Hugues Romain
 \date 2005
 
Initialise les pointeurs � null en attendant de recevoir les donn�es.
*/
cInterface_Objet::cInterface_Objet()
{
    _PremierElement = NULL;
}



/*! \brief Destructeur
 \author Hugues Romain
 \date 2005
 
Supprime les objets li�s.
 
La suppression du premier �l�ment entra�ne automatiquement la suppression de l'�l�ment suivant.
*/
cInterface_Objet::~cInterface_Objet()
{
    // Suppression de la liste d'�l�ments
    delete _PremierElement;
}



/*! \brief Constitution des �l�ments en fonction d'une ligne de commande � interpr�ter
 \param __Parametres Objet-Parametres de l'objet de niveau sup�rieur
 \param __Texte Cha�ne de caract�res � interpr�ter pour g�n�rer la cha�ne d'�l�ments
 \return true si l'interpr�tation a �t� men�e avec succ�s, false sinon
 \author Hugues Romain
 \date 2005
 
La cha�ne cr��e est une suite d'objets dont la description est s�par�e par des espaces dans le texte. Les commandes suivantes sont disponibles :
 - Pxx Copie du xx�me param�tre de l'objet sup�rieur (Element statique ou Element dynamique)
 - Axx Appel au xx�me texte dynamique (Element dynamique)
 - {...} Copie directe du texte contenu entre les accolades (Element de type connu)
*/
bool cInterface_Objet_AEvaluer_ListeParametres::InterpreteTexte( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres, const std::string& __Texte )
{
    // Locales
    int __PositionDebut;

    // Parcours de la cha�ne de caract�res en entier
    for ( int __Position = 0; __Position < __Texte.size (); __Position++ )
    {
        switch ( __Texte[ __Position ] )
        {
            case 'P':
                for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
                        __Texte[ __Position ]
                        && __Texte[ __Position ] != ' '
                        && __Texte[ __Position ] != 'P'
                        && __Texte[ __Position ] != 'A'
                        && __Texte[ __Position ] != '{';
                        __Position++
                    )
                { }

                // Chainage
                if ( __Parametres.EstVide() )
                {
                    if ( !_Chaine( new cInterface_Objet_Element_Parametre_TexteAEvaluer( 
				       synthese::util::Conversion::ToInt (__Texte.substr (__PositionDebut, __Position - __PositionDebut)) 
				       ) ) )
                        return false;
                }
                else
                {
                    if ( !_Chaine( __Parametres[ 
				       synthese::util::Conversion::ToInt (__Texte.substr (__PositionDebut,  __Position - __PositionDebut)) ] ->Copie() ) )
			 return false;
                }
                break;

            case 'A':
                for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
                        __Texte[ __Position ]
                        && __Texte[ __Position ] != ' '
                        && __Texte[ __Position ] != 'P'
                        && __Texte[ __Position ] != 'A'
                        && __Texte[ __Position ] != '{';
                        __Position++
                    )
                { }

                // Chainage
                if ( !_Chaine( new cInterface_Objet_Element_Parametre_DonneeEnvironnement( 
				   synthese::util::Conversion::ToInt (__Texte.substr( __PositionDebut, __Position - __PositionDebut ))
				   ) ) )
                    return false;
                break;

            case '{':
                // D�but du champ texte
                __PositionDebut = ++__Position;

		__Position = __Texte.find ("}", __Position);

                if ( !_Chaine( new cInterface_Objet_Element_Parametre_TexteConnu( __Texte.substr( __PositionDebut, __Position - __PositionDebut ) ) ) )
                    return false;
                break;
        }
    }

    // Sortie succ�s
    return true;
}


/*! \brief Ajout d'�l�ment d'interface en fin de cha�ne
 \author Hugues Romain
 \date 2005
 \param __Objet Pointeur vers l'�lement d'interface � cha�ner
 \return Le pointeur vers l'�l�ment d'interface cha�n� : une valeur NULL indique qu'aucun �l�ment n'a �t� pass� en param�tre, ce qui d�coule dans la majorit� des cas d'utilisation d'une erreur d'allocation lors de la cr�ation de l'objet destin� � �tre cha�n�.
 
Cette m�thode cha�ne l'objet d'interface fourni en fin de liste.
 
\warning L'objet fourni sera supprim� � la destruction de l'objet d'interface. Ne pas donner en argument d'objet qui doit �tre partag� (effectuer une copie pr�alable)
*/
cInterface_Objet_Element* cInterface_Objet::_Chaine( cInterface_Objet_Element* __Objet )
{
    if ( __Objet != NULL )
    {
        // Cha�nage
        if ( _PremierElement == NULL )
            _PremierElement = __Objet;
        else
        {
            // Dernier �l�ment
            cInterface_Objet_Element* __Dernier;

            for ( __Dernier = _PremierElement; __Dernier->Suivant() != NULL; __Dernier = __Dernier->Suivant() )
            { }
            __Dernier->setSuivant( __Objet );
        }
    }
    return __Objet;
}



/*! \brief (A utiliser � l'�x�cution) Evaluation de l'objet pour diffusion du r�sultat dans un flux de sortie
 \author Hugues Romain
 \date 2005
 \param __Flux Flux sur lequel �crire le r�sultat
 \param __Parametres Param�tres statiques � fournir aux �l�ments d'interface
 \param __Objet Pointeur vers un objet de donn�es ou de r�sultat de calcul � fournir aux �l�ments d'interface qui le n�cessitent pour l'�valuation (NULL par d�faut)
 \param __Site Pointeur vers le site d'affichage ayant g�n�r� l'�valuation (NULL par d�faut)
 
L'affichage s'interrompt si un �l�ment renvoie false (commande break)
*/
void cInterface_Objet::Evalue( std::ostream& __Flux, const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* __Objet, const cSite* __Site ) const
{
    int __LigneAAtteindre = INCONNU;

    // Parcours de tous les �l�ments � afficher
    for ( const cInterface_Objet_Element * __Element = _PremierElement; __Element != NULL; __Element = __Element->getSuivant() )
    {
        if ( __LigneAAtteindre == INCONNU || __LigneAAtteindre == __Element->NumeroLigne() )
        {
            __LigneAAtteindre = __Element->Evalue( __Flux, __Parametres, __Objet, __Site );

            if ( __LigneAAtteindre == 0 )
                break;
        }
    }
}





/*! \brief Ajout d'un �l�ment texte en fin de cha�ne
 \warning La r�ussite de la copie n'est pas contr�l�e.
*/
cInterface_Objet_Connu_ListeParametres& cInterface_Objet_Connu_ListeParametres::operator<<( const std::string& __Texte )
{
    _Chaine( new cInterface_Objet_Element_Parametre_TexteConnu( __Texte ) );
    return *this;
}






/*! \brief Ajout d'un �l�ment num�rique en fin de cha�ne
 \warning La r�ussite de la copie n'est pas contr�l�e.
*/
cInterface_Objet_Connu_ListeParametres& cInterface_Objet_Connu_ListeParametres::operator<<( int __Nombre )
{
    _Chaine( new cInterface_Objet_Element_Parametre_TexteConnu( __Nombre ) );
    return *this;
}



/*! \brief Accesseur au ni�me �l�ment d'interface de l'objet
 \param __Index Index de l'�l�ment � retourner
 \return Le __Index �me �l�ment de l'objet
 \author Hugues Romain
 \date 2000-2005
 
Le comportement de la m�thode est le suivant :
 - Si __Index < 0, renvoie NULL
 - Si __Index est compris entre 0 et le nombre d'�l�ments, renvoie l'�l�ment demand�
 - Si __Index est sup�rieur au nombre d'�l�ments, renvoie NULL
*/
const cInterface_Objet_Element* cInterface_Objet::_getElement( int __Index ) const
{
    if ( __Index < 0 )
        return NULL;

    const cInterface_Objet_Element* __EI = _PremierElement;
    for ( int __Compteur = 0; __Compteur < __Index; __Compteur++ )
    {
        __EI = __EI->getSuivant();
        if ( __EI == NULL )
            break;
    }
    return __EI;
}


const cInterface_Objet_Element_Parametre_TexteConnu* cInterface_Objet_AEvaluer::operator[] ( int __Index ) const
{
    return ( cInterface_Objet_Element_Parametre_TexteConnu* ) _getElement( __Index );
}

const cInterface_Objet_Element_Parametre* cInterface_Objet_Connu_ListeParametres::operator[] ( int __Index ) const
{
    return ( cInterface_Objet_Element_Parametre* ) _getElement( __Index );
}

