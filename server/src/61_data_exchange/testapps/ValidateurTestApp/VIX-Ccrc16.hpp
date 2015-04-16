#pragma once

namespace synthese
{

	class Ccrc16
	{
		static unsigned short crctable[256];

	public:
		Ccrc16(void);
		~Ccrc16(void);

		void crc_calc( unsigned char ch , unsigned short * crc );
		unsigned short calculCrc16_ForTransparent_Mode( unsigned char * buffer, int len, int init );

		bool testCRC();
	};
}