#include <stdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>


int main()  {
	printf("hello world");
	return 0;

}

static double calcFaddeeva(double z) {
	std::array<char, 128> buffer;
	std::string result;
	//convert input into string
	std:ostringstream strs;
	strs << z;
	std::string str =  strs.str();
	
	// runs command
	std::shared_ptr<FILE> pipe(popen("python3 bleeh.py", "r"), pclose);
	if(!pipe)
		throw std::runtime_error("popen() failed!");
	
	//continuously read the stdout
	while(!feof(pipe.get())) {
		if (fgets(buffer.data(), 128, pipe.get()) !=nullptr)
			result += buffer.data();
	}
}
