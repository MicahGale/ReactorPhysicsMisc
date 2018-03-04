#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <sstream>
#include <tr1/memory> //shared_ptr library
#include <stdlib.h>

static double getSquiggle();
#include "../lib/universe.h"

static double getSquiggle() {
        double squiggle=(rand()/((double)(RAND_MAX)+1));
        return squiggle;
}


int main() {
	return 0;
}
