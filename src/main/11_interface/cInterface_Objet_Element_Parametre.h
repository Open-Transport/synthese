


/*! \brief El�ment d'interface pouvant �tre inclus dans des objets de type param�tre
 \author Hugues Romain
 \date 2005
 @ingroup m11
 
Cette classe d�clare l'aptitude d'un �l�ment d'interface � fournir une valeur r�f�rence texte ou num�rique � l'�x�cution, directement issue d'un objet en m�moire, � l'oppos� des autres objets qui ne peuvent qu'alimenter un flux de donn�es dans un processus d'�valuation plus complexe fabriquant le r�sultat en direct.
 
L'int�r�t de conna�tre cette propri�t� est de b�n�ficier de deux accesseurs suppl�mentaires permettant de stocker le r�sultat d'une future �valuation de l'objet, par exemple pour int�grer le r�sultat � une processus de d�cision :
 - La m�thode Texte fournir une r�f�rence vers une cha�ne de caract�res qui correspond � ce que produirait une �valuation
 - La m�thode Nombre fournit la traduction num�rique du texte pr�c�dent (0 si le texte n'est pas num�rique)
*/
class cInterface_Objet_Element_Parametre : public cInterface_Objet_Element
{
    public:
        //! \name Accesseurs
        //@{
        const cInterface_Objet_Element_Parametre* getSuivant() const;
        //@}

        //! \name Calculateurs
        //@{
        virtual cInterface_Objet_Element* Copie() const = 0;
        virtual cInterface_Objet_Element* Copie( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres ) const = 0;
        virtual int Nombre( const cInterface_Objet_Connu_ListeParametres& __Parametres
                            , const void* __Objet = NULL ) const = 0;
        virtual const std::string& Texte( const cInterface_Objet_Connu_ListeParametres& __Parametres
                                     , const void* __Objet = NULL ) const = 0;
        int Evalue( std::ostream&, const cInterface_Objet_Connu_ListeParametres&
                       , const void* ObjetAAfficher = NULL, const cSite* __Site = NULL ) const;
        //@}

        cInterface_Objet_Element_Parametre() : cInterface_Objet_Element() {}
        ~cInterface_Objet_Element_Parametre() { }

};

