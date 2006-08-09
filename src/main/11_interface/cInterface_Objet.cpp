#include "cInterface_Objet.h"
#include "cInterface_Objet_Element.h"

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


bool cInterface_Objet::EstVide() const
{
    return _PremierElement == NULL;
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
void 
cInterface_Objet::Evalue( std::ostream& __Flux, const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* __Objet, const cSite* __Site ) const
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
const cInterface_Objet_Element* 
cInterface_Objet::_getElement( int __Index ) const
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


