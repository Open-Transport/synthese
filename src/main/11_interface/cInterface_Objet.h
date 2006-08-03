#ifndef SYNTHESE_CINTERFACEOBJET_H
#define SYNTHESE_CINTERFACEOBJET_H

class cInterface_Objet;
class cInterface_Objet_Element;

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



#endif

