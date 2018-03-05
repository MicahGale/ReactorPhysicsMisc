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

static universe worldBuild();
static double getSquiggle() {
        double squiggle=(rand()/((double)(RAND_MAX)+1));
        return squiggle;
}


int main() {
	universe shire=worldBuild();
	shire.randomWalk(1,5000);
	return 0;
}
static universe worldBuild() {
	std::vector<surface*> cell1surf,cell2surf,cell3surf,cell4surf;
	cell cell1, cell2,cell3,cell4;
	std::vector<cell> cells;
	//surfaces!
	xPlane midPlane(0.0);
	xPlane left(-2.0);
	xPlane right(4.0); 

	cell1surf.reserve(2);
	cell1surf.push_back(&midPlane);
	cell1surf.push_back(&left);

	cell2surf.reserve(2);
	cell2surf.push_back(&midPlane);
	cell2surf.push_back(&right);
	
	cell3surf.reserve(1);
	cell3surf.push_back(&left);
	cell4surf.reserve(1);
	cell4surf.push_back(&right);
	//materials!!
	std::vector<material> mat1 {material("",1,1/material::BARNS_TO_CM,0.5,0.5)};
	std::vector<material> mat2 {material("",1,1/material::BARNS_TO_CM,0.3,1.2)};
	std::vector<material> vacuum {material()}; //this is a vacuum material
	//tallies!
	std::vector<tally*> tallies(1);
	//source!!
	isoXLineSrc src(-2,0,20);
	source* nSrc= &src;

	cells.push_back(cell(cell1surf,std::vector<bool>(false,true),mat1,tallies));
	cells.push_back(cell(cell2surf,std::vector<bool>(true, false),mat2,tallies));
	cells.push_back(cell(cell3surf,std::vector<bool>(false),vacuum,tallies));
	cells.push_back(cell(cell4surf,std::vector<bool>(true),vacuum, tallies));
	
	return universe(cells,nSrc); 
}
