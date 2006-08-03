
#ifndef SYNTHESE_CINTERFACEOBJETAEVALUERLISTEPARAMETRES_H
#define SYNTHESE_CINTERFACEOBJETAEVALUERLISTEPARAMETRES_H


/*! \brief Objets d'interface contenant une liste d'éléments destinés à être fournis en paramètres, mais dont les éléments ne sont pas nécessairement connus
 \author Hugues Romain
 \date 2005
 
Pour construire les objets de cette classe, le chaînage classique d'éléments est naturellement possible. S'ajoute cependant une fonctionnalité d'interprétation d'une chaîne de caractères décrivant l'ensemble des éléments le constituant, assurée par la méthode cInterface_Objet_AEvaluer_ListeParametres::InterpreteTexte.
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



#endif

