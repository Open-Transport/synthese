/*! \file cElementInterfaceTestParametreDynamique.cpp
\brief Implémentation classe élément d'interface de test de paramètre dynamique
*/


#include "cElementInterface.h"
#include "cElementInterfaceTestParametreDynamique.h"
#include "cInterface_Objet_Element_Parametre_DonneeEnvironnement.h"
#include "cInterface_Objet_Element_Bibliotheque.h"
#include "cInterface_Objet_Element_Goto.h"
#include "cInterface_Objet_Element_Ligne.h"


/*! \brief Constructeur à partir d'une chaîne de commandes
 \param __Texte Texte à interpréter
 \author Hugues Romain
 \date 2005
 
Le texte à interpréter est une suite de trois commandes pouvant être séparées par des espaces (non obligatoire).
La première commande décrit l'élément testé déterminant quel élément sera évalué
La seconde décrit l'élément qui sera évalué si le test a donné un résultat non vide
La troisième décrit l'élément qui sera évalué si le test a donné un résultat vide.
 
Valeur possible pour l'élément testé :
 - Pxx Copie du xxème paramètre de l'objet supérieur (Element statique ou Element dynamique)
 - Axx Appel au xxème texte dynamique (Element dynamique)
 
Valeur possible pour les élements à évaluer (cas vide ou non vide):
 - Pxx xxème paramètre de l'objet d'interface contenant l'élément (Element statique ou Element dynamique)
 - Oxx Appel au xxème objet dynamique (Element dynamique)
 - Axx Appel au xxème texte dynamique (Element dynamique)
 - {...} Copie directe du texte contenu entre les accolades (Element statique)
*/
cInterface_Objet_Element_TestConditionnel::cInterface_Objet_Element_TestConditionnel( const cTexte& __Texte )
{
    // Locales
    int __PositionDebut;
    int __Position;

    // PARAMETRE PREMIER Parcours de la chaîne de caractères en entier
    for ( __Position = 0; __Position < __Texte.Taille(); __Position++ )
        if ( __Texte[ __Position ] == 'P' )
        {
            for ( __PositionDebut = ++__Position; // Début de l'index du paramètre
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
            for ( __PositionDebut = ++__Position; // Début de l'index du paramètre
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
            for ( __PositionDebut = ++__Position; // Début de l'index du paramètre
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
            for ( __PositionDebut = ++__Position; // Début de l'index du paramètre
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
            for ( __PositionDebut = ++__Position; // Début de l'index du paramètre
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
            for ( __PositionDebut = ++__Position; // Début de l'index du paramètre
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
            for ( __PositionDebut = ++__Position; // Début de l'index du paramètre
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
            // Début du champ texte
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
            for ( __PositionDebut = ++__Position; // Début de l'index du paramètre
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
            for ( __PositionDebut = ++__Position; // Début de l'index du paramètre
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
            for ( __PositionDebut = ++__Position; // Début de l'index du paramètre
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
            for ( __PositionDebut = ++__Position; // Début de l'index du paramètre
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
            for ( __PositionDebut = ++__Position; // Début de l'index du paramètre
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
            // Début du champ texte
            __PositionDebut = ++__Position;
            __Position = __Texte.RechercheOccurenceGauche( '}', 1, __Position );
            _ObjetSiVide = new cInterface_Objet_Element_Parametre_TexteConnu( __Texte.Extrait( __PositionDebut, __Position - __PositionDebut ) );
            __OKElse = true;
            break;
        }
    if ( !__OKElse )
        _ObjetSiVide = new cInterface_Objet_Element_Parametre_TexteConnu( "" );
}
