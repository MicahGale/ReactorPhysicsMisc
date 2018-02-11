#include <stdio.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>


//function prototypes
static double calcFaddeeva(double z);


int main()  {
	printf("hello world\n");
	calcFaddeeva(1.0);
	return 0;

}

static double calcFaddeeva(double z) {
	std::array<char, 128> buffer;
	std::string result;
	//convert input into string
	std::ostringstream strs;
	strs << z;
	std::string Zstr =  strs.str();

	std::string cmd = "python3 feddeeva.py "+Zstr;
	
	// runs command
	std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
	if(!pipe)
		throw std::runtime_error("popen() failed!");
	
	//continuously read the stdout
	while(!feof(pipe.get())) {
		if (fgets(buffer.data(), 128, pipe.get()) !=nullptr)
			result += buffer.data();
	}
}
