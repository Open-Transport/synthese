#include "cInterface_Objet_AEvaluer_ListeParametres.h"
#include "cInterface_Objet_Element_Parametre_TexteAEvaluer.h"
#include "cInterface_Objet_Element_Parametre_TexteConnu.h"
#include "cInterface_Objet_Element_Parametre_DonneeEnvironnement.h"


#include "01_util/Conversion.h"


/*! \brief Constitution des éléments en fonction d'une ligne de commande à interpréter
 \param __Parametres Objet-Parametres de l'objet de niveau supérieur
 \param __Texte Chaîne de caractères à interpréter pour générer la chaîne d'éléments
 \return true si l'interprétation a été menée avec succès, false sinon
 \author Hugues Romain
 \date 2005
 
La chaîne créée est une suite d'objets dont la description est séparée par des espaces dans le texte. Les commandes suivantes sont disponibles :
 - Pxx Copie du xxème paramètre de l'objet supérieur (Element statique ou Element dynamique)
 - Axx Appel au xxème texte dynamique (Element dynamique)
 - {...} Copie directe du texte contenu entre les accolades (Element de type connu)
*/
bool cInterface_Objet_AEvaluer_ListeParametres::InterpreteTexte( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres, const std::string& __Texte )
{
    // Locales
    int __PositionDebut;

    // Parcours de la chaîne de caractères en entier
    for ( int __Position = 0; __Position < __Texte.size (); __Position++ )
    {
        switch ( __Texte[ __Position ] )
        {
            case 'P':
                for ( __PositionDebut = ++__Position; // Début de l'index du paramètre
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
                for ( __PositionDebut = ++__Position; // Début de l'index du paramètre
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
                // Début du champ texte
                __PositionDebut = ++__Position;

		__Position = __Texte.find ("}", __Position);

                if ( !_Chaine( new cInterface_Objet_Element_Parametre_TexteConnu( __Texte.substr( __PositionDebut, __Position - __PositionDebut ) ) ) )
                    return false;
                break;
        }
    }

    // Sortie succès
    return true;
}

