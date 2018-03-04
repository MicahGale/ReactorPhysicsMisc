
class source {
	public:
		virtual event getNextNeutron();
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

class isoPntSrc: public source {
	private:
		vec pnt;

	public:
		event getNextNeutron() {
			//TODO implement
		}

};
class isoXLineSrc: public source {
	private:
		double xMax;
		double xMin;
		double E;

	public:
		event getNextNeutron() {
			vec point, dir;
			double x,y,z;

			dir=this->getIsoDir(); //find the direction
			x=(xMax-xMin)*getSquiggle()+xMin; //go within the x range
			y=rand()-RAND_MAX/2;  //just anywhere in space 
			z=rand()-RAND_MAX/2;
		        std::vector<double> ret {x,y,z};
			point=vec(ret);
			return event(this->sampleE(),event::NO_EVENT,point,dir);	
		}
		double sampleE() {
			return E;
		}

};
