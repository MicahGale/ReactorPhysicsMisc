
class tally {
	public: 
	//	tally() {
		virtual void doTally(const event& end, const vec& start,double sigT,
			       	const material& mat)=0;
		virtual void flushTally()=0;
};

class collideTally: public tally {
	private:
		vector<double> buffer;
	        vector<double> mean;
		vector<double> meanSquare;
		vector<double> meshBounds;	
	public:
		collideTally() {

		}
		void doTally(const event& end, const vec& start,double sigT,
				const material& mat) {

		}
		void flushTally() {
			for(int i=0; i<buffer.size();i++) {
				mean[i]+=buffer[i]; //move the mean over
				meanSquare[i]+=buffer[i]*buffer[i];
				buffer[i]=0; //clear it out for next one
			}
		}
};

class trackTally: public tally {
	private:
                vector<double> buffer;
		vector<double> mean;
		vector<double> meanSquare;
		vecotr<double> meshBounds;

	public:
		trackTally() {

		}
		void doTally(const event& end, const vec& start, double sigT,
				const material & mat) {

		}
                void flushTally() {
                        for(int i=0; i<buffer.size();i++) {
                                mean[i]+=buffer[i]; //move the mean over
                                meanSquare[i]+=buffer[i]*buffer[i];
                                buffer[i]=0; //clear it out for next one
                        }
                }

};
