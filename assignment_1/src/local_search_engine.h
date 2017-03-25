#ifndef _LOCALSEARCHENGINE_H_
#define _LOCALSEARCHENGINE_H_

#include "pfsp_problem.h"
#include "pfsp_state.h"

class LocalSearchEngine
{
  private:
    PfspProblem* m_pcProblem;
    PfspState* m_pcResult;
    long int m_dResultValue;
    bool m_bBestImprovement;

  public:
    LocalSearchEngine(PfspProblem& c_problem, bool best_improvement = false);
    ~LocalSearchEngine();

    // Get the result of the search, as state and value.
    const long int GetResultValue() const;
    PfspState& GetResultState();

    // Get the problem associated to the optimization.
    PfspProblem& GetProblem();

    const bool GetBestImprovementValue() const;

    void SetBestImprovementValue(bool best_improvement);

    // Start the optimization process.
    void PerformSearch();
};

#endif