#include <cmath>
#include "../lib/mathVec.h"
#include "../lib/materials.h"
#include "../lib/surfaces.h"
#include "../lib/tallies.h"

class cell {
	private:
		std::vector<surface*> surfaces;
		//std::vector<std::tr1::shared_ptr<surface>> surfaces;
		std::vector<material> materials;
		std::vector<tally*>   tallies;
		std::vector<int>      neighbors;
		std::vector<bool> side; //which side the cell is on for all surfaces
	public:
		/**
		 *Creates a cell object that is defined by the surface and materials.
		 *
		 * A Cell is defined a homegenous spatial region. The cell is defined by 
		 * multiple surfaces which you must specify on which side of the surface the cell
		 * lies. The cell is the union of these regions. The materails for the region
		 * are as specified. Tallies may be specified in which they will be called for 
		 * every history in that cell.
		 *
		 * @param surfaces -the surfaces defining the cell
		 * @param side    - defines which side of each surface the cell is on. False is
		 * 		    the negative side, and true the positive
		 * @param materials -all isotopes in the cell
		 * @param tallies  - the tallies to be completed with every neutron history. Is pass
		 * 			by reference, and will be mutated!
		 *
		 */
		cell(){}
		cell(const std::vector<surface*> &surfaces,const std::vector<bool> &side, 
				std::vector<material> materials,std::vector<tally*> &tallies,
				std::vector<int> neighbors) {

			this->surfaces=surfaces;
			this->materials=materials;
			this->tallies=tallies;
			this->side=side;
			this->neighbors=neighbors;

		}

		std::vector<int> getNeighbors() {
			return this->neighbors;
		}
		/**
		 *Determines if this cell is a vacuum.
		 *
		 * @return true iff the cell contains at least one vacuum material
		 */
		bool isVac() {
			bool isVac=false;

			for(int i=0; i<materials.size();i++) {
				if(materials[i].isVac()) { //if any material is vacuum return true
					isVac=true;
					//return isVac;
				}
			}
			return isVac;
		}
		/**
		 * Determines if this point is inside of the cell.
		 *
		 * @param point - the point to be evaluated at.
		 * @return true if and only if the point is inside the cell
		 */
		bool findInOut(const vec& point) {
			bool inCell=true;
			surface* ptr;

			//test every surface
			for(int i=0; i<surfaces.size();i++) {
				//if on the wrong side it's not inside
				ptr=surfaces[i];
				try{
					if(ptr->findSide(point)	!=side[i]) {
						inCell=false;
					}
				} catch( int e) {
					//silently catch the exception 5
					//this means the point is on the surface
					//can assume is in the cell then
				}
			}
			return inCell;
		}
		vec findIntercept(const event& start) {
			vec intercept;
			for(surface* ptr: surfaces) {
				try{
					intercept= ptr->findIntercept(start);
					if( this->findInOut(intercept)
				/*	&& vec::getDistance(intercept,start.getPoint())>1e-9*/) {
						std::cout<<intercept.print()<<std::endl;
						return intercept;
					} //if in the cell it's probably the right one
					//TODO implement more rigurous checks if we do circly things
				}
				catch (int e) {
					//lol woops not a valid intercept
				}
			}
			//if we got to this point there were no intercepts... throw an error
			throw "hi";
			throw 20;
		}
		/**
	 	*Calculates the total Sigma_T for the cell
	 	*/	
                double getMacroMacroSigT( double E) {
                        double sum =0; 
                        for(material mat: materials) {
                                sum+=mat.getMacroSigT(E);
                        }
			return sum;
                }
                /**
                 * *based on cross-sections decide which material to scatter off of
                 * *
                 * *@param materials, all of the materials in this problem
                 * *@param E the energy of the current neutron
                 * *@return the index of the material which was randomly chosen
                 * */
                 int selectMat(double E) {
                        double squiggle=getSquiggle();
                        double total = 0;
                        double accum = 0;
                        //find the sum of all of the cross sections
                        total=getMacroMacroSigT(E);
                         //run through it again with to find which one is right
                        for(int i=0;i<materials.size(); i++) {
                                accum+=materials[i].getMacroSigT(E)/total;   
                                //adds this materials fraction of it
                                 if(squiggle<=accum) {
                                         return i; //if the material is the selected one return it 
                                 }       
                         }       
                        //if all else fails default to 0
                        return 0;
                }
		 /**
		  *Runs all of the tallies to see if they need to be updated.
		  */
		 void doTallies(const event& end, const vec& start, double SigT, int Mat) {
			
			 
			 for(tally* ptr: tallies) {
				ptr->doTally(end, start,SigT, materials[Mat]);
			 }
		 }
		 /**
		  *Flushes the tallies for this neutron history.
		  *
		  * Must be called with every neutron death on all cells.
		  */
		 void flushTallies()  {
			for(tally* ptr: tallies) {
				ptr->flushTally();
			}
		 }
		 int getTallySize() {
			 return tallies.size();
		 }
		 std::vector<std::vector<double>> getTallyData(int i,double W) {
			return tallies[i]->getData(W);
		 }	
		 /**
		  *Does the Monte Carlo walk through the cell. Updating tallies as needed
		  *
		  *@param start. The entering event
		  *@return the event where the neutron exits the cell or dies.
		  */
		event walkRandomly(const event& start) {
			double x, dis,E,W;
			vec intercept,pnt,dir,newPnt;
			event finish,startIntern;
			bool inCell;
			int mat;

			inCell=true;
			finish=start;
			startIntern=start;
			E=start.getE();
			W=start.getW();
			pnt=start.getPoint();
			dir=start.getDir();
			//if this cell is a vacuum just leak the neutron asap
			if(this->isVac()) {
				finish=event(E,W,event::LEAK,pnt,dir);
				this->doTallies(finish,pnt,this->getMacroMacroSigT(E),0);
				return finish;
			}
			//while the neutron is in this cell
			while(inCell) {
				try{
					intercept=this->findIntercept(finish);
					dis=vec::getDistance(pnt,intercept); 
					//find distance to intercept
					x=-std::log(getSquiggle())/this->getMacroMacroSigT(E); 
					//find MC distance traveled.
					if(x<dis) {
						mat= selectMat(E);
						finish=materials[mat].randomWalk(startIntern); 
						//do the monte Carlo
						if(finish.getType()==event::ABSORB||
								finish.getType()==event::LEAK)
						{ 
							//if the neutron died
							inCell=false;
						}
						this->doTallies(finish,pnt,
 							this->getMacroMacroSigT(E),mat);
						startIntern=finish; //update for next round
						pnt=startIntern.getPoint();		
					} else { //if it exited the cell.
						finish= event(E,W,event::NO_EVENT,intercept,dir);
						inCell=false;
					}
					finish.print();
				} catch (int e) {
					std::cout<<"It happened: ";
					finish.print();
				}
			}
			this->doTallies(finish,pnt,this->getMacroMacroSigT(E),mat);

			return finish;

		}

};
