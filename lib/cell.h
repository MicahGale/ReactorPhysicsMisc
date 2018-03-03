#include "../lib/materials.h"
#include "../lib/surfaces.h"

class cell {
	private:
		std::vector<surface> surfaces;
		std::vector<material> materials;
		std::vector<tally>   tallies;
	public:
		cell(std::vector<surfacce> surfaces, std::vector<material>* materials, std::vector<tally>* tallies) {

			this->surfaces=surfaces;
			this->material=material;
			this->tallies=tallies;

		}

};
