


/*! \brief Elément d'interface pouvant être inclus dans des objets de type paramètre
 \author Hugues Romain
 \date 2005
 @ingroup m11
 
Cette classe déclare l'aptitude d'un élément d'interface à fournir une valeur référence texte ou numérique à l'éxécution, directement issue d'un objet en mémoire, à l'opposé des autres objets qui ne peuvent qu'alimenter un flux de données dans un processus d'évaluation plus complexe fabriquant le résultat en direct.
 
L'intérêt de connaître cette propriété est de bénéficier de deux accesseurs supplémentaires permettant de stocker le résultat d'une future évaluation de l'objet, par exemple pour intégrer le résultat à une processus de décision :
 - La méthode Texte fournir une référence vers une chaîne de caractères qui correspond à ce que produirait une évaluation
 - La méthode Nombre fournit la traduction numérique du texte précédent (0 si le texte n'est pas numérique)
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

