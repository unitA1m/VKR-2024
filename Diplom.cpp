#include "functionsetpole.h"
#include "functiondeletecells.h"
#include "dancingLinks.h"
#include "solveWithGeneticAlgorithm.h"

using namespace std;

vector<vector<long int>> mains(int size,int numAlgor) {
    int h = size;
    vector<vector<long int>> arrFull(h, vector<long int>(h, 0));
 
    if (numAlgor == 0) {
        backtracking(arrFull, h);
    }
    if (numAlgor == 1) {
        arrFull = generateDancingLinks(h);
        
    }
    if (numAlgor == 2) {
        arrFull = solveWithGeneticAlgorithm( 2, 1, h);
    }
    return arrFull;
}