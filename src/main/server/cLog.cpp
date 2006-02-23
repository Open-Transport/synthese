
#include "cLog.h"
#include "SYNTHESE.h"
#include "cElementTrajet.h"
#include "cArretPhysique.h"
#include "LogicalPlace.h"

extern SYNTHESE Synthese;


cLog::cLog(tNiveauLog __Niveau)
{
	if (__Niveau == LogInconnu)
		SetNiveau(Synthese.getNiveauLog());
	else
		SetNiveau(__Niveau);
}



void cLog::Ecrit(tNumeroMessageStandard __NumeroMessage, const cTexte& __Objet, const cTexte& __Donnees)
{
	if (ControleNiveau(Synthese._NiveauMessageStandard[__NumeroMessage]))
	{
		cTexte __MessageComplet;
		__MessageComplet << Synthese._MessageStandard[__NumeroMessage] << ";" << __Objet << ";" << __Donnees;
		Ecrit(Synthese._NiveauMessageStandard[__NumeroMessage], __MessageComplet, Synthese._CodesMessageStandard[__NumeroMessage]);
	}
}

void cLog::EcritElementTrajet(tNiveauLog __Niveau, const cElementTrajet* __ElementTrajet, const cTexte& __Message, const cTexte& __Code)
{
		cTexte __MessageET;
		cTexte __MomentISO;
		__MessageET << "cElementTrajet;";
		if (__ElementTrajet->getOrigin())
			__MessageET << __ElementTrajet->getOrigin()->getLogicalPlace()->getDesignationOD();
		__MessageET << ";"
					<< __ElementTrajet->getOrigin()->getRankInLogicalPlace() << ";";
		__MomentISO.Vide();
		__MomentISO << __ElementTrajet->MomentDepart();
		__MessageET << __MomentISO << ";";
		if (__ElementTrajet->getDestination())
			__MessageET << __ElementTrajet->getDestination()->getLogicalPlace()->getDesignationOD();
		__MessageET << ";"
					<< __ElementTrajet->getDestination()->getRankInLogicalPlace() << ";";
		__MomentISO.Vide();
		__MomentISO << __ElementTrajet->MomentArrivee();
		__MessageET << __MomentISO << ";";
		if (__ElementTrajet->getLigne())
			__MessageET << __ElementTrajet->getLigne()->getCode();
		__MessageET << ";";
		__MessageET << __Message;
		
		Ecrit(__Niveau, __MessageET, __Code);
}

void cLog::EcritTrajet(tNiveauLog __Niveau, const cTrajet& __Trajet, int __Numero, const cTexte& __Message, const cTexte& __Code)
{
	cTexte __MessageTrajet;
	cTexteSQL __MomentISO;
	
	__MessageTrajet	<< "cTrajet;"
					<< __Numero << ";";
	for (int __i=0; __i<NMAXPROFONDEUR; __i++)
	{
		if (__i < __Trajet.Taille())
			__MessageTrajet << "X";
		__MessageTrajet << ";";
	}
	if (__Trajet.Taille())
	{
		__MessageTrajet << __Trajet.getOrigin()->getLogicalPlace()->getDesignationOD() << ";";
		__MessageTrajet << __Trajet.PremierElement()->getOrigin()->getRankInLogicalPlace() << ";";
		__MessageTrajet << __Trajet.PremierElement()->getDistanceCarreeObjectif().DistanceCarree() << ";";
		__MomentISO.Vide();
		__MomentISO << __Trajet.getMomentDepart();
		__MessageTrajet << __MomentISO << ";";
		__MessageTrajet << __Trajet.PremierElement()->getLigne()->getCode() << ";";
		
		__MessageTrajet << __Trajet.getDestination()->getLogicalPlace()->getDesignationOD() << ";";
		__MessageTrajet << __Trajet.DernierElement()->getDestination()->getRankInLogicalPlace() << ";";
		__MessageTrajet << __Trajet.DernierElement()->getDistanceCarreeObjectif().DistanceCarree() << ";";
		__MomentISO.Vide();
		__MomentISO << __Trajet.getMomentArrivee();
		__MessageTrajet << __MomentISO << ";";
		__MessageTrajet << __Trajet.DernierElement()->getLigne()->getCode() << ";";
	}
	else
		__MessageTrajet << ";;;;;;;;;;";
	__MessageTrajet	<< __Message;
	
	Ecrit(__Niveau, __MessageTrajet, __Code);

}

