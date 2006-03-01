/*! \file cElementInterfaceTestParametreDynamique.cpp
\brief Impl�mentation classe �l�ment d'interface de test de param�tre dynamique
*/


#include "cElementInterface.h"
#include "cElementInterfaceTestParametreDynamique.h"
#include "cInterface_Objet_Element_Parametre_DonneeEnvironnement.h"
#include "cInterface_Objet_Element_Bibliotheque.h"
#include "cInterface_Objet_Element_Goto.h"
#include "cInterface_Objet_Element_Ligne.h"


/*! \brief Constructeur � partir d'une cha�ne de commandes
 \param __Texte Texte � interpr�ter
 \author Hugues Romain
 \date 2005
 
Le texte � interpr�ter est une suite de trois commandes pouvant �tre s�par�es par des espaces (non obligatoire).
La premi�re commande d�crit l'�l�ment test� d�terminant quel �l�ment sera �valu�
La seconde d�crit l'�l�ment qui sera �valu� si le test a donn� un r�sultat non vide
La troisi�me d�crit l'�l�ment qui sera �valu� si le test a donn� un r�sultat vide.
 
Valeur possible pour l'�l�ment test� :
 - Pxx Copie du xx�me param�tre de l'objet sup�rieur (Element statique ou Element dynamique)
 - Axx Appel au xx�me texte dynamique (Element dynamique)
 
Valeur possible pour les �lements � �valuer (cas vide ou non vide):
 - Pxx xx�me param�tre de l'objet d'interface contenant l'�l�ment (Element statique ou Element dynamique)
 - Oxx Appel au xx�me objet dynamique (Element dynamique)
 - Axx Appel au xx�me texte dynamique (Element dynamique)
 - {...} Copie directe du texte contenu entre les accolades (Element statique)
*/
cInterface_Objet_Element_TestConditionnel::cInterface_Objet_Element_TestConditionnel( const cTexte& __Texte )
{
    // Locales
    int __PositionDebut;
    int __Position;

    // PARAMETRE PREMIER Parcours de la cha�ne de caract�res en entier
    for ( __Position = 0; __Position < __Texte.Taille(); __Position++ )
        if ( __Texte[ __Position ] == 'P' )
        {
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
                    __Texte[ __Position ]
                    && __Texte[ __Position ] != ' ';
                    __Position++
                )
                ;

            _ObjetATester = new cInterface_Objet_Element_Parametre_TexteAEvaluer( __Texte.GetNombre( __Position - __PositionDebut, __PositionDebut ) );
            break;
        }
        else if ( __Texte[ __Position ] == 'A' )
        {
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
                    __Texte[ __Position ]
                    && __Texte[ __Position ] != ' ';
                    __Position++
                )
                ;

            // Chainage
            _ObjetATester = new cInterface_Objet_Element_Parametre_DonneeEnvironnement( __Texte.GetNombre( __Position - __PositionDebut, __PositionDebut ) );
            break;
        }

    // ELEMENT EVALUE SI NON VIDE
    for ( ; __Position < __Texte.Taille(); __Position++ )
        if ( __Texte.Compare( "break", 5, __Position ) )
        {
            _ObjetSiNonVide = NULL;
            __Position += 5;
            break;
        }
        else if ( __Texte[ __Position ] == 'P' )
        {
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
                    __Texte[ __Position ]
                    && __Texte[ __Position ] != ' ';
                    __Position++
                )
                ;

            // Chainage
            _ObjetSiNonVide = new cInterface_Objet_Element_Parametre_TexteAEvaluer( __Texte.GetNombre( __Position - __PositionDebut, __PositionDebut ) );
            break;
        }
        else if ( __Texte[ __Position ] == 'G' )
        {
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
                    __Texte[ __Position ]
                    && __Texte[ __Position ] != ' ';
                    __Position++
                )
                ;

            // Chainage
            _ObjetSiNonVide = new cInterface_Objet_Element_Goto( __Texte.GetNombre( __Position - __PositionDebut, __PositionDebut ) );
            break;
        }
        else if ( __Texte[ __Position ] == 'L' )
        {
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
                    __Texte[ __Position ]
                    && __Texte[ __Position ] != ' ';
                    __Position++
                )
                ;

            // Chainage
            _ObjetSiNonVide = new cInterface_Objet_Element_Ligne( __Texte.GetNombre( __Position - __PositionDebut, __PositionDebut ) );
            break;
        }
        else if ( __Texte[ __Position ] == 'A' )
        {
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
                    __Texte[ __Position ]
                    && __Texte[ __Position ] != ' ';
                    __Position++
                )
                ;

            // Chainage
            _ObjetSiNonVide = new cInterface_Objet_Element_Parametre_DonneeEnvironnement( __Texte.GetNombre( __Position - __PositionDebut, __PositionDebut ) );
            break;
        }
        else if ( __Texte[ __Position ] == 'O' )
        {
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
                    __Texte[ __Position ]
                    && __Texte[ __Position ] != ' ';
                    __Position++
                )
                ;

            // Chainage
            _ObjetSiNonVide = new cInterface_Objet_Element_Bibliotheque( __Texte.GetNombre( __Position - __PositionDebut, __PositionDebut ) );
            break;
        }
        else if ( __Texte[ __Position ] == '{' )
        {
            // D�but du champ texte
            __PositionDebut = ++__Position;
            __Position = __Texte.RechercheOccurenceGauche( '}', 1, __Position );
            _ObjetSiNonVide = new cInterface_Objet_Element_Parametre_TexteConnu( __Texte.Extrait( __PositionDebut, __Position - __PositionDebut ) );
            break;
        }

    // ELEMENT EVALUE SI VIDE
    bool __OKElse = false;
    for ( ; __Position < __Texte.Taille(); __Position++ )
        if ( __Texte.Compare( "break", 5, __Position ) )
        {
            _ObjetSiVide = NULL;
            __Position += 5;
            __OKElse = true;
            break;
        }
        else if ( __Texte[ __Position ] == 'P' )
        {
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
                    __Texte[ __Position ]
                    && __Texte[ __Position ] != ' ';
                    __Position++
                )
                ;

            // Chainage
            _ObjetSiVide = new cInterface_Objet_Element_Parametre_TexteAEvaluer( __Texte.GetNombre( __Position - __PositionDebut, __PositionDebut ) );
            __OKElse = true;
            break;
        }

        else if ( __Texte[ __Position ] == 'G' )
        {
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
                    __Texte[ __Position ]
                    && __Texte[ __Position ] != ' ';
                    __Position++
                )
                ;

            // Chainage
            __OKElse = true;
            _ObjetSiVide = new cInterface_Objet_Element_Goto( __Texte.GetNombre( __Position - __PositionDebut, __PositionDebut ) );
            break;
        }
        else if ( __Texte[ __Position ] == 'L' )
        {
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
                    __Texte[ __Position ]
                    && __Texte[ __Position ] != ' ';
                    __Position++
                )
                ;

            // Chainage
            __OKElse = true;
            _ObjetSiVide = new cInterface_Objet_Element_Ligne( __Texte.GetNombre( __Position - __PositionDebut, __PositionDebut ) );
            break;
        }
        else if ( __Texte[ __Position ] == 'A' )
        {
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
                    __Texte[ __Position ]
                    && __Texte[ __Position ] != ' ';
                    __Position++
                )
                ;

            // Chainage
            _ObjetSiVide = new cInterface_Objet_Element_Parametre_DonneeEnvironnement( __Texte.GetNombre( __Position - __PositionDebut, __PositionDebut ) );
            __OKElse = true;
            break;
        }
        else if ( __Texte[ __Position ] == 'O' )
        {
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
                    __Texte[ __Position ]
                    && __Texte[ __Position ] != ' ';
                    __Position++
                )
                ;

            // Chainage
            _ObjetSiVide = new cInterface_Objet_Element_Bibliotheque( __Texte.GetNombre( __Position - __PositionDebut, __PositionDebut ) );
            __OKElse = true;
            break;
        }
        else if ( __Texte[ __Position ] == '{' )
        {
            // D�but du champ texte
            __PositionDebut = ++__Position;
            __Position = __Texte.RechercheOccurenceGauche( '}', 1, __Position );
            _ObjetSiVide = new cInterface_Objet_Element_Parametre_TexteConnu( __Texte.Extrait( __PositionDebut, __Position - __PositionDebut ) );
            __OKElse = true;
            break;
        }
    if ( !__OKElse )
        _ObjetSiVide = new cInterface_Objet_Element_Parametre_TexteConnu( "" );
}
