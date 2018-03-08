#include "../lib/cell.h"
#include "../lib/sources.h"

class universe {
	private:
		std::vector<cell> cells;
		source* nSource;
		int SEED;

	public:
		universe(std::vector<cell> cells,
			       	source* nSource) {
			this->cells=cells;
			this->nSource=nSource;
		}
		int findCell(const vec& pnt) {
			for(int i=0;i<cells.size();i++) { //test all cell
				//std::cout<<"Cell: "<<i<<std::endl;
				if( cells[i].findInOut(pnt)) {
					return i; //if it's in this cell give up you found it
				}
			}
			return -1; //well your universe is screwed. Prepare for apocolypse
		}
		void setSeed(int seed) {
			SEED=seed;
		}
		int findCell(const vec& pnt, const std::vector<int>& neighbor) {
			for(int neigh: neighbor) {
				if(cells[neigh].findInOut(pnt)) {
					return neigh;
				}
			}
			return this->findCell(pnt);
		}

		void flushTallies() {
			for( cell buf: this->cells) {
				buf.flushTallies();
			}
		}
		int getCellSize() {
			return cells.size();
		}
		std::vector<std::vector<double>> getTallyData(int i, int j,double W) {
			return cells[i].getTallyData(j,W);
		}
		void randomWalk(int BatchSize, int Batches) {
			bool isAlive;
			event history;
			int cell;
			double W;
			std::vector<int> neighbors;

			//run the batches
			W=1/((double)(BatchSize)); //start off with the tallies being per start neutron
			for(int batchCnt=0;batchCnt<Batches;batchCnt++) {
				//run a batch
				for(int ntrns=0; ntrns<BatchSize;ntrns++) {
					isAlive=true;
					//jump 100 random numbers for every neutron
					srand((batchCnt*BatchSize+ntrns)*100+SEED); 
					history=nSource->getNextNeutron(W);//start a neutron
					//clear out the old nearest neighbor
					neighbors= std::vector<int>(0);
				       if(ntrns%100==0) 
				       {	
						std::cout<<"Neutron: "<<ntrns
							<<" Batch: "<<batchCnt<<std::endl;
					}
					while(isAlive) {
						cell=this->findCell(history.getPoint(),neighbors);
						//std::cout<<"Cell: "<<cell<<std::endl;
						//find where you are
						history=this->cells[cell].walkRandomly(history);
						if(history.getType()==event::ABSORB||
								history.getType()==event::LEAK) {
							isAlive=false; //kill it
						}
						neighbors=cells[cell].getNeighbors();
					}
					//flush the tallies after everything died
					this->flushTallies();
					//TODO figure out fission banking		
				}
			}

		}
		
		
};
