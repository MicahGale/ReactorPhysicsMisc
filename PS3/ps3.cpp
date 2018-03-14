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

double getSquiggle();
double trapInt(std::vector<double> & vals, double stepSize);
#include "../lib/mathVec.h"
#include "../lib/materials.h"

using namespace std;

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
void Q1C() {
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
	bins=1e6;

	//file stuff
	out.open("Q1Cxs.tex",ios::trunc);  //opens it
	out<<"\\begin{tabular}{|c|c|c|}"<<endl;
	out<<"\\hline \\textbf{C/R ratio} & NR $\\sigma_a$& WR$\\sigma_a$ \\\\\\hline"<<endl;

	for(double dilut: dilution) {
		sigD=dilut*C_nat_sig_t; //calculate the dilution cross-section
            out<<dilut<< " & "<<rocket.collapseXS_NR(lowE,upE,sigD,bins)
		    <<" & " <<rocket.collapseXS_WR(lowE,upE,sigD,bins)<<"\\\\\\hline"<<endl;

	}
	out<<"\\end{tabular}"<<endl;
	out.close();
}
void Q2() {
	rocketonium rocket;
	double sigD,lowE,upE;
	int bins;

	lowE=1;
	upE=5e3;
	bins=1e6;

	sigD=1259.9; //[b]

	cout<<"SigD"<<sigD<<" NR: "<<rocket.collapseXS_NR(lowE,upE,sigD,bins)
		<<" WR: "<<rocket.collapseXS_WR(lowE,upE,sigD,bins)<<endl;
}
void Q3() {
	vector<double> Temp, E0,GG,GN,RIval,flux;
	vector<vector<double>> bounds;
	double stepSize, RI;
	material uran;
	ofstream out; 
	int bins;

	bins=500;
	Temp={300,1000};
	bounds={{6,10},{10,25},{25,50}};
        E0= {6.673491, 20.87152,36.68212}; //alll units in eV
        GG ={0.02300000, 0.02286379,0.02300225};
        GN ={0.001475792, 0.01009376,0.03354568};
	
	out.open("Q3RI.tex",ios::trunc);
	out<<"\\begin{tabular}{|c|c|c|c|c|c|c|}"<<endl;
	out<<"\\hline \\textbf{Temp (K)}& \\multicolumn{2}{|c|}{\\textbf{6-10eV}} &\\multicolumn{2}{|c|}{\\textbf{10-25eV}} & \\multicolumn{2}{|c|}{\\textbf{25-50eV}}\\\\\\hline  " <<endl;
	out<<"&\\textbf{RI} & \\textbf{XS} &\\textbf{RI} & \\textbf{XS} &\\textbf{RI} & \\textbf{XS} \\\\\\hline"<<endl;
	for(double T: Temp) {
		uran=material("U-238",238,1e24,5.0,0,E0,GG,GN,T); //doppler broaden the problem.
		out<<T; //write the rows temperature
		for(vector<double> limits: bounds) {
			stepSize=(limits[1]-limits[0])/bins; //get that step size
			RIval={};
			flux={};
			for(double E=limits[0];E<=limits[1];E+=stepSize) {
				RIval.push_back(1/E*uran.getMicroSigA(E)); //calc infin dilute
				flux.push_back(1/E);
			}
			RI=trapInt(RIval,stepSize);
			out<<" & "<<RI<<" & "<<RI/trapInt(flux,stepSize);
		}
		out<<"\\\\\\hline"<<endl;
	}
	out<<"\\end{tabular}"<<endl;
	out.close();
}

void Q4() {
        vector<double> dilute, E0,GG,GN,RIval,flux,output;
	vector<short int> resModel;
        vector<vector<double>> bounds;
        double stepSize, RI,lambda;
        material uran;
        ofstream out;
        int bins;

        bins=50000;
	lambda=0.5;
        dilute={20000,200,20};
	resModel={material::NR,material::WR,material::IR};
        bounds={{6,10},{10,25},{25,50}};
        E0= {6.673491, 20.87152,36.68212}; //alll units in eV
        GG ={0.02300000, 0.02286379,0.02300225};
        GN ={0.001475792, 0.01009376,0.03354568};

        out.open("Q4.tex",ios::trunc);
        out<<"\\begin{tabular}{|c|c|c|c|c|c|c|}"<<endl;
        out<<"\\hline \\textbf{Dilution (b)}& \\multicolumn{2}{|c|}{\\textbf{6-10eV}} &\\multicolumn{2}{|c|}{\\textbf{10-25eV}} & \\multicolumn{2}{|c|}{\\textbf{25-50eV}}\\\\\\hline  " <<endl;
        out<<"&\\textbf{RI} & \\textbf{XS} &\\textbf{RI} & \\textbf{XS} &\\textbf{RI} & \\textbf{XS} \\\\\\hline"<<endl;
	uran=material("U-238",238,1e24,5.0,0,E0,GG,GN,300); //doppler broaden the problem.
	for(double sigD: dilute) {
		for(short int res: resModel) {
			out<<sigD<<" ";
			if(res==material::NR)
				out<<"NR ";
			if(res==material::WR)
				out<<"WR ";
			if(res==material::IR)
				out<<" IR ";
			for(vector<double> bound: bounds) {
				output=uran.collapseXS(res,bound[0],bound[1],sigD,lambda,bins);
				out<<" & "<< output[0]<<" & "<<output[1];
			}
			out<<"\\\\\\hline"<<endl;

		}
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
//	Q1C();
//	Q2();
//	Q3();
	Q4();	
	return 1;
}
