
#include "cInterface_Objet_Element_Parametre_DonneeEnvironnement.h"

#include "15_env/ConnectionPlace.h"

#include "15_env/City.h"
#include "15_env/Line.h"
#include "15_env/Place.h"

#include "cTrajets.h"

#include "01_util/Conversion.h"

using synthese::env::ConnectionPlace;
using synthese::env::Line;
using synthese::env::Place;


/*! \brief Constructeur
 \param __NumeroObjet Num�ro de l'objet texte dynamique qui sera �valu�
 \author Hugues Romain
 \date 2005
*/
cInterface_Objet_Element_Parametre_DonneeEnvironnement::cInterface_Objet_Element_Parametre_DonneeEnvironnement( int __NumeroObjet ) : cInterface_Objet_Element_Parametre()
{
    _NumeroObjet = __NumeroObjet;
}



/*! \brief Copie exacte d'un �l�ment de texte dynamique
 \author Hugues Romain
 \date 2005
*/
cInterface_Objet_Element* cInterface_Objet_Element_Parametre_DonneeEnvironnement::Copie() const
{
    return new cInterface_Objet_Element_Parametre_DonneeEnvironnement( _NumeroObjet );
}

cInterface_Objet_Element* cInterface_Objet_Element_Parametre_DonneeEnvironnement::Copie( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres ) const
{
    return Copie();
}



int cInterface_Objet_Element_Parametre_DonneeEnvironnement::Nombre( const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* __Objet ) const
{
    return synthese::util::Conversion::ToInt (Texte( __Parametres, __Objet ));
}



/*! \brief Extraction d'une valeur texte depuis l'objet
 \param __Parametres Liste cha�n�e de param�tres correspondant aux besoins de l'�l�ment (voir \ref InterfaceAttribut )
 \param __Objet Pointeur vers l'objet duquel extraire l'attribut
 \return Valeur de l'attribut de l'objet fourni, point� par l'�l�ment, �ventuellement fonction de param�tres fournis
 \author Hugues Romain
 \date 2005
*/
const std::string& cInterface_Objet_Element_Parametre_DonneeEnvironnement::Texte( const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* __Objet ) const
{
    switch ( _NumeroObjet )
    {
        case EI_ATTRIBUT_ArretLogique_Nom:
            return ( ( const Place* ) __Objet ) ->getName(); 

        case EI_ATTRIBUT_ArretLogique_NomCommune:
            return ( ( const Place* ) __Objet ) ->getCity()->getName(); 

        case EI_ATTRIBUT_Ligne_LibelleSimple:
            return ( ( const Line* ) __Objet ) ->getShortName ();

        case EI_ATTRIBUT_Ligne_Image:
            return ( ( const Line* ) __Objet ) ->getImage();

        case EI_ATTRIBUT_Ligne_LibelleComplet:
            return ( ( const Line* ) __Objet ) ->getLongName ();

        case EI_ATTRIBUT_Ligne_Style:
            return ( ( const Line* ) __Objet ) ->getStyle();
	    
        case EI_ATTRIBUT_Ligne_ArticleMateriel:
            return ( ( const Line* ) __Objet ) ->getLongName ();
            // MJ TODO return ( ( const Line* ) __Objet ) ->Materiel() ->getArticle();

        case EI_ATTRIBUT_Ligne_LibelleMateriel:
            return ( ( const Line* ) __Objet ) ->getLongName ();
            // MJ TODO return ( ( const Line* ) __Objet ) ->Materiel() ->getLibelleSimple();

        case EI_ATTRIBUT_TRAJETS_TAILLE:
            return ( ( const cTrajets* ) __Objet ) ->GetTailleTexte();

        case EI_ATTRIBUT_TRAJETS_AUMOINSUNEALERTE:
            return ( ( const cTrajets* ) __Objet ) ->getAuMoinsUneAlerte();

    }

    return _TexteVide;
}
