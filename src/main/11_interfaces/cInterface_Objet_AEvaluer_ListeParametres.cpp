#include "cInterface_Objet_AEvaluer_ListeParametres.h"
#include "cInterface_Objet_Element_Parametre_TexteAEvaluer.h"
#include "cInterface_Objet_Element_Parametre_TexteConnu.h"
#include "cInterface_Objet_Element_Parametre_DonneeEnvironnement.h"


#include "01_util/Conversion.h"


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

