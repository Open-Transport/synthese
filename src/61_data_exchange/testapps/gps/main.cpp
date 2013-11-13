#include <iostream>
#include <unistd.h>

#include "gps.h"

int main(int argc, char **argv) {
	std::cout << "Starting GPS fun stuffs!" << std::endl; 
	
	gps g;
	g.initSocket("127.0.0.1",2947);
	if(!g.enableTalk())
	{
	    std::cout << "GPS not enabled correctly." << std::endl;
	}
	
	//g.tests();
	
	for(int i=1;i<5;i++)
	{
		std::cout << "start updateFromGps " << std::endl;
		if(g.updateFromGps()){
			double lon,lat;
			lon = 0;
			lat = 0;
			g.getLatLong(lon,lat);
			std::cout << "longitude=" << lon <<std::endl;
			std::cout << "latidue=" << lat <<std::endl;
		}
		
		sleep(1000);
		
	}
	
	if(g.closeTalk())
	{
		std::cout << "GPS  closed!" << std::endl;
	}
	
	return 0;//g.tests();
}
