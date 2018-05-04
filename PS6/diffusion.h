/*
 * The library for a multi-group 1D diffusion solver
 */
#include <vector>

/**
 * Sums all of the elements of a 1D array
 *
 * @param summee the array to be summed.
 * @return the sum of the entire array
 */
double sumArray(const std::vector<double> &summee) {
    double sum=0;
    for(double buff: summee) {
        sum+=buff;
    }
    return sum;
}
int sumArray(const std::vector<int> &summee) {
    int sum=0;
    for(int buff:summee) {
        sum+=buff;
    }
    return sum;
}

/**
 *Builds 2D vector of material properties for every mesh point.
 *
 *This vector can then be fed into the matrix builder functions.
 *
 *@param cellSpec, the material properties in each physical cell level 1-cell
                    level 2- groups
                    level 3 properties
                        0-D
                        1-Sigma_a
                        2-Sigma_upscat
                        3-Sigma_downscat
                        4-chi
                        5-nu-Sigma_f
 *@param cellThick the thickness in unit mesh cells for each spacial region above
 *@param delta     the thickness in cm of each mesh grid
 *
 *@return the top level of the vector is each material the lower level mimicks cellSpec
 *
 */

std::vector<std::vector<std::vector<double>>> buildPropArray(
        const std::vector<std::vector<std::vector<double>>> &cellSpec, 
        const std::vector<int> &cellThick) {
    int total,groups,outI;
     
    total=sumArray(cellThick);
    groups=cellSpec[0].size(); //get the number of groups being used
    std::vector<std::vector<std::vector<double>>> ret
        (total,std::vector<std::vector<double>>(groups,
            std::vector<double>(6))); //init the return vector

    outI=0;
    for(int i =0; i<cellSpec.size(); i++) { //iterate over all cells
        for(int j=0;j<cellThick[i];j++) { //count over all of the meshpoints in a cell
            ret[outI]=cellSpec[i];  //copies in the material properties for that cell
            outI++; //move onto the next mesh point
        }
    }
    return ret;
}

/**
 * Creates the H matrix for solving H\phi=F\phi+S.
 *
 * Assumes that fluxes are in order by group the space. So 1g1, 1g2, 2g1, ....
 *
 * @param leftBound, the left boundary (0 side) condition. True= vacuum,
 * false=reflective
 * @param righBouund same but for the right side.
 *@return the H matrix assumes first dimension is row. 
 */

std::vector<std::vector<double>> generateH (std::vector<
        std::vector<std::vector<double>>> propArray, double delta,
        bool leftBound, bool rightBound) {
    int cells, groupsi,cellTarget;
    double DtildaMinus, DtildaPlus,sigmaRemoval;

    cells=propArray.size(); //gets the number mesh cells
    groups=propArray[0].size(); //gets the number of groups in model

    std::vector<std::vector<double>> ret(groups*cells,std::vector<double>(
                groups*cells,0)); //creates square return matrix

    for(int i=0; i<cells; i++) { //iterate over all mesh cells
        for(int j=0;j<groups;j++) { //iterate over all groups
            if(i>0) { //if D-tilda-minus exists find it
                DtildaMinus=2*propArray[i][j][0]*propArray[i-1][j][0]/
                    delta*(propArray[i][j][0]+propArray[i-1][j][0]);
            } else if(leftBound) { //if at vacuum boundary update the stufff
                DtildaMinus=2*propArray[i][j][0]/(delta*(1+4*propArray[i][j][0]/delta));
            } else {  //if at reflective boundary set it to 0
                DtildaMinus=0;
            }
            if(i<cells-i) {//same for dTitldaPlus
                DtildaPlus=2*propArray[i][j][0]*propArray[i+1][j][0]/
                    delta*(propArray[i][j][0]+propArray[i+1][j][0]);
            } else if (rightBound) { //if at right vacuum boundary
                DtildaPlus=2*propArray[i][j][0]/(delta*(1+4*propArray[i][j][0]/delta));
            } else { //if at reflective boundary
                DtildaPlus=0;
            }

            cellTarget=groups*i+j; //finds where this cell and group lives in flux array
           //
            /******************Now to start filling the array*************/ 
            //
            if(i>0) { //fill in the left difusion
                //same energy group 1 cell to the left though
                ret[cellTarget][(i-1)*groups+j]= -DtildaMinus;
            } 

            if(i<cells -1 )  {// fill in the right difusion. 
                ret[cellTarget][(i+1)*groups+j]= -DtildaPlus;
            }
            sigmaRemoval=propArray[i][j][1]+propArray[i][j][2]+propArray[i][j][3];

            //fill in flux removal term
            ret[cellTarget][cellTarget]=sigmaRemoval*delta+DtildaMinus+DtildaPlus;
        }
    }
}
