//All units are in cm for distance
 class surface {
	public:
		virtual vec findIntercept(const event& start)=0;
		virtual bool findSide(const vec point)=0;
		virtual void helloWorld()=0;
};

class xPlane: public surface {
	private:
		double x;
	public:
		/**
		 *Creates an YZ-plane at x.
		 *
		 *@param x the x-intercept
		 */
		xPlane(double x) {
			this->x=x;
		}
		/**
		 *Finds where this event will intercept with the surface.
		 *
		 */
		vec findIntercept(const event& start) {
			double t;
			vec point, dir,ret;

			point=start.getPoint();
			dir=start.getDir();
			t=x-point.get(0); //find x displacement
			if(t>0==dir.get(0)>0) {  //if the neutron is headed for the plane get t
				//the intercept
				t=t/dir.get(0); //find how many path lengths to intercept
				ret=point+t*dir; //gets the intercept
			} else {
				throw 20;
			}
			return ret;
		}
		/**
		 * Determines which side of the plane the vector is on.
		 *
		 * @return false- on the negative infinity side. True- Positive infinity side
		 *
		 */
		bool findSide(const vec point) {
			double xEval=point.get(0); //get the x point
			return xEval>x;
		}
		void helloWorld() {

		}

};
