#include <stdio.h> 
#include <iostream>
#include <fstream>
#include <string>  
#include <array> 
#include <vector> 
#include <sstream> 
#include <tr1/memory> //shared_ptr library
#include <stdlib.h>                           
#include <cmath>

class rocketonium  {
	private:	
		 std::vector<double> peaks ={1e3, 2e3 };
		 double sigPot             =50;
	public:
		rocketonium(){}
		/**
		*Hard coding in data just seems dirty.
		*/
		double getPeak(double diff) {
			if(diff>=-0.3 && diff<-0.1) {
				return 5e3*diff+1.5e3;
			} else if (diff>=-0.1 && diff<0) {
				return 20e3*diff+ 4e3;
			} else if(diff>=0&& diff <0.1) {
				return -20e3*diff+4e3;
			} else if( diff>=0.1 && diff<=0.3) {
				return -5e3*diff+1.5e3;
			}

		}
		double getSigS(double E) {
			return sigPot;
		}
		double getSigA(double E) {
			
			for(double peak: peaks) {
				if(fabs(peak-E)<0.3) {
					return getPeak(E-peak); //treat as identical peaks
				}
			}
			return 0;	
		} 
		double getSigT(double E) {
			return getSigA(E)+getSigS(E);

		}

};

double Q1C() {
	rocketonium rocket; //init that stuff
}
void testRocket() {
	rocketonium rocket;
	double start=999;
	double stepSize=0.05;
	double end=1000.5;

	for(double E=start;E<end;E+=stepSize) {
		std::cout<<"E: "<<E<<" Sig_t: "<<rocket.getSigT(E)<<std::endl;
		
	}



}
int main() {	
	return 1;
}
