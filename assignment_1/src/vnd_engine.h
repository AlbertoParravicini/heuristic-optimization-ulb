#ifndef _VNDENGINE_H_
#define _VNDNGINE_H_

#include "pfsp_problem.h"
#include "pfsp_state.h"

class VndEngine
{
  private:
    PfspProblem* m_pcProblem;
    PfspState* m_pcResult;
    long int m_dResultValue;
    std::vector<GetNeighbourFunctionPfsp> m_vecNeighbourFunctions;

  public:
    VndEngine(PfspProblem& c_problem, std::vector<GetNeighbourFunctionPfsp>& vec_neighbour_functions);
    ~VndEngine();

    // Get the result of the search, as state and value.
    const long int GetResultValue() const;
    PfspState& GetResultState();

    // Get the problem associated to the optimization.
    PfspProblem& GetProblem();

    // Get the vector of neighbourhood functions;
    std::vector<GetNeighbourFunctionPfsp>& GetNeighbourFunctions();

    // Start the optimization process.
    void PerformSearch();
};

#endif