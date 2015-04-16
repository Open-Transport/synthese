#include <stdio.h>

#include "Log.h"

#include "VIX-Ccrc16.hpp"
#include "VIX-BSC-defines.hpp"
namespace synthese
{

	Ccrc16::Ccrc16(void)
	{
	}

	Ccrc16::~Ccrc16(void)
	{
	}

	void Ccrc16::crc_calc( unsigned char ch , unsigned short * crc )
	{
		static unsigned short crctbl[16] = 	{
			0x0000, 0xcc01, 0xd801, 0x1400,
			0xf001, 0x3c00, 0x2800, 0xe401,
			0xa001, 0x6c00, 0x7800, 0xb401,
			0x5000, 0x9c01, 0x8801, 0x4400
		};

		*crc = ( *crc >> 4 ) ^ crctbl[ ( ch ^ *crc ) & 0x0f ];
		*crc = ( *crc >> 4 ) ^ crctbl[ ( ( ch >> 4 ) ^ *crc ) & 0x0f ];
	}

	unsigned short Ccrc16::calculCrc16_ForTransparent_Mode( unsigned char * buffer, int len, int init )
	{
		int	i;
		unsigned short crc = init;

		//TODO(JD): ensure i++ doesn't mess up

		for ( i =0; i< len; i++)
		{
			if(buffer[i]==BSC_CODE_ASCII_DLE)
			{
				i++;
				if(buffer[i]!=BSC_CODE_ASCII_DLE)
				{	
					//Invalid condition. Are we missing a DLE?
					util::Log::GetInstance().error("Invalid buffer. Transparent mode not respected. Missing some DLE?");
				}
			}
				
			crc_calc( buffer[i], &crc );
		}
		return crc;
	}

	bool Ccrc16::testCRC()
	{
		unsigned char buf[200];
		buf[0]=0x18;
		buf[1]=0x01;
		buf[2]=0x6f;
		buf[3]=0x0b;
		buf[4]=0x19;
		buf[5]=0x06;
		buf[6]=0x1d;
		buf[7]=0x1b;
		buf[8]=0x27;
		buf[9]=0x0e;
		buf[10]=0x00;
		buf[11]=0x14;
		buf[12]=0x00;
		buf[13]=0x39;
		buf[14]=0x39;
		buf[15]=0x39;
		buf[16]=0x38;
		buf[17]=0x27;
		buf[18]=0x0e;
		buf[19]=0x27;
		buf[20]=0x0e;
		buf[21]=0x27;
		buf[22]=0x0e;
		buf[23]=0x00;
		buf[24]=0x03;	// WARNING ETX must be included in order to CRC to works.

		unsigned short crc = calculCrc16_ForTransparent_Mode(buf, 25, 0);

		if(crc==0x84D4)
			return true;
		else
			return false;
	}

}