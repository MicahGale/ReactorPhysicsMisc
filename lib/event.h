
class event {
	private:
		double E;
		short int type;
		vec point;  //where the neutron is
		vec direction; //where it's going
	public:
		static const short int SCATTER=0xe7; //the constant for a scatter event
		static const short int ABSORB=0xff;  //the constant for an absorption event
		static const short int NO_EVENT=0x54; //the constant for leaving a cell
		static const short int LEAK=0xf7;     //the neutron leaked out 
		event() {
			this->E=0;
			this->type=0x00;
		}
		/**
		 *Tracks a nuetron.
		 *
		 * Stores the position, direction, and energy, and last interaction type.
		 * The direction is always converted to a unit vector when stored.
		 *
		 */
		event(double E, int type,const  vec& point, const vec& direction) {
			this->E=E;
			this->type=type;
			this->point=point;
			this->direction=direction.getUnit();
		}
		double getE() const {
			return E;
		}
		int getType() const {
			return type;
		}
		vec getPoint() const {
			return point;
		}
		vec getDir() const {
			return direction;
		}



};
