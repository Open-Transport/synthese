
#ifndef SYNTHESE_cInterface_Objet_Element_Parametre_DonneeEnvironnement_H
#define SYNTHESE_cInterface_Objet_Element_Parametre_DonneeEnvironnement_H



#include "cInterface_Objet_Element.h"
#include "cInterface_Objet_Element_Parametre.h"


class cInterface_Objet_Element_Parametre_DonneeEnvironnement;



/*! \addtogroup m11
 @{
*/

//! \name Liste des attributs d'objets de la base disponibles � l'affichage (commande A)
//@{



/** Nom de point d'arr�t
 @code A10 @endcode
 @param Objet LogicalPlace * Point d'arr�t
 @param 0 Index de la d�signation utilis�e
 @return Nom de point d'arr�t (selon d�signation en param�tre)
 */
#define EI_ATTRIBUT_ArretLogique_Nom        10


/** Nom de commune de point d'arr�t
 @code A11 @endcode
 @param Objet LogicalPlace * Point d'arr�t
 @param 0 Index de la d�signation utilis�e
 @return Nom de commune de point d'arr�t (selon d�signation en param�tre)
*/
#define EI_ATTRIBUT_ArretLogique_NomCommune      11


/** Libell� simple de ligne
 @code A20 @endcode
 @param Objet cLigne * : Ligne
 @return Libell� simple de ligne
*/
#define EI_ATTRIBUT_Ligne_LibelleSimple       20



/** Image logo de ligne
 @code A21 @endcode
 @param Objet cLigne * : Ligne
 @return Image logo de ligne
*/
#define EI_ATTRIBUT_Ligne_Image         21



/*! \brief Libell� complet de ligne
*/
#define EI_ATTRIBUT_Ligne_LibelleComplet      22



/*! \brief Style de ligne
*/
#define EI_ATTRIBUT_Ligne_Style         23



/*! \brief Article devant pr�c�der le nom du mat�riel roulant de la ligne
*/
#define EI_ATTRIBUT_Ligne_ArticleMateriel      24



/*! \brief Libell� du mat�riel roulant
*/
#define EI_ATTRIBUT_Ligne_LibelleMateriel      25



/*! \brief Nombre de trajets dans un objet cTrajets
Objet � fournir : cTrajets*
 */
#define EI_ATTRIBUT_TRAJETS_TAILLE        30



/*! \brief Pr�sence d'au moins une alerte dans la liste de trajets
Objet � fournir : cTrajets*
 */
#define EI_ATTRIBUT_TRAJETS_AUMOINSUNEALERTE     31



//@}



/*! \brief Element d'interface d�crivant une donn�e issue d'un objet de la base de donn�es
 \author Hugues Romain
 \date 2005
 
Le num�ro de l'objet � �valuer indique quelle donn�es doit �tre puis�e depuis l'instance point�e par le pointeur __Objet de la fonction d'�valuation.
*/
class cInterface_Objet_Element_Parametre_DonneeEnvironnement : public cInterface_Objet_Element_Parametre
{
    protected:
        int _NumeroObjet; //!< Num�ro de l'objet � �valuer
        std::string _TexteVide;  //!< Texte vide � retourner en cas d'index inconnu

    public:
        //! \name Calculateurs
        //@{
        cInterface_Objet_Element* Copie() const;
        cInterface_Objet_Element* Copie( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres ) const;
        int Nombre( const cInterface_Objet_Connu_ListeParametres& __Parametres
                    , const void* __Objet = NULL ) const;
        const std::string& Texte( const cInterface_Objet_Connu_ListeParametres& __Parametres
                             , const void* __Objet = NULL ) const;
        //@}

        //! \name Constructeur
        //@{
        explicit cInterface_Objet_Element_Parametre_DonneeEnvironnement( int __NumeroObjet = INCONNU );
        //@}
};



/*! @} */

#endif

