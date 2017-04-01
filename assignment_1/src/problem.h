#ifndef _PROBLEM_H_
#define _PROBLEM_H_

#include "state.h"
#include <vector>
#include <iostream>

class Problem;

using GetNeighbourFunction = std::vector<State *> &(*)(Problem &c_problem, State &c_state);
using EvaluateStateFunction = const long int (*)(Problem &c_problem, State &c_state);

class Problem
{
private:
  State m_cInitialState;

public:
  Problem();
  virtual ~Problem();

  GetNeighbourFunction GetNeighbours;
  virtual State &GetInitialState();
  EvaluateStateFunction EvaluateState;

  std::vector<State *>& Neigh(State &c_state);
  const long int EvalState(Problem &c_problem, State &c_state);
};

#endif