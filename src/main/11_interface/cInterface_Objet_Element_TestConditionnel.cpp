#include "cInterface_Objet_Element_TestConditionnel.h"

#include "cInterface_Objet_Element_Parametre.h"
#include "cInterface_Objet_Element.h"
#include "cInterface_Objet_AEvaluer_ListeParametres.h"
#include "cInterface_Objet_Connu_ListeParametres.h"
#include "cInterface_Objet_Element_Parametre_TexteAEvaluer.h"
#include "cInterface_Objet_Element_Parametre_DonneeEnvironnement.h"
#include "cInterface_Objet_Element_Goto.h"
#include "cInterface_Objet_Element_Ligne.h"
#include "cInterface_Objet_Element_Bibliotheque.h"
#include "cInterface_Objet_Element_Parametre_TexteConnu.h"


#include "01_util/Conversion.h"



/*! \brief Evaluation de l'élément
 \param __Flux Flux de sortie sur lequel écrire le résultat de l'évaluation
 \param __Parametres Paramètres à transmettre à l'élément qui est évalué
 \param __Objet Objet à transmettre à l'élément qui est évalué
 \param __Site Pointeur vers le site ayant déclenché l'évaluation
 \return 
 \author Hugues Romain
 \date 2005
 
Lance l'évaluation de l'élément "si vide" si l'objet à tester évalué retourne un texte vide ou bien le texte "0", l'autre élément sinon
 
*/
 int cInterface_Objet_Element_TestConditionnel::Evalue( std::ostream& __Flux, const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* __Objet, const cSite* __Site ) const
{
    const std::string & __Texte = _ObjetATester->Texte( __Parametres, __Objet );
    if ( __Texte.size () && __Texte != ( "0" ) )
        return _ObjetSiNonVide == NULL ? NULL : _ObjetSiNonVide->Evalue( __Flux, __Parametres, __Objet, __Site );
    else
        return _ObjetSiVide == NULL ? NULL : _ObjetSiVide->Evalue( __Flux, __Parametres, __Objet, __Site );
}



/*! \brief Copie de l'élément
 \param __Parametres La chaine de paramètres de l'objet d'interface destiné à recevoir le nouvel élément
 \return Element d'interface à ajouter à l'objet d'interface, construit en fonction de sa chaîne de paramètres
 \author Hugues Romain
 \date 2005
*/
 cInterface_Objet_Element* cInterface_Objet_Element_TestConditionnel::Copie( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres ) const
{
    cInterface_Objet_Element_TestConditionnel * __EI = new cInterface_Objet_Element_TestConditionnel();
    __EI->_ObjetATester = ( cInterface_Objet_Element_Parametre* ) _ObjetATester->Copie( __Parametres );
    __EI->_ObjetSiNonVide = _ObjetSiNonVide == NULL ? NULL : _ObjetSiNonVide->Copie( __Parametres );
    __EI->_ObjetSiVide = _ObjetSiVide == NULL ? NULL : _ObjetSiVide->Copie( __Parametres );
    return __EI;
}



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
cInterface_Objet_Element_TestConditionnel::cInterface_Objet_Element_TestConditionnel( const std::string& __Texte )
{
    // Locales
    int __PositionDebut;
    int __Position;

    // PARAMETRE PREMIER Parcours de la chaîne de caractères en entier
    for ( __Position = 0; __Position < __Texte.size (); __Position++ )
        if ( __Texte[ __Position ] == 'P' )
        {
            for ( __PositionDebut = ++__Position; // Début de l'index du paramètre
                    __Texte[ __Position ]
                    && __Texte[ __Position ] != ' ';
                    __Position++
                )
                ;

            _ObjetATester = new cInterface_Objet_Element_Parametre_TexteAEvaluer( 
		
		synthese::util::Conversion::ToInt (
		    __Texte.substr (__PositionDebut, 
				    __Position - __PositionDebut))
		);
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
            _ObjetATester = new cInterface_Objet_Element_Parametre_DonneeEnvironnement( 
		synthese::util::Conversion::ToInt (
		    __Texte.substr (__PositionDebut, 
				    __Position - __PositionDebut))
		);
            break;
        }

    // ELEMENT EVALUE SI NON VIDE
    for ( ; __Position < __Texte.size (); __Position++ )

	if ("break" == __Texte.substr (__Position, 5))
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
            _ObjetSiNonVide = new cInterface_Objet_Element_Parametre_TexteAEvaluer( 

		synthese::util::Conversion::ToInt (
		    __Texte.substr (__PositionDebut, 
				    __Position - __PositionDebut))
);
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
            _ObjetSiNonVide = new cInterface_Objet_Element_Goto( 
		synthese::util::Conversion::ToInt (
		    __Texte.substr (__PositionDebut, 
				    __Position - __PositionDebut))
		);
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
            _ObjetSiNonVide = new cInterface_Objet_Element_Ligne( 
		synthese::util::Conversion::ToInt (
		    __Texte.substr (__PositionDebut, 
				    __Position - __PositionDebut))
		);
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
            _ObjetSiNonVide = new cInterface_Objet_Element_Parametre_DonneeEnvironnement( 
		synthese::util::Conversion::ToInt (
		    __Texte.substr (__PositionDebut, 
				    __Position - __PositionDebut))
		);
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
            _ObjetSiNonVide = new cInterface_Objet_Element_Bibliotheque( 
		synthese::util::Conversion::ToInt (
		    __Texte.substr (__PositionDebut, 
				    __Position - __PositionDebut))
		);
            break;
        }
        else if ( __Texte[ __Position ] == '{' )
        {
            // Début du champ texte
            __PositionDebut = ++__Position;

            __Position = __Texte.find ("}", __Position);

            _ObjetSiNonVide = new cInterface_Objet_Element_Parametre_TexteConnu( 
		__Texte.substr (__PositionDebut, __Position - __PositionDebut));
            break;
        }

    // ELEMENT EVALUE SI VIDE
    bool __OKElse = false;
    for ( ; __Position < __Texte.size (); __Position++ )
	if ("break" == __Texte.substr (__Position, 5))
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
            _ObjetSiVide = new cInterface_Objet_Element_Parametre_TexteAEvaluer( 
		synthese::util::Conversion::ToInt (
		    __Texte.substr (__PositionDebut, 
				    __Position - __PositionDebut))
		);
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
            _ObjetSiVide = new cInterface_Objet_Element_Goto( 
		synthese::util::Conversion::ToInt (
		    __Texte.substr (__PositionDebut, 
				    __Position - __PositionDebut))
		);
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
            _ObjetSiVide = new cInterface_Objet_Element_Ligne( 
		synthese::util::Conversion::ToInt (
		    __Texte.substr (__PositionDebut, 
				    __Position - __PositionDebut))
		);
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
            _ObjetSiVide = new cInterface_Objet_Element_Parametre_DonneeEnvironnement( 
		synthese::util::Conversion::ToInt (
		    __Texte.substr (__PositionDebut, 
				    __Position - __PositionDebut))
		);
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
            _ObjetSiVide = new cInterface_Objet_Element_Bibliotheque( 
		synthese::util::Conversion::ToInt (
		    __Texte.substr (__PositionDebut, 
				    __Position - __PositionDebut))
		);
            __OKElse = true;
            break;
        }
        else if ( __Texte[ __Position ] == '{' )
        {
            // Début du champ texte
            __PositionDebut = ++__Position;
            __Position = __Texte.find ("}", __Position);

            _ObjetSiVide = new cInterface_Objet_Element_Parametre_TexteConnu(
		__Texte.substr (__PositionDebut, 
				__Position - __PositionDebut));
            __OKElse = true;
            break;
        }
    if ( !__OKElse )
        _ObjetSiVide = new cInterface_Objet_Element_Parametre_TexteConnu( "" );
}
