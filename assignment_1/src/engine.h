#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "pfsp_problem.h"
#include "pfsp_state.h"

/*
* Engine:
*   This class works as general template for the optimization algorithms.
*/
class Engine {

  protected:
    // Contains the problem, i.e. all the information about the specific problem instance,
    // and functions that will be used to guide the optimization process.
    PfspProblem* m_pcProblem;
    // Best solution state found by the optimization process.
    PfspState* m_pcResult;
    // Score associated to the best state found in the optimization.
    long int m_dResultValue;

  public:
    Engine();
    ~Engine();

    // Get the result of the search, as state and value.
    const long int GetResultValue() const;
    PfspState& GetResultState();

    // Get the problem associated to the optimization.
    PfspProblem& GetProblem();

    // Start the optimization process.
    virtual void PerformSearch() = 0;
};

#endif