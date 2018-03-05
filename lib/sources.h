
class source {
	public:
		source(){}
		virtual event getNextNeutron(double W)=0;
		
};
/*
class isoPntSrc: public source {
	private:
		vec pnt;

	public:
		event getNextNeutron() {
			//TODO implement
		}

};*/
class isoXLineSrc: public source {
	private:
		double xMax;
		double xMin;
		double E;

	public:
		isoXLineSrc(double xMin, double xMax,double E) {
			this->xMin=xMin;
			this->xMax=xMax;
			this->E=E;
		}
		/**
		 *Generates a neutron history to start off. 
		 *
		 * Samples x within the range specified by xMin and xMax.
		 * y and z are between -RAND_MAX/2 and RAND_MAX/2.
		 * The direction is isotropic.
		 *
		 * @param W- the initial neutron weight
		 */
		event getNextNeutron(double W) {
			vec point, dir;
			double x,y,z;

			dir=this->getIsoDir(); //find the direction
			x=(xMax-xMin)*getSquiggle()+xMin; //go within the x range
			y=rand()-RAND_MAX/2;  //just anywhere in space 
			z=rand()-RAND_MAX/2;
		        std::vector<double> ret {x,y,z};
			point=vec(ret);
			return event(this->sampleE(),W,event::NO_EVENT,point,dir);	
		}
		double sampleE() {
			return E;
		}
                vec getIsoDir() {
                        double mu, phi,x,y,z,xy;

                        mu=2*getSquiggle()-1;
                        phi=2*M_PI*getSquiggle();
                        z=mu;
                        xy=std::pow(1-mu*mu,0.5); //project onto the xy plane
                        x=std::cos(phi)*xy;
                        y=std::sin(phi)*xy;
                        std::vector<double> ret {x,y,z};
                        return vec(ret); //return the isotropic source
                }
};
