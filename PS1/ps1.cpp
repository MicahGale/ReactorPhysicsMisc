#include <stdio.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>
#include <stdlib.h>   //srand()

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
static void walkRandomly() {
	srand(25);
	for(int i=0;i<25; i++) {
		cout<<getSquiggle()<<endl; //test the random generator
	}
}
/**
* *********************Main ******************
*
*/
int main()  {
	walkRandomly();
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
