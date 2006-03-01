
#ifdef SYNTHESE_CDOCUMENTHORAIRE_H
#define SYNTHESE_CDOCUMENTHORAIRE_H

/** @ingroup m15 */
class cDocumentHoraire : public cDocument
{
    protected:
        synthese::time::Date _DateDebut;
        synthese::time::Date _DateFin;

    public:
        //! \name Calculateurs
        //@{
        bool AAfficher( int __DelaiAvant = 30, int __DelaiApres = 0 );
        //@}

        //! \name Modificateurs
        //@{
        bool SetDateDebut( const synthese::time::DateTime& );
        bool SetDateFin( const synthese::time::DateTime& );
        bool SupprimerFichier();
        //@}


};

#endif
