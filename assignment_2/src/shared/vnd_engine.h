#ifndef _VNDENGINE_H_
#define _VNDNGINE_H_

#include "pfsp_problem.h"
#include "pfsp_state.h"
#include "engine.h"
#include <memory>

/*
* VndEngine:
*   Variable Neighbour Descent engine, it can take as input a vector
*   of neighbourhood generator functions, and use them to search on increasingly large 
*   neighbourhoods.
*/
class VndEngine : public Engine
{
  private:
    // Vector of function pointers, store the neighbourhood generator functions to be used.
    std::vector<GetNeighbourFunctionPfsp> m_vecNeighbourFunctions;

  public:
    VndEngine(PfspProblem& c_problem, std::vector<GetNeighbourFunctionPfsp>& vec_neighbour_functions);
    ~VndEngine();

    // Get the vector of neighbourhood functions;
    std::vector<GetNeighbourFunctionPfsp>& GetNeighbourFunctions();

    // Start the optimization process.
    void PerformSearch();
};

#endif