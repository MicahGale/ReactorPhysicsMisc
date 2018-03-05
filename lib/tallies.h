
class tally {
	public: 
	//	tally() {}
		virtual void doTally(const event& end, const vec& start)=0;
};

class collideTally: public tally {
	private:

	public:
		collideTally() {

		}
		void doTally(const event& end, const vec& start) {

		}
};

class trackTally: public tally {
	private:

	public:
		trackTally() {

		}
		void doTally(const event& end, const vec& start) {

		}
};
