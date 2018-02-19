#include <cmath>           //math things like sqrt
#include <math.h>            //Pi

//Thank you to the MIT Math Department for the Faddeeva implementation
//<http://ab-initio.mit.edu/wiki/index.php/Faddeeva_Package>
#include "../lib/Faddeeva.hh"
#include "../lib/event.h"    //event type
/**
 * Holds the material data for an isotope
 *
 */

//using namespace std;
class material {
	private:

	std::string name;
	double sigPot; //potential scattering
	double N;       //number density
	float alpha;   //cache the alpha term
	int A;
	//resonance data	
	std::vector<double> E0; //resonance energies
	std::vector<double> GG; //gamma for (n,gamma)
	std::vector<double> GN; //gamma for (n,n)
	double T;

	//calculated resonance info
	std::vector<double> G;  //gamma total
	std::vector<double> r; 
	std::vector<double> q;
	std::vector<double> SLBW_squiggle;
	public:
	/**
	*Boltzmann Constant taken from: <https://en.wikipedia.org/wiki/Boltzmann_constant>
	*In units of eV/k
	*/
	static double BOLTZ_K(){return 8.6173303e-5;}
	/**
	 *Constructor 
	 *
	 */
	material() {

	}
	material (std::string name, int A, double N, double sigPot) {
		int top, bottom;
		this->name=name;
		this->A=A;
		top=A-1;
		bottom=A+1; //calculation top and bottom terms of alpha
		this->alpha=top*top/(bottom*bottom);
		this->sigPot=sigPot;
		this->N=N;
	}
	
	void initResonance(std::vector<double> E0, std::vector<double> GG, std::vector<double> GN) {
		this->E0=E0;
		this->GG=GG;
		this->GN=GN;
		this->G.reserve(E0.size());
		this->r.reserve(E0.size()); //initialize vectors before using them
		this->q.reserve(E0.size());
		for(int i=0; i<GG.size(); i++) {
			this->G.push_back(GG[i]+GN[i]);
			this->r.push_back(2603911/E0[i]*(this->A+1)/this->A); //calculate r
			this->q.push_back(2*sqrt(r[i]*sigPot));     //2sqrt(r*sig_pot)
		}
	}
	double getMacroSigP(double E, double T) {
		return this->sigPot*this->N;
	}
	double updateN(double N) {
		this->N=N;
	}
	/**
	 *Calculates the sigma_s with the SLBW.
	 * excludes the potential term.
	 *
	 * @param E the energy to evaluate
	 * @param T the temperature to evaluate
	 * @return the resonant contribution to sigma_s in barns
	 */
	double getMicroSigSRes(double E, double T) {
		double sum=0;
		double gammaTerm, phi,chi;

		for(int i=0; i<E0.size(); i++) {
			phi=this->get_SLBW_phi(E,i);
			chi=this->get_SLBW_chi(E,i);
			gammaTerm=GN[i]*GN[i]/(G[i]*G[i]);
			sum+=gammaTerm*(r[i]*phi+G[i]/GN[i]*q[i]*chi);
			//calculates the SLBW resonance
		}
		return sum;
	}
	double getMicroSigS(double E, double T) {
		return this->getMicroSigSRes(E,T)+sigPot;
	}
       /**
	*Calculates the absorption cross-section using SLBW.
	*
	*@param E energy
	*@param T temperature
	*@return the full absorption cross-section
	*/	
	double getMicroSigA(double E, double T) {
		double sum=0; //the sum of all resonances
		double gammaTerm, phi;
		for(int i=0; i<E0.size();i++) {
			phi=this->get_SLBW_phi(E,i);
			gammaTerm=GN[i]*GG[i]/(G[i]*G[i]); // Gn/G* Gg/G
			sum+=gammaTerm*sqrt(E0[i]/E)*r[i]*phi; //calculate SLBW sig a
		}
		return sum;
	}

	double getMacroSigS(double E, double T) {
                return this->getMacroSigP(E,T)+
                        this->N*this->getMicroSigSRes(E,T);
        }

	double getMacroSigA(double E, double T) {
                return this->N*this->getMicroSigA(E, T);
        }

	double getMacroSigT(double E, double T) {
                return this->getMacroSigS(E, T)+this->getMacroSigA(E, T);
        }
	/**
	 * Performs a random walk 
	 *@param E- the incoming neutron energy
	 *@return an event with the energy and event type
	 */
	event randomWalk(double E, double T) {
		double total= this->getMacroSigT(E,T); //get the total cross-section
		double squiggle=getSquiggle();
	        event output;	
		//if it was scattered
		if(squiggle<= (this->getMacroSigS(E,T)/total) ) {
			squiggle=getSquiggle(); //decide on new energy now
			E=E*squiggle/((1-this->alpha)); //scatter to a whole new energy!
			output= event(E,event::SCATTER);
				
		} else  {  //otherwise assume absorbed. TODO implement fission if needed
			output=event(0, event::ABSORB);
		}
		return output;	
	}
	//################################SLBW Section#################################
        /**
	 *prepares squiggle for doppler broadening
	 *  
	 */
        double set_SLBW_temperature(double T) {
 	       SLBW_squiggle.reserve(E0.size()); //allocate that space
	       for(int i=0; i<E0.size();i++) {
		       if(T>1) {
			       SLBW_squiggle[i]=G[i]*sqrt(A/(4*BOLTZ_K()*T*E0[i]));
		       		std::cout<<SLBW_squiggle[i]<<std::endl;
		       } else
			       SLBW_squiggle[i]=1;
	       }
	       this->T=T;
	}

	private:
	double get_SLBW_x(double E, int resPointer) {
		if(resPointer>E0.size()-1) //if out of bounds have none of that
			return 0;
		return 2*(E-E0[resPointer])/G[resPointer];  //2(E-E0)/Gamma
	} 
	double get_SLBW_phi(double E, int resPointer) {
		double x;
		x=this->get_SLBW_x(E,resPointer);
		if(abs(T)<0.01) {  //if its really small then it's 0K.
			return 1/(1+x*x); //1\(1+x^2)
		} else {
			//screw doppler broadening
			double squiggle=SLBW_squiggle[resPointer];
			std::complex<double>in (x*squiggle,squiggle);
			std::complex<double> out= Faddeeva::w(in);
			out=out*squiggle;   
			return sqrt(M_PI)*out.real();
		}
	}	
	double get_SLBW_chi(double E, int resPointer) {
		double x;
		x=this->get_SLBW_x(E, resPointer);
		
		if(abs(T)<0.01) {  //do the 0k!!!
			return x/(1+x*x);  //  x/(1+x^2)
		} else {
			double squiggle=SLBW_squiggle[resPointer];
			std::complex<double> in (x*squiggle, squiggle);
			std::complex<double> out=Faddeeva::w(in);
			out=squiggle*out;
			return sqrt(M_PI)*out.imag();
		}
	}
};
