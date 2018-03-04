
class source {
	public:
		virtual event getNextNeutron();
};

class isoPntSrc: public source {
	private:
		vec pnt;

	public:
		event getNextNeutron() {
			//TODO implement
		}

};
