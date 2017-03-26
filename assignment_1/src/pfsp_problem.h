#ifndef _PFSPPROBLEM_H_
#define _PFSPPROBLEM_H_

// #include "problem.h"
#include "pfsp_state.h"
#include "pfspinstance.h"

class PfspProblem;

using GetNeighbourFunctionPfsp = std::vector<PfspState *> &(*)(PfspState &c_state);
using EvaluateStateFunctionPfsp = const long int (*)(PfspProblem &c_problem, PfspState &c_state);
using InitialStateFunctionPfsp = PfspState (*)(PfspInstance &c_instance);

class PfspProblem //: public Problem
{
private:
  PfspInstance m_cProblemInstance;
  PfspState m_cInitialState;

public:
  PfspProblem(
    char *instance_filename,
    std::vector<PfspState*>& (*neigh_fct)(PfspState&),
    const long int (*eval_fct)(PfspProblem&, PfspState&),
    InitialStateFunctionPfsp init_state);
  ~PfspProblem();

  PfspInstance &GetProblemInstance();

  GetNeighbourFunctionPfsp GetNeighbours;
  EvaluateStateFunctionPfsp EvaluateState;

  PfspState& GetInitialState();
};

#endif