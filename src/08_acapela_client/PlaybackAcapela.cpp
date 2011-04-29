#include "PlaybackAcapela.h"


int PlaybackAcapela::callBackSpeechData(const unsigned char *pData,unsigned int cbDataSize,PNSC_SOUND_DATA pSoundData,void *pAppInstanceData)
{
    fwrite(pData,cbDataSize,1,(FILE *)pAppInstanceData);
    return 0;
}

int PlaybackAcapela::callBackSpeechEvent(unsigned int nEventID,
                        unsigned int cbEventDataSize,
                        PNSC_EVENT_DATA pEventData,
                        void *pAppInstanceData)
{
    switch(nEventID)
    {
		case NSC_EVID_TEXT_STARTED :
		{
			NSC_EVENT_DATA_TextStarted *pEVTS = (NSC_EVENT_DATA_TextStarted *)pEventData;
			//printf("IdText %i : Synthesis started", (int)pEVTS->pUserData);
			break;
		}
		case NSC_EVID_TEXT_DONE :
		{
			NSC_EVENT_DATA_TextDone *pEVTD = (NSC_EVENT_DATA_TextDone *)pEventData;
			//printf("IdText %i : Synthesis done", (int)pEVTD->pUserData);
			break;
		}
    }
    return 0;
}


int PlaybackAcapela::mainFunc(string _text, string _fileName)
{
	return mainFunc(_text, _fileName, "10.12.155.36", "claire8kmu");
}

int PlaybackAcapela::mainFunc(string _text, string _fileName, string _ipServer, string _voice)
{
    nscHSRV					hSrv;					/* Handle of an Acapela Telecom server context returned by nscCreateServerContext */
    nscRESULT				Result;					/* Result of nsc functions */
    NSC_SRVINFO_DATA		SrvInfo;				/* This structure describes informations about server */
    NSC_FINDVOICE_DATA		FindVoice;				/* This structure describes a voice found by nscFindFirstVoice */
    nscHANDLE				hVoice;					/* Handle to be used in subsequent calls to nscFindNextVoice */
    nscHANDLE				hDispatch;				/* Handle will be passed to the event dispatching functions and returned by nscCreateDispatcher */
    nscHANDLE				hTTS;					/* Handle of channel returned by nscLockChannel */
    nscCHANID				ChId;					/* Id Of channel returned by nscInitChannel */
    NSC_EXEC_DATA			ExecData;				/* Informations using by a call to nscExecChannel */
    NSC_FINDVOICE_DATA 		*ptabFindVoice;

    int						idtabFindVoice = 0;

    int nbvoice;

	// voice file type AU head
    static const unsigned char auHead[24] =
	{
		0x2E, 0x73, 0x6E, 0x64,         /*magic number*/
		0x00, 0x00, 0x00, 0x18,         /*data offset*/
		0xFF, 0xFF, 0xFF, 0xFF,         /*data size*/
		0x00, 0x00, 0x00, 0x01,         /*encoding*/
        0x00, 0x00, 0x1F, 0x40,         /*sample rate*/
        0x00, 0x00, 0x00, 0x01          /*channels*/
	};

	/* connect to server using old client-server communication protocol */
    if ((Result = nscCreateServerContext(NSC_AF_INET,0,_ipServer.c_str(),&hSrv)) == NSC_OK)
    {
          ////cout<<"nsc connected"<<endl;
    }
    else
    {
        //cout<<"EROR: nscCreateServerContext return "<<Result<<endl;
        return -1;
    }


    /* Dispatch */
    if ((Result = nscCreateDispatcher(&hDispatch)) == NSC_OK)
    {
        ////cout<<"Dispatcher created"<<endl;
    }
    else
    {
        //cout<<"ERROR: nscCreateDispatcher return "<<Result<<endl;
        return -1;
    }



    /* Enumeration */
    if ((Result = nscGetServerInfo(hSrv, &SrvInfo)) != NSC_OK)
    {
		//cout<<"ERROR: nscGetServerInfo return "<<Result<<endl;
		return -1;
    }
    ptabFindVoice = (NSC_FINDVOICE_DATA *)malloc(sizeof(NSC_FINDVOICE_DATA) * SrvInfo.nMaxNbVoice);
    if (ptabFindVoice == NULL)
    {
        //cout<<"ERROR: not enough memory"<<endl;
        return -1;
    }


    // search voice
    if ((Result = nscFindFirstVoice(hSrv,NULL,0,0,0,&FindVoice,&hVoice)) == NSC_OK)
    {
        ptabFindVoice[idtabFindVoice++] = FindVoice;
        while((Result = nscFindNextVoice(hVoice,&FindVoice)) == NSC_OK)
		{
            ptabFindVoice[idtabFindVoice++] = FindVoice;
		}
    }
    else
    {
        //cout<<"ERROR: nscFindFirstVoice return "<<Result<<endl;
        return -1;
    }
    nscCloseFindVoice(hVoice);

    nbvoice = idtabFindVoice;
    idtabFindVoice = 0;

    while (nbvoice > 0)
    {
		//cerr<<ptabFindVoice[idtabFindVoice].cVoiceName<<endl;
		idtabFindVoice++;
		nbvoice--;
    }


    /* Init channel  */

    if ((Result = nscInitChannel(hSrv,_voice.c_str(),0,0,hDispatch,&ChId)) == NSC_OK)
    {
        ////cout<<"Channel initialised"<<endl;
    }
    else
    {
        //cout<<"ERROR: nscInitChannel return "<<Result<<endl;
        return -1;
    }

    /* Lock channel */
    if( (Result = nscLockChannel(hSrv,ChId,hDispatch,&hTTS)) == NSC_OK)
    {
        ////cout<<"Channel locked"<<endl;
    }
    else
    {
        //cout<<"ERROR: nscLockChannel return "<<Result<<endl;
        return -1;
    }

	if((Result=nscSetParamChannel(hTTS,NSC_PRM_SPEED,96))== NSC_OK)
	{
		cerr<<"parameter of voice speed changed"<<endl;
	}
	else
	{
		cerr<<"change of voice speed error: "<<Result<<endl;
	}


    /* AddText channel */
     ////cout<<"textVoiceAck: "<<_text<<endl;
     ////cout<<"voiceFileNameAck: "<<_fileName<<endl;

    int IdText = 1;
    ////cout<<"Text %i adding...   "<<IdText<<endl;

    if ((Result = nscAddText(hTTS,_text.c_str(),(void *)IdText)) == NSC_OK)
    {
        ////cout<<"Text added: "<<_text<<endl;
    }
    else
    {
        //cout<<"ERROR: nscAddText return "<<Result<<endl;
        return -1;
    }

    /* ExecChannel */
    ExecData.pfnSpeechData = callBackSpeechData;
    ExecData.pfnSpeechEvent = callBackSpeechEvent;
    ExecData.ulEventFilter = NSC_EVTBIT_TEXT;
    ExecData.bEventSynchroReq = 1;
    ExecData.vsSoundData.uiSize = 0;
    ExecData.vsSoundData.pSoundBuffer = NULL;

	FILE *f;
    if ((f = fopen (_fileName.c_str(), "w+")) != NULL)
    {
	    // fput head inside
            //fwrite (auHead[i] ,sizeof(auHead[i]),sizeof(auHead),f);

        for(int i=0;i<24;i++)
        {
            fputc(auHead[i],f);
        }

        ExecData.pAppInstanceData=(void *)f;

        if((Result = nscExecChannel(hTTS,&ExecData)) == NSC_OK)
		{
	            ////cout<<"Channel Executed "<<endl;
		}
		else
		{
	            //cout<<"ERROR: nscExecChannel return"<<endl;
	            return -1;
		}
		fclose(f);
    }
    else
    {
		//cout<<"ERROR:PERMISSION DENIED"<<endl;
    }

    /* Unlock channel */

    if ((Result = nscUnlockChannel(hTTS)) == NSC_OK)
    {
        ////cout<<"Channel unlocked"<<endl;
    }
    else
    {
        //cout<<"ERROR: nscUnlockChannel return  "<<Result<<endl;
        return -1;
    }

    /* Close channel */

    if( (Result = nscCloseChannel(hSrv,ChId))==NSC_OK)
    {
        ////cout<<"Channel closed"<<endl;
    }
    else
    {
        //cout<<"ERROR: nscCloseChannel return "<<endl;
        return -1;
    }


    /* DeleteDispatcher */

    if ((Result = nscDeleteDispatcher(hDispatch)) == NSC_OK)
    {
        ////cout<<"Dispatcher Deleted"<<endl;
    }
    else
    {
        //cout<<"ERROR: nscDeleteDispatcher return "<<Result<<endl;
        return -1;
    }

    /* ReleaseServer */

    if ((Result = nscReleaseServerContext(hSrv)) == NSC_OK)
    {
        ////cout<<"Disconnected"<<endl;
    }
    else
    {
        //cout<<"ERROR: nscReleaseServerContext return "<<Result<<endl;
        return -1;
    }

    return 0;
}
