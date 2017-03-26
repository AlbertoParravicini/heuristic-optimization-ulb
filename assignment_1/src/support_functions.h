#ifndef _SUPPORTFUNCTIONS_H_
#define _SUPPORTFUNCTIONS_H_

#include "pfspinstance.h"
#include "pfsp_state.h"
#include "pfsp_problem.h"
#include "ii_engine.h"

#include <armadillo>

// Used to find the neighbours of a state.
std::vector<PfspState *> &GetNeighboursTest(PfspState &c_state);

std::vector<PfspState *> &GetNeighboursTranspose(PfspState &c_state);

std::vector<PfspState *> &GetNeighboursExchange(PfspState &c_state);

std::vector<PfspState *> &GetNeighboursInsert(PfspState &c_state);

// Evaluation functions;
const long int EvaluateStateTest(PfspState &c_state);

const long int ComputeWCT(PfspProblem &c_problem, PfspState &c_state);

const long int ComputeWCTInternals(
    const arma::Col<int> &solution,
    int n_number_of_jobs,
    int n_number_of_machines,
    arma::Mat<long int> &c_processing_time_matrix,
    arma::Col<long int> &vec_priorities);

// Used to initialize the first state;
arma::Col<int> RandomPermutation(int number_of_jobs);

PfspState RandomInitialState(PfspInstance &c_instance);

// Simplified RZ heuristic
PfspState RZHeuristic(PfspInstance &c_instance);

#endif