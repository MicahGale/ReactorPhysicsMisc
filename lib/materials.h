#include "../lib/resonance.h"
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
	int A;	
	public:
	/**
	 *Constructor 
	 *
	 */	
	material (std::string name, int A, double N, double sigPot) {
		this->name=name;
		this->A=A;
		this->sigPot=sigPot;
		this->N=N;
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
	double getAlpha() {
		return (this->A-1)^2/(this->A+1)^2;
	}
	/**
	 * Performs a random walk 
	 *@param E- the incoming neutron energy
	 *@return an event with the energy and event type
	 */
	event randomWalk(double E) {
		double total= this->getMacroSigT(E); //get the total cross-section
		double squiggle=rand();
	        event output;	
		//if it was scattered
		if(squiggle<= this->getMacroSigS(E) ) {
			squiggle=rand(); //decide on new energy now
			E=1/((1-this->getAlpha())*E); //scatter to a whole new energy!
			output= event(E,event::SCATTER);
				
		} else  {  //otherwise assume absorbed. TODO implement fission if needed
			output=event(0, event::ABSORB);
		}
		return output;	
	}

};
