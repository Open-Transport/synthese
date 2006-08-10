
#ifndef SYNTHESE_CINTERFACE_OBJET_ELEMENT_LIGNE_H
#define SYNTHESE_CINTERFACE_OBJET_ELEMENT_LIGNE_H

#include "cInterface_Objet_Element.h"


/*! \brief Objet d'interface contenant l'ordre d'avancer à la ligne portant le numéro enregistré
 @ingroup m11
 \warning S'assurer de l'unicité des numéros de ligne en les préfixant du numéro de la page écran
 \warning La ligne numéro 0 correspond à la sortie du programme
 \warning La ligne numéro 1 correspond à la prochaine ligne
*/
class cInterface_Objet_Element_Ligne : public cInterface_Objet_Element
{
    protected:
        int _NumeroLigne; //!< Numéro de ligne

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
