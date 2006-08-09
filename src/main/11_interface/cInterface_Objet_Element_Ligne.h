
#ifndef SYNTHESE_CINTERFACE_OBJET_ELEMENT_LIGNE_H
#define SYNTHESE_CINTERFACE_OBJET_ELEMENT_LIGNE_H

#include "cInterface_Objet_Element.h"


/*! \brief Objet d'interface contenant l'ordre d'avancer � la ligne portant le num�ro enregistr�
 @ingroup m11
 \warning S'assurer de l'unicit� des num�ros de ligne en les pr�fixant du num�ro de la page �cran
 \warning La ligne num�ro 0 correspond � la sortie du programme
 \warning La ligne num�ro 1 correspond � la prochaine ligne
*/
class cInterface_Objet_Element_Ligne : public cInterface_Objet_Element
{
    protected:
        int _NumeroLigne; //!< Num�ro de ligne

    public:
        //! \name Calculateurs
        //@{
        int Evalue( std::ostream&, const cInterface_Objet_Connu_ListeParametres&
                       , const void* ObjetAAfficher = NULL, const cSite* __Site = NULL ) const;
        cInterface_Objet_Element* Copie( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres ) const;
        int NumeroLigne() const;
        //@}

        //! \name Constructeur
        //@{
        explicit cInterface_Objet_Element_Ligne( int );
        //@}
};

#endif
