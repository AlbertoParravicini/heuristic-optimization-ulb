#ifndef _PFSPPROBLEM_H_
#define _PFSPPROBLEM_H_

#include "pfsp_state.h"
#include "pfspinstance.h"

class PfspProblem;

using GetNeighbourFunction = std::vector<PfspState*>& (*)(PfspState& c_state);
using EvaluateStateFunction = const long int (*)(PfspProblem& c_problem, PfspState& c_state);
using InitialStateFunction = PfspState (*)(PfspInstance& c_instance);

class PfspProblem
{
  private:
    PfspInstance m_cProblemInstance;
    PfspState m_cInitialState;

  public:
    PfspProblem(char* instance_filename, GetNeighbourFunction neigh_fct, EvaluateStateFunction eval_fct, InitialStateFunction init_state);
    ~PfspProblem();

    GetNeighbourFunction GetNeighbours;
    PfspState& GetInitialState();
    PfspInstance& GetProblemInstance();
    EvaluateStateFunction EvaluateState;
};

#endif