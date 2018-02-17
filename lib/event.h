
class event {
	public:
		double E;
		short int type;

		static const short int SCATTER=0xe7; //the constant for a scatter event
		static const short int ABSORB=0xff;  //the constant for an absorption event
		event() {
			this->E=0;
			this->type=0x00;
		}
		event(double E, int type) {
			this->E=E;
			this->type=type;
		}


};
