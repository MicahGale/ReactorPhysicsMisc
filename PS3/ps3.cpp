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

using namespace std;

double trapInt(vector<double>& vals,double stepSize);
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
		double NR_RI_intGrand(double E, double sigD) {
			return getSigA(E)/(E*(getSigT(E)+sigD));
		}
		double get_NR_flux( double E, double sigD) {
			return (sigPot+sigD)/(E*(getSigT(E)+sigD));
		}

		double get_WR_flux(double E, double sigD) {
			return sigD/(E*(getSigA(E)+sigD));
		}
		double WR_RI_intGrand(double E, double sigD) {
			return getSigA(E)/(E*(getSigA(E)+sigD));
		}

		double collapseXS_NR(double lowE, double upE,double sigD, int bins) {
			vector<double> RI, flux;
			double stepSize;

			stepSize=(upE-lowE)/bins;
			for(double E=lowE;E<=upE;E+=stepSize) {
				RI.push_back(NR_RI_intGrand(E,sigD));
				flux.push_back(get_NR_flux(E,sigD));
			}
			return (sigPot+sigD)*trapInt(RI,stepSize)/trapInt(flux,stepSize);
			//do the collapse integral
		}
                double collapseXS_WR(double lowE, double upE,double sigD, int bins) {
                        vector<double> RI, flux;
                        double stepSize;

                        stepSize=(upE-lowE)/bins;
                        for(double E=lowE;E<=upE;E+=stepSize) {
                                RI.push_back(WR_RI_intGrand(E,sigD));
                                flux.push_back(get_WR_flux(E,sigD));
                        }
                        return (sigD)*trapInt(RI,stepSize)/trapInt(flux,stepSize);
                        //do the collapse integral
                }


};
double trapInt(vector<double> & vals, double stepSize) {
	double sum;
	int multiplier;

	sum=0;
	for(int i=0;i<vals.size();i++) {
		if(i==0||i==vals.size()-1) {
			multiplier=1;
		} else {
			multiplier=2;
		}
		sum+=multiplier*vals[i]*stepSize;
	
	}
	return sum;

}
double Q1C() {
	rocketonium rocket; //init that stuff
	double sigD, C_nat_sig_t,lowE,upE;
	vector<double> dilution;
	int bins;
	ofstream out;

	//define problem constants
	C_nat_sig_t=4.75; //eyeball it from ENDF-B/VII.1
	dilution= {10,100,1e3};
	lowE=1.0;
	upE=5e3;
	bins=500e3;

	//file stuff
	out.open("Q1Cxs.tex",ios::trunc);  //opens it
	out<<"\\begin{tabular}{|c|c|c|}"<<endl;
	out<<"\\textbf{C/R ratio} & NR $\\sigma_a$& WR$\\sigma_a$ \\\\\\hline"<<endl;

	for(double dilut: dilution) {
		sigD=dilut*C_nat_sig_t; //calculate the dilution cross-section
            out<<dilut<< " & "<<rocket.collapseXS_NR(lowE,upE,sigD,bins)
		    <<" & " <<rocket.collapseXS_WR(lowE,upE,sigD,bins)<<"\\\\\\hline"<<endl;

	}
	out<<"\\end{tabular}"<<endl;
	out.close();
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
	Q1C();	
	return 1;
}
