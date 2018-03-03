
/**
*An object for handling mathematical vectors.
*
*
*/
class vec {
	private:
		std::vector<double> data;
	public:
		vec(std::vector<double> vector) {
			data=vector;
		}
		/**
		*Returns the ith component of the vector.
		*
		*@Param i the index to retrieve
		*@Return the component which was requested.
		*/
		double get(int i) {
			return data[i];
		}
		int size() {
			return data.size();
		}
		static double dotProduct(vec a, vec b) {
			double ret=0;
			for( int i =0; i<a.size();i++) {
				ret+=a.get(i)*b.get(i);
			}
			return ret;
		}

};
