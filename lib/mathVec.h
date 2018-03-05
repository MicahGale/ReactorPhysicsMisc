#include <cmath>
/**
*An object for handling mathematical vectors.
*
*
*/
class vec;
vec operator+ (const vec& a, const vec& b);
vec operator- (const vec& a, const vec& b);
vec operator* (double x, const vec& b);
class vec {
	private:
		std::vector<double> data;
	public:
		/**
		 * Constructor
		 *
		 */
		vec() {}
		vec(std::vector<double> vector) {
			data=vector;
		}
		/**
		*Returns the ith component of the vector.
		*
		*@Param i the index to retrieve
		*@Return the component which was requested.
		*/
		double get(int i) const {
			return data[i];
		}
		int size() const {
			return data.size();
		}
		double getMagnitude() const {
			double accum=0;
			for(double x:data) {
				accum+=x*x; //sum the square of the elements
			}
			return std::pow(accum,0.5); //take the square-root
		}
		vec getUnit() const {
			std::vector<double> newVec;
			double mag;

			mag=this->getMagnitude();
			for(double v: data) {
				newVec.push_back(v/mag);
			}
			
			return vec(newVec); //returns the new unit vector
		}
		static double dotProduct(const vec& a,const  vec& b) {
			double ret=0;
			for( int i =0; i<a.size();i++) {
				ret+=a.get(i)*b.get(i);
			}
			return ret;
		}
		/**
		 *Rotates the vector by a polar and azimuthal angle.
		 *
		 *@param mu- the polar angle
		 *@param phi - the azimuthal angle
		 *@return a new vector object that is now rotated.
		 */
		vec rotate(double mu, double phi) const {
			double x,y,z,u,v,w;
			if( this->size() !=3)
				throw 21;   //wrong size vector
			else {
				u=this->get(0);
				v=this->get(1);
				w=this->get(2);
				x=mu*get(0)+(std::pow(1-mu*mu,0.5)*(u*w*std::cos(phi)-
							v*std::sin(phi)))/(std::pow(1-w*w,0.5));
				y=mu*v+(std::pow(1-mu*mu,0.5)*(v*w*std::cos(phi)+u*std::sin(phi)))
					/(std::pow(1-w*w,0.5));
				z=mu*w-std::pow(1-mu*mu,0.5)*std::pow(1-w*w,0.5)*std::cos(phi);
			}
			std::vector<double> ret {x,y,z};
			return vec(ret);
		}
		static double getDistance(const vec& a, const vec& b) {
			vec c= a-b; //get the displacement
			return c.getMagnitude(); //and now the distance
		}
};

/**
 * Overloads the * operator to allow scalar multiplication
 *
 */
vec operator* (double x, const vec& y) {
	std::vector<double> z;
	
	for(int i=0; i<y.size();i++) {
		z.push_back(x*y.get(i)); //multiply the scalar
	}
	return vec(z);
}

/**
 *Overloads the + operator to allow vector addition
 *
 */

vec operator+ (const vec& a, const vec& b) {
	std::vector<double> c;
	
	for(int i=0;i<a.size();i++) {
		c.push_back(a.get(i)+b.get(i)); //add each component together
	}
	return vec(c);
}
vec operator- (const vec& a, const vec& b) {
	return a+-1*b; //why redefine the wheel?
}

