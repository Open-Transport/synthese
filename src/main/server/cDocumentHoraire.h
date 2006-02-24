
#ifdef SYNTHESE_CDOCUMENTHORAIRE_H
#define SYNTHESE_CDOCUMENTHORAIRE_H

/** @ingroup m15 */
class cDocumentHoraire : public cDocument
{
protected:
	cDate	_DateDebut;
	cDate	_DateFin;

public:
	//!	\name Calculateurs
	//@{
	bool	AAfficher(tDureeEnJours __DelaiAvant = 30, tDureeEnJours __DelaiApres = 0);
	//@}

	//!	\name Modificateurs
	//@{
	bool	SetDateDebut(const cMoment&);
	bool	SetDateFin(const cMoment&);
	bool	SupprimerFichier();
	//@}


};

#endif
