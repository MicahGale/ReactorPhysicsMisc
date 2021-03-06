
class tally {
	public: 
	//	tally() {
		virtual void doTally(const event& end, const vec& start,double sigT,
			       	const material& mat)=0;
		virtual void flushTally()=0;
		virtual std::vector<std::vector<double>> getData(double W)=0;
};
class meshTally: public tally {
	protected: 
		std::vector<double> meshBounds;
		double stepSize;
	public:
	//	virtual void doTally(const event& end, const vec& start, double sigT, double sigT,
	//		const material& mat)=0; //kick the can down the road
	//	virtual void flushTally()=0;
		/**
		 *generates a uniform x mesh over the given bounds.
		 *
		 */
		void genMesh(double xMin, double xMax,int bins) {

			stepSize=(xMax-xMin)/((double)(bins)); //find the size of the bins
			meshBounds.reserve(bins+1); //grow to size
			//march through all bounds
			for(double val=xMin;val<=xMax;val+=stepSize) {
				meshBounds.push_back(val);  //build the vector
			}
		}
		/**
		 *Determines which mesh grid the point lies in.
		 *
		 * A point is in a mesh bin i if mesh_i<pnt_x<mesh_i+1 
		 */
		int findMesh(const vec& pnt) {
			double x;

			x=pnt.get(0); //get the x value
			for(int i=0;i<meshBounds.size();i++) {
				if(x<=meshBounds[i]) {
					return i-1;
				}
			}
			return -1;
		}
};
class collideTally: public meshTally {
	private:
		std::vector<double> buffer;
		std::vector<double> mean;
		std::vector<double> meanSquare;	
	public:
		collideTally(){}
		collideTally(double xMin, double xMax, int bins) {
			this->genMesh(xMin, xMax,bins);
			buffer = std::vector<double>(bins,0);
			mean= std::vector<double>(bins,0);
			meanSquare=std::vector<double>(bins,0); //initialize all of the data holders
		}
		void doTally(const event& end, const vec& start,double sigT,
				const material& mat) {
			int bin;
			short int type;

			type=end.getType();
			if(type!=event::NO_EVENT&&type!=event::LEAK) { // check this was a collsion
				//end.print();
				bin=this->findMesh(end.getPoint());
				buffer[bin]+=end.getW();  //add the weight to the buffer
				//wow that's straight forward
			}
			
		}
		void flushTally() {
			for(int i=0; i<buffer.size();i++) {
				//std::cout<<"Tally: "<<i<<" Value: "<<mean[i]<<std::endl;
				mean[i]+=buffer[i]; //move the mean over
				meanSquare[i]+=buffer[i]*buffer[i];
				buffer[i]=0; //clear it out for next one
			}
		}
                std::vector<std::vector<double>> getData(double N) {
                        this->flushTally();
                        std::vector<std::vector<double>> ans;
                        ans.resize(meshBounds.size()-1);  //setup the size

                        for(int i=0;i<meshBounds.size()-1;i++) {
                                ans[i].resize(3);  //sqwoosh it! screw spelling! viva la caffiene!
                                ans[i][0]=(meshBounds[i]+meshBounds[i+1])/2; //the spatial part!
                                ans[i][1]=mean[i]/N; //get the mean
                                ans[i][2]=pow(meanSquare[i]/(N)-mean[i]*mean[i]/(N*N),0.5);

                        }
                        return ans;
                }

};

class trackTally: public meshTally {
	private:
		std::vector<double> buffer;
		std::vector<double> mean;
		std::vector<double> meanSquare;

	public:
		trackTally(){}
		trackTally(double xMin, double xMax, int bins) {
                        this->genMesh(xMin, xMax,bins); //gen the mesh
                        buffer = std::vector<double>(bins,0);
                        mean= std::vector<double>(bins,0);
                        meanSquare=std::vector<double>(bins,0); //initialize all of the data holder
		}
		void doTally(const event& end, const vec& start, double sigT,
				const material& mat) {
			double dis,xDis,botX,topX,W;
			int bot, top,startMesh,endMesh;
			vec displacement;

			W=end.getW()*sigT; //add sigma_T to get that stuff
			dis=vec::getDistance(end.getPoint(),start); //get the flight distance
			displacement=end.getPoint()-start;
			xDis=fabs(displacement.get(0));  //project onto the x axis
			startMesh=this->findMesh(start); //find the limits
			endMesh=this->findMesh(end.getPoint()); //find the limits
			if(startMesh<endMesh) { //if going left to right
				bot=startMesh;
				top=endMesh;
				botX=start.get(0);
				topX=end.getPoint().get(0);
			} else  {  //otherwise going right to left
				bot=endMesh;
				top=startMesh;
				botX=end.getPoint().get(0);
				topX=start.get(0);
			}
			if(top==bot) {  //if only in one mesh grid
				buffer[bot]+=W*dis; //just add the whole path length to that grid
			} else {
				buffer[bot]+=W*dis*(meshBounds[bot+1]-botX)/xDis; 
				//add the edge contribution

				for(int i=bot+1;i<top;i++) {  
					//fill in all of the mesh points in between
					buffer[i]+=W*dis*stepSize/xDis;
				}
				buffer[top]+=W*dis*(topX-meshBounds[top])/xDis;
			       	//add top edge contribution
			}


		}
                void flushTally() {
                        for(int i=0; i<buffer.size();i++) {
				//std::cout<<"Tally: "<<i<<" Val: "<<buffer[i]<<std::endl;
                                mean[i]+=buffer[i]; //move the mean over
                                meanSquare[i]+=buffer[i]*buffer[i];
                                buffer[i]=0; //clear it out for next one
                        }
                }
                std::vector<std::vector<double>> getData(double N) {
                        this->flushTally();
                        std::vector<std::vector<double>> ans;
                        ans.resize(meshBounds.size()-1);  //setup the size

                        for(int i=0;i<meshBounds.size()-1;i++) {
                                ans[i].resize(3);  //sqwoosh it! screw spelling! viva la caffiene!
                                ans[i][0]=(meshBounds[i]+meshBounds[i+1])/2; //the spatial part!
                                ans[i][1]=mean[i]/N; //get the mean
                                ans[i][2]=pow(meanSquare[i]/(N)-mean[i]*mean[i]/(N*N),0.5);

                        }
                        return ans;
                }
};
