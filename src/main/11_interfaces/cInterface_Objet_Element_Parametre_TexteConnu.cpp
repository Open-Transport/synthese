
#include "cInterface_Objet_Element_Parametre_TexteConnu.h"
#include "01_util/Conversion.h"




int cInterface_Objet_Element_Parametre_TexteConnu::Nombre( const cInterface_Objet_Connu_ListeParametres&, const void* ) const
{
    return synthese::util::Conversion::ToInt (_Texte);
}



const std::string& cInterface_Objet_Element_Parametre_TexteConnu::Texte( const cInterface_Objet_Connu_ListeParametres&, const void* ) const
{
    return _Texte;
}



/*! \brief Constructeur d'apr�s un texte
  \param Valeur Texte contenu dans l'�l�ment
  \author Hugues Romain
  \date 2000-2005
*/
cInterface_Objet_Element_Parametre_TexteConnu::cInterface_Objet_Element_Parametre_TexteConnu( const std::string& Valeur ) 
    : cInterface_Objet_Element_Parametre()
    , _Texte (Valeur)
{
    
}




/*! \brief Constructeur d'apr�s un entier
  \param Valeur Nombre contenu dans l'�l�ment
  \author Hugues Romain
  \date 2005
*/
cInterface_Objet_Element_Parametre_TexteConnu::cInterface_Objet_Element_Parametre_TexteConnu( const int Valeur ) 
    : cInterface_Objet_Element_Parametre()
    , _Texte (synthese::util::Conversion::ToString (Valeur))
{

}



/*! \brief Copie d'un �l�ment statique
  \return la copie de l'objet
  \author Hugues Romain
  \date 2000-2005
 
  La copie d'un �l�ment statique consiste � cr�er un �l�ment statique contenant le m�me texte. Les param�tres sont ignor�s.
*/
cInterface_Objet_Element* cInterface_Objet_Element_Parametre_TexteConnu::Copie( const cInterface_Objet_AEvaluer_ListeParametres& ) const
{
    return Copie();
}



/*! \brief Copie d'un �l�ment statique
  \return la copie de l'objet
  \author Hugues Romain
  \date 2000-2005
 
  La copie d'un �l�ment statique consiste � cr�er un �l�ment statique contenant le m�me texte.
*/
cInterface_Objet_Element* cInterface_Objet_Element_Parametre_TexteConnu::Copie() const
{
    return new cInterface_Objet_Element_Parametre_TexteConnu( _Texte );
}
