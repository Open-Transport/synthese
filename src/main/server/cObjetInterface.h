/*! \file cObjetInterface.h
\brief En-t�te classes objets d'interface
*/

#ifndef SYNTHESE_COBJETINTERFACE_H
#define SYNTHESE_COBJETINTERFACE_H

class cInterface_Objet;
class cInterface_Objet_Connu_ListeParametres;
class cInterface_Objet_AEvaluer;
class cInterface_Objet_AEvaluer_ListeParametres;
class cInterface_Objet_Element;
class cInterface_Objet_Element_Parametre;
class cInterface_Objet_Element_Parametre_TexteConnu;

class cSite;

#include <iostream>


/** @addtogroup m11
 @{
*/


/** Objet d'interface
 \author Hugues Romain
 \date 2005
 
Cette classe correspond � la description d'un objet pouvant �tre affich� dans l'interface qui le poss�de.
 
Un objet d'interface est d�crit par une succession d'�l�ments (classes cInterface_Objet_Element et d�riv�es) devant �tre �valu�s successivement lors de l'affichage, en fonction des diff�rents param�tres fournis d'apr�s les donn�es et r�sultats de calculs.
 
Les �l�ments composant l'objet sont index�s par :
 - un pointeur sur le premier �l�ment : cInterface_Objet::_PremierElement (�gal � NULL si l'objet n'est compos� d'aucun �l�ment)
 - un cha�nage entre les �l�ments : cInterface_Objet_Element::_Suivant (�gal � NULL sur le dernier �l�ment)
 
La constitution de la suite d'�l�ments se fait par appels � la m�thode cInterface_Objet::_Chaine assurant l'enregistrement des pointeurs de cha�nage. Cependant, cette m�thode ne garantit pas que les �l�ments cha�n�s soient compatibles avec le type d'objet d'interface. Cette m�thode est donc prot�g�e, et est donc appel�e par des op�rateurs de cha�nages publics, d�finis pour chaque type d'objet.
 
L'affichage de l'objet en fonction des donn�es n�cessaires � celui-ci se fait par la m�thode cInterface_Objet::Evalue.
 
Les objets d'interface peuvent �tre construits � plusieurs fins, dont chacune fait l'objet d'une classe d�finissant les possibilit�s de chaque utilisation, et permettant d'assurer certaines propri�t�s :
 - cInterface_Objet_Connu_ListeParametres : objet contenant une liste d'�l�ments destin�s � �tre fournis ult�rieurement en param�tre d'un autre objet, dont tous les �l�ments sont d'ores et d�j� d�finis
 - cInterface_Objet_AEvaluer : ensemble des objets n�cessitant des informations compl�mentaires pour �tre �valu� :
  - cInterface_Objet_AEvaluer_PageEcran : objet d�crivant une page �cran destin�e � �tre �valu�e pour l'affichage
  - cInterface_Objet_AEvaluer_ListeParametres : objet contenant une liste d'�l�ments destin�s � �tre fournis en param�tres, mais dont les �l�ments ne sont pas n�cessairement connus
*/
class cInterface_Objet
{
    protected:
        cInterface_Objet_Element* _PremierElement; //!< Liste cha�n�e d'�l�ments d'interface d�finissant l'objet

        //! \name Modificateurs
        //@{
        cInterface_Objet_Element* _Chaine( cInterface_Objet_Element* );
        //@}

        //! \name Accesseurs
        //@{
        const cInterface_Objet_Element* _getElement( int ) const;
        //@}

    public:
        //! \name Calculateurs
        //@{
        bool EstVide() const;
        void Evalue( std::ostream&, 
		     const cInterface_Objet_Connu_ListeParametres&, 
		     const void* __Objet = 0
                     , const cSite* __Site = NULL ) const;
        //@}

        //! \name Constructeur et destructeur
        //@{
        cInterface_Objet();
        ~cInterface_Objet();
        //@}
};



/*! \brief Objet d'interface contenant une liste d'�l�ments destin�s � �tre fournis ult�rieurement en param�tre d'un autre objet, dont le contenu des �l�ments est connu
 \author Hugues Romain
 \date 2005
 
L'usage de cette classe garantit que tous ses �l�ments constituants peuvent �tre �valu�s sans autre besoin d'informations.
 
Ainsi, les �l�ments constituant les liste de param�tres connus sont n�c�ssairement des constantes texte ou num�riques (recod�es en texte)
 
Pour construire un objet de ce type, les �l�ments sont successivements encha�n�s gr�ce � l'utilisation des deux op�rateurs << pouvant fabriquer des �l�ments constants � partir de texte ou de nombre entier.
 
*/
class cInterface_Objet_Connu_ListeParametres : public cInterface_Objet
{
    public:
        //! \name Modificateurs
        //@{
        cInterface_Objet_Connu_ListeParametres& operator<<( const std::string& );
        cInterface_Objet_Connu_ListeParametres& operator<<( int );
        //@}

        //! \name Accesseurs
        //@{
        const cInterface_Objet_Element_Parametre* operator[] ( int ) const;
        //@}

};



/*! \brief Objets d'interface n�cessitant des informations compl�mentaires pour �tre �valu�
 \author Hugues Romain
 \date 2005
 
Cette classe sert � r�unir les sous-classes des types d'objet d'interface n�cessitant des informations compl�mentaires pour �tre �valu�s. A l'usage, elle n'est pas instanci�e directement.
 
Les informations compl�mentaires n�cessaires peuvent �tre les suivantes :
 - fourniture d'un objet liste de param�tres connus
 - fourniture d'un pointeur vers un objet de donn�es ou r�sultat d'un calcul
 
La fabrication des objets n�cessitant des informations compl�mentaires pour �tre �valu� s'op�re par le cha�nage d'�lements de type quelconque (connus ou � �valuer � l'aide des informations compl�mentaires) � l'aide de la m�thode cInterface_Objet_AEvaluer::Chaine. Les �l�ments de type non connu seront �valu�s lors de l'affichage.
*/
class cInterface_Objet_AEvaluer : public cInterface_Objet
{
    public:
        //! \name Modificateurs
        //@{
        cInterface_Objet_Element* Chaine( cInterface_Objet_Element* );
        //@}

        //! \name Accesseurs
        //@{
        const cInterface_Objet_Element_Parametre_TexteConnu* operator[] ( int ) const;
        //@}

        //! \name Constructeur et destructeur
        //@{
        cInterface_Objet_AEvaluer() : cInterface_Objet() {}

};



/*! \brief Objets d'interface contenant une liste d'�l�ments destin�s � �tre fournis en param�tres, mais dont les �l�ments ne sont pas n�cessairement connus
 \author Hugues Romain
 \date 2005
 
Pour construire les objets de cette classe, le cha�nage classique d'�l�ments est naturellement possible. S'ajoute cependant une fonctionnalit� d'interpr�tation d'une cha�ne de caract�res d�crivant l'ensemble des �l�ments le constituant, assur�e par la m�thode cInterface_Objet_AEvaluer_ListeParametres::InterpreteTexte.
*/
class cInterface_Objet_AEvaluer_ListeParametres : public cInterface_Objet_AEvaluer
{
    public:
        //! \name Modificateurs
        //@{
        bool InterpreteTexte( const cInterface_Objet_AEvaluer_ListeParametres&, const std::string& );
        //@}

        //! \name Constructeur et destructeurs
        //@{
        cInterface_Objet_AEvaluer_ListeParametres() : cInterface_Objet_AEvaluer() {}
        //@}

};

        /** @} */
#define SYNTHESE_CINTERFACE_OBJET_AEVALUER_LISTE_PARAMETRES






        /*! \brief Ajout d'un �l�ment en fin de cha�ne
         \param __Objet L'�l�ment � ajouter
         \return L'objet apr�s avoir �t� modifi�
         \author Hugues Romain
         \date 2005
         
        Cette impl�mentation sert � autoriser l'usage de cObjetInterface::_Chaine sur tout type d'�l�ment, dans le cas o� l'objet impact� est bien un objet permettant les �l�ments non connus.
        */
        inline cInterface_Objet_Element* cInterface_Objet_AEvaluer::Chaine( cInterface_Objet_Element* __Objet )
        {
            return ( cInterface_Objet_Element* ) _Chaine( __Objet );
        }

        inline bool cInterface_Objet::EstVide() const
        {
            return _PremierElement == NULL;
        }


#endif

