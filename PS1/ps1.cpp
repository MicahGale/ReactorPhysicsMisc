#include <stdio.h>
#include <iostream>
#include <fstream>   //for all the files
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <vector>
#include <sstream>
#include <stdlib.h>   //srand()
static double getSquiggle(); //declaring the prototype here to allow materials to acces it
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
	double squiggle=getSquiggle();
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
static int walkRandomly(vector<material> materials, int walks, string filename) {
	srand(25);  //seed the random number gen
	double energy, squiggle;
	int mat;	
	event interact;
	ofstream output;
	
	output.open(filename,ios::trunc); //opens the file and overwite old

	if(output.is_open()) {
			output<<"event number, energy (eV), scatter"<<endl;  //write the header of the file

		for(int i=0;i<walks; i++) {   //iterates over all the walks!!!
			energy=1e3;         //start off the neutron at the high energy
			for(int j=1; energy>=1; j++) { //run while the energy is high enough and keep track of number of events
				mat=selectMat(materials,energy); //find the right material
				interact=materials[mat].randomWalk(energy); //do the random walk
				cout<<interact.E<<" "<<interact.type<<endl;
			
				if(interact.type==event::SCATTER) {
					energy=interact.E;   //update the energy

					output<<j<<","<<energy<<","<<"1"<<endl;     //write the scatter event
				} else  { //assume absorption TODO fission here 
					energy=0; //tell it to kill this walk

					output<<j<<",0,0"<<endl;   //keep track of absorptions
				}
			}
		}
		output.close();
		return 1; //everything went alllright.... maybe
	} else {  // if the file can't be opened display an error
		cout<<"failed to open output file "+filename<<endl;
	       	return -1; //file failed	
	}
}
/**
* *********************Main ******************
*
*/

int Q2()  {
	material mod ( "Hydrogen-1", 1, 1.0, 20);
	vector<material> stuff(1,mod);
	return walkRandomly(stuff,10,"Q2flux.csv");
}
int main()  {
	if( Q2()== -1) 
		return -1; //run Q2 and die if it fails
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
