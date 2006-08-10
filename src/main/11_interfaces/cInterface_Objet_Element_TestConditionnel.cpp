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



/*! \brief Evaluation de l'�l�ment
 \param __Flux Flux de sortie sur lequel �crire le r�sultat de l'�valuation
 \param __Parametres Param�tres � transmettre � l'�l�ment qui est �valu�
 \param __Objet Objet � transmettre � l'�l�ment qui est �valu�
 \param __Site Pointeur vers le site ayant d�clench� l'�valuation
 \return 
 \author Hugues Romain
 \date 2005
 
Lance l'�valuation de l'�l�ment "si vide" si l'objet � tester �valu� retourne un texte vide ou bien le texte "0", l'autre �l�ment sinon
 
*/
 int cInterface_Objet_Element_TestConditionnel::Evalue( std::ostream& __Flux, const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* __Objet, const cSite* __Site ) const
{
    const std::string & __Texte = _ObjetATester->Texte( __Parametres, __Objet );
    if ( __Texte.size () && __Texte != ( "0" ) )
        return _ObjetSiNonVide == NULL ? NULL : _ObjetSiNonVide->Evalue( __Flux, __Parametres, __Objet, __Site );
    else
        return _ObjetSiVide == NULL ? NULL : _ObjetSiVide->Evalue( __Flux, __Parametres, __Objet, __Site );
}



/*! \brief Copie de l'�l�ment
 \param __Parametres La chaine de param�tres de l'objet d'interface destin� � recevoir le nouvel �l�ment
 \return Element d'interface � ajouter � l'objet d'interface, construit en fonction de sa cha�ne de param�tres
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
cInterface_Objet_Element_TestConditionnel::cInterface_Objet_Element_TestConditionnel( const std::string& __Texte )
{
    // Locales
    int __PositionDebut;
    int __Position;

    // PARAMETRE PREMIER Parcours de la cha�ne de caract�res en entier
    for ( __Position = 0; __Position < __Texte.size (); __Position++ )
        if ( __Texte[ __Position ] == 'P' )
        {
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
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
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
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
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
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
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
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
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
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
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
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
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
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
            // D�but du champ texte
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
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
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
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
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
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
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
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
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
            for ( __PositionDebut = ++__Position; // D�but de l'index du param�tre
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
            // D�but du champ texte
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
