
#include "cInterface_Objet_AEvaluer_PageEcran.h"
#include "cInterface_Objet_Element.h"


/*! \brief Constitution d'un objet d'interface d�finissant tout ou partie d'une page d'�cran, par inclusion d'une copie des �l�ments d'un autre objet de m�me type
 \author Hugues Romain
 \date 2005
 \param __Objet Objet � copier
 \param __Parametres Parametres de la copie
 \return true si la copie a �t� effectu�e avec succ�s, false sinon.
 
L'ensemble des �l�ments de l'objet � inclure sont copi�s dans l'objet courant.
Selon le type d'�l�ment � inclure, la copie peut tenir compte des param�tres de la copie :
 - Pour un �l�ment connu, les param�tres sont ignor�s
 - Pour un �l�ment de type param�tre � �valuer � l'�x�cution, l'objet copi� est puis� de l'objet param�tres de la copie : l'�l�ment du rang de l'index fourni dans l'objet inclu est recopi�.
   
*/
bool cInterface_Objet_AEvaluer_PageEcran::InclureCopie( const cInterface_Objet_AEvaluer_PageEcran& __Objet, const cInterface_Objet_AEvaluer_ListeParametres& __Parametres )
{
    // Parcours des �l�ments de l'objet � copier
    for ( const cInterface_Objet_Element * __Element = __Objet._PremierElement; __Element != NULL; __Element = __Element->getSuivant() )
        if ( !_Chaine( __Element->Copie( __Parametres ) ) )
            return false;

    // Sortie succ�s
    return true;
}

