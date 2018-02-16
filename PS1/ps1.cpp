#include <stdio.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <vector>
#include <sstream>
#include <stdlib.h>   //srand()
#include "../lib/materials.h" //material object



//namespace
using namespace std;

//function prototypes
static double calcFaddeeva(double z);

/**
*Returns a double between 0 and 1
*which is uniformally distributed along the range.
*/
static double getSquiggle() {
	double squiggle=(rand()/((double)(RAND_MAX)+1));
	return squiggle;
}
/**
*based on cross-sections decide which material to scatter off of
*
*@param materials, all of the materials in this problem
*@param E the energy of the current neutron
*@return the index of the material which was randomly chosen
*/
static int selectMat(vector<material> materials, double E) {
	double squiggle=rand();
	double total = 0;
	double accum = 0;
	//find the sum of all of the cross sections
	for(int i=0; i<materials.size(); i++) {
		total+=materials[i].getMacroSigT(E);
	}
	//run through it again with to find which one is right
	for(int i=0;i<materials.size(); i++) {
		accum+=materials[i].getMacroSigT(E); //adds this materials fraction of it
		if(squiggle<=accum) {
			return i; //if the material is the selected one return it 
		}
	}
	//if all else fails default to 0
	return 0;
}
/**
 *Does a random Monte Carlo walk.
 *
 *@Param the materials for the array
 */
static void walkRandomly(vector<material> materials, int walks) {
	srand(25);  //seed the random number gen
	double energy, squiggle;
	
	for(int i=0;i<walks; i++) {   //iterates over all the walks!!!
		energy=1e3;         //start off the neutron at the high energy
		for(int j=0; energy>=1; j++) { //run while the energy is high enough and keep track of loops
			squiggle=rand();
		}
	}
}
/**
* *********************Main ******************
*
*/
int main()  {
	vector <material> materials;
	walkRandomly(materials, 10);
	//calcFaddeeva(1.0);
	return 0;

}

static double calcFaddeeva(double z) {
	array<char, 128> buffer;
	string result;
	//convert input into string
	ostringstream strs;
	strs << z;
	string Zstr =  strs.str();

	string cmd = "python3 feddeeva.py "+Zstr;
	
	// runs command
/*	shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
	if(!pipe)
		throw runtime_error("popen() failed!");
	
	//continuously read the stdout
	while(!feof(pipe.get())) {
		if (fgets(buffer.data(), 128, pipe.get()) !=nullptr)
			result += buffer.data();
	}*/
}
