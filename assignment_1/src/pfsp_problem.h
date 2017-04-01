#ifndef _PFSPPROBLEM_H_
#define _PFSPPROBLEM_H_

// #include "problem.h"
#include "pfsp_state.h"
#include "pfspinstance.h"

class PfspProblem;

// Define the function pointer that represent the functions to
// generate neighbours, evaluate states, and create the initial state.
using GetNeighbourFunctionPfsp = std::vector<PfspState *> &(*)(PfspProblem &c_problem, PfspState &c_state);
using EvaluateStateFunctionPfsp = const long int (*)(PfspProblem &c_problem, PfspState &c_state);
using InitialStateFunctionPfsp = PfspState (*)(PfspProblem &c_problem);


/*
* PfspProblem:
*   The class wraps the specific implementation of a PFSP instance.
*   A problem should have an initial state, and offer functions to compute
*   the initial state, the neighbourhood of a state, and the score of a state.
*/
class PfspProblem //: public Problem
{
private:
  // Contains the problem instance, i.e. the actual implementation of the problem.
  PfspInstance m_cProblemInstance;
  // Initial candidate solution.
  PfspState m_cInitialState;

public:
  // The constructor requires the filename of the instance,
  // and function pointers that define the neighbourhood generator,
  // the state evaluation function, the initial state function.
  PfspProblem(
    char *instance_filename,
    std::vector<PfspState*>& (*neigh_fct)(PfspProblem&, PfspState&),
    const long int (*eval_fct)(PfspProblem&, PfspState&),
    InitialStateFunctionPfsp init_state);
  ~PfspProblem();

  // Returns the problem instance.
  PfspInstance &GetProblemInstance();

  // Generate the neighbourhood for a given state.
  GetNeighbourFunctionPfsp GetNeighbours;
  // Evaluate the score of a given state.
  EvaluateStateFunctionPfsp EvaluateState;
  // Returns the initial state.
  PfspState& GetInitialState();
};

#endif