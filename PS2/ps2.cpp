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
static void dumpData(universe& shire, int N);
static double getSquiggle() {
        double squiggle=(rand()/((double)(RAND_MAX)+1));
        return squiggle;
}


int main() {
	universe shire=worldBuild();
	//shire.setSeed(132181200);
	//shire.randomWalk(100,1);
	int neutrons=(int)1e6;
	shire.randomWalk(neutrons,1);
	dumpData(shire,neutrons);
	return 0;
}
static void dumpData(universe& shire,int N) {
	std::vector<std::vector<double>>buffer;
	std::ofstream out;
	
	double w=(double)N;
	std::vector<int> cells {0,1};
	std::vector<int> tall {0,1};
	std::vector<std::string> fileName {"FluxByCollide.csv","FluxbyPath.csv"};
	for(int i=0;i<tall.size();i++) {
		out.open(fileName[i],std::ios::trunc); //open that shit!
		out<<"Space![x],Mean!,Std.Dev!!!"<<std::endl;
		
		for(int cell: cells) {
			buffer=shire.getTallyData(cell,i,w); //get the tally data
			for(std::vector<double> yup: buffer) {
				out<<yup[0]<<","<<yup[1]<<","<<yup[2]<<std::endl; //write that line!!
			}
		}
		out.close();
	}
}
static universe worldBuild() {
	std::vector<surface*> cell1surf,cell2surf,cell3surf,cell4surf;
	cell cell1, cell2,cell3,cell4;
	std::vector<cell> cells;
	//surfaces!
	double midPlane=0.0;
	double left = -2.0;
	double right=4.0; 

	cell1surf.reserve(2);
	cell1surf.push_back(new xPlane(midPlane));
	cell1surf.push_back(new xPlane(left));

	cell2surf.reserve(2);
	cell2surf.push_back(new xPlane(midPlane));
	cell2surf.push_back(new xPlane(right));
	
	cell3surf.reserve(1);
	cell3surf.push_back(new xPlane(left));
	cell4surf.reserve(1);
	cell4surf.push_back(new xPlane(right));
	//materials!!
	
	std::vector<material> mat1 {material("",1,1/material::BARNS_TO_CM,0.5,0.5)};
	std::vector<material> mat2 {material("",1,1/material::BARNS_TO_CM,0.3,1.2)};
	std::vector<material> vacuum { material()}; //this is a vacuum material
	//tallies!
	
	std::vector<tally*> tallies1,tallies2,empty;
	tallies1.reserve(2);
	tallies1.push_back(new collideTally(left,midPlane,10));
	tallies1.push_back(new trackTally(left,midPlane,10));

	tallies2.reserve(2);
	tallies2.push_back(new collideTally(midPlane,right,20));
	tallies2.push_back(new trackTally(midPlane,right,20));
	//source!!
	source* nSrc= new isoXLineSrc(-2,0,20);

	cells.push_back(cell(cell1surf,std::vector<bool>{false,true},mat1,tallies1,std::vector<int>{1,2}));
	cells.push_back(cell(cell2surf,std::vector<bool>{true, false},mat2,tallies2, std::vector<int>{0,3}));
	cells.push_back(cell(cell3surf,std::vector<bool>{false},vacuum,empty,std::vector<int>{0}));
	cells.push_back(cell(cell4surf,std::vector<bool>{true},vacuum, empty,std::vector<int>{1}));
	
	return universe(cells,nSrc); 
}
