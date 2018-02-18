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
	std::vector<double> r;
	std::vector<double> E0; //resonance energies
	std::vector<double> GG; //gamma for (n,gamma)
	std::vector<double> GN; //gamma for (n,n)
	std::vector<double> G;  //gamma total
	public:
	/**
	 *Constructor 
	 *
	 */	
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
		for(int i=0; i<GG.size(); i++) {
			this->G[i]=GG[i]+GN[i];
			this->r[i]=2603911/E0[i]*(this->A+1)/this->A; //calculate r
		}
	}
	double getMacroSigP(double E) {
		return this->sigPot*this->N;
	}

	double getMicroSigSRes(double E) {
		return 0;
	} 
	double getMicroSigA(double E) {
		return 0;
	}

	double getMacroSigS(double E) {
                return this->getMacroSigP(E)+
                        this->N*this->getMicroSigSRes(E);
        }

	double getMacroSigA(double E) {
                return this->N*this->getMicroSigA(E);
        }

	double getMacroSigT(double E) {
                return this->getMacroSigS(E)+this->getMacroSigA(E);
        }
	/**
	 * Performs a random walk 
	 *@param E- the incoming neutron energy
	 *@return an event with the energy and event type
	 */
	event randomWalk(double E) {
		double total= this->getMacroSigT(E); //get the total cross-section
		double squiggle=getSquiggle();
	        event output;	
		//if it was scattered
		if(squiggle<= (this->getMacroSigS(E)/total) ) {
			squiggle=getSquiggle(); //decide on new energy now
			E=E*squiggle/((1-this->alpha)); //scatter to a whole new energy!
			output= event(E,event::SCATTER);
				
		} else  {  //otherwise assume absorbed. TODO implement fission if needed
			output=event(0, event::ABSORB);
		}
		return output;	
	}

};
