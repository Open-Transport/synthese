#include "cInterface_Objet.h"
#include "cInterface_Objet_Element.h"

#include "01_util/Conversion.h"


/*! \brief Constructeur
 \author Hugues Romain
 \date 2005
 
Initialise les pointeurs à null en attendant de recevoir les données.
*/
cInterface_Objet::cInterface_Objet()
{
    _PremierElement = NULL;
}



/*! \brief Destructeur
 \author Hugues Romain
 \date 2005
 
Supprime les objets liés.
 
La suppression du premier élément entraîne automatiquement la suppression de l'élément suivant.
*/
cInterface_Objet::~cInterface_Objet()
{
    // Suppression de la liste d'éléments
    delete _PremierElement;
}


bool cInterface_Objet::EstVide() const
{
    return _PremierElement == NULL;
}



/*! \brief Ajout d'élément d'interface en fin de chaîne
 \author Hugues Romain
 \date 2005
 \param __Objet Pointeur vers l'élement d'interface à chaîner
 \return Le pointeur vers l'élément d'interface chaîné : une valeur NULL indique qu'aucun élément n'a été passé en paramètre, ce qui découle dans la majorité des cas d'utilisation d'une erreur d'allocation lors de la création de l'objet destiné à être chaîné.
 
Cette méthode chaîne l'objet d'interface fourni en fin de liste.
 
\warning L'objet fourni sera supprimé à la destruction de l'objet d'interface. Ne pas donner en argument d'objet qui doit être partagé (effectuer une copie préalable)
*/
cInterface_Objet_Element* cInterface_Objet::_Chaine( cInterface_Objet_Element* __Objet )
{
    if ( __Objet != NULL )
    {
        // Chaînage
        if ( _PremierElement == NULL )
            _PremierElement = __Objet;
        else
        {
            // Dernier élément
            cInterface_Objet_Element* __Dernier;

            for ( __Dernier = _PremierElement; __Dernier->Suivant() != NULL; __Dernier = __Dernier->Suivant() )
            { }
            __Dernier->setSuivant( __Objet );
        }
    }
    return __Objet;
}



/*! \brief (A utiliser à l'éxécution) Evaluation de l'objet pour diffusion du résultat dans un flux de sortie
 \author Hugues Romain
 \date 2005
 \param __Flux Flux sur lequel écrire le résultat
 \param __Parametres Paramètres statiques à fournir aux éléments d'interface
 \param __Objet Pointeur vers un objet de données ou de résultat de calcul à fournir aux éléments d'interface qui le nécessitent pour l'évaluation (NULL par défaut)
 \param __Site Pointeur vers le site d'affichage ayant généré l'évaluation (NULL par défaut)
 
L'affichage s'interrompt si un élément renvoie false (commande break)
*/
void 
cInterface_Objet::Evalue( std::ostream& __Flux, const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* __Objet, const cSite* __Site ) const
{
    int __LigneAAtteindre = INCONNU;

    // Parcours de tous les éléments à afficher
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



/*! \brief Accesseur au nième élément d'interface de l'objet
 \param __Index Index de l'élément à retourner
 \return Le __Index ème élément de l'objet
 \author Hugues Romain
 \date 2000-2005
 
Le comportement de la méthode est le suivant :
 - Si __Index < 0, renvoie NULL
 - Si __Index est compris entre 0 et le nombre d'éléments, renvoie l'élément demandé
 - Si __Index est supérieur au nombre d'éléments, renvoie NULL
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


