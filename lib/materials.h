#include "../lib/resonance.h"
/**
 * Holds the material data for an isotope
 *
 */

using namespace std;
class material {
	private:

	string name;
	double sigPot; //potential scattering
	double N;       //number density
	
	private:
	/**
	 *Constructor 
	 *
	 */	
	material (double N, double sigPot) {
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

};
