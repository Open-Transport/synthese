
#ifndef SYNTHESE_CINTERFACEOBJETAEVALUERLISTEPARAMETRES_H
#define SYNTHESE_CINTERFACEOBJETAEVALUERLISTEPARAMETRES_H


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



#endif

