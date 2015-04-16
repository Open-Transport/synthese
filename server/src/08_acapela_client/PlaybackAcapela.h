#ifndef PLAYBACKACAPELA_H
#define PLAYBACKACAPELA_H

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <iostream>

#include "nscube.h"

using namespace std;

class PlaybackAcapela
{
	public:
		static int callBackSpeechData(const unsigned char *pData,unsigned int cbDataSize,PNSC_SOUND_DATA pSoundData,void *pAppInstanceData);

		static int callBackSpeechEvent(unsigned int nEventID,unsigned int cbEventDataSize,PNSC_EVENT_DATA pEventData,void *pAppInstanceData);

		static int mainFunc(string _text, string _fileName);

		static int mainFunc(string _text, string _fileName, string _ipServer, string _voice);



};

#endif
