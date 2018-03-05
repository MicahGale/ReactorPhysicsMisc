#include "../lib/cell.h"
#include "../lib/sources.h"

class universe {
	private:
		std::vector<cell> cells;
		source* nSource;

	public:
		universe(std::vector<cell> cells,
			       	source* nSource) {
			this->cells=cells;
			this->nSource=nSource;
		}
		int findCell(const vec& pnt) {
			for(int i=0;i<cells.size();i++) { //test all cells
				if( cells[i].findInOut(pnt))
					return i; //if it's in this cell give up you found it
			}
			return -1; //well your universe is screwed. Prepare for apocolypse
		}

		void flushTallies() {
			for( cell buf: this->cells) {
				buf.flushTallies();
			}
		}
		void randomWalk(int BatchSize, int Batches) {
			bool isAlive;
			event history;
			int cell;
			double W;

			//run the batches
			W=1/BatchSize; //start off with the tallies being per start neutron	
			for(int batchCnt=0;batchCnt<Batches;batchCnt++) {
				//run a batch
				for(int ntrns=0; ntrns<BatchSize;ntrns++) {
					isAlive=true;
					//jump 100 random numbers for every neutron
					srand((batchCnt*BatchSize+ntrns)*100); 
					history=nSource->getNextNeutron(double W);//start a neutron
//					std::cout<<"Neutron: "<<ntrns<<" Batch: "<<batchCnt<<std::endl;
					while(isAlive) {
						cell=this->findCell(history.getPoint()); 
						//find where you are
						history=this->cells[cell].walkRandomly(history);
						if(history.getType()==event::ABSORB||
								history.getType()==event::LEAK) {
							isAlive=false; //kill it
						}
					}
					//flush the tallies after everything died
					this->flushTallies();
					//TODO figure out fission banking		
				}
			}

		}
		
		
};
