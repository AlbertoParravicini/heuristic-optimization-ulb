#ifndef _SUPPORTFUNCTIONS_H_
#define _SUPPORTFUNCTIONS_H_

#include "pfspinstance.h"
#include "pfsp_state.h"
#include "pfsp_problem.h"
#include "ii_engine.h"

#include <armadillo>
#include <memory>

/****************************************/
/****************************************/

// NEIGHBOURHOOD GENERATOR FUNCTIONS:
// Used to find the neighbours of a state.

// Just a test.
std::vector<std::unique_ptr<PfspState>> &GetNeighboursTest(PfspProblem &c_problem, PfspState &c_state);
// Generate neighbours by transposition.
std::vector<std::unique_ptr<PfspState>> &GetNeighboursTranspose(PfspProblem &c_problem, PfspState &c_state);
// Generate neighbours by exchange.
std::vector<std::unique_ptr<PfspState>> &GetNeighboursExchange(PfspProblem &c_problem, PfspState &c_state);
// Generate neighbours by insertion.
std::vector<std::unique_ptr<PfspState>> &GetNeighboursInsert(PfspProblem &c_problem, PfspState &c_state);
// Generate neighbours by transposition.
std::vector<std::unique_ptr<PfspState>> &GetNeighboursTransposeFI(PfspProblem &c_problem, PfspState &c_state);
// Generate neighbours by exchange.
std::vector<std::unique_ptr<PfspState>> &GetNeighboursExchangeFI(PfspProblem &c_problem, PfspState &c_state);
// Generate neighbours by insertion.
std::vector<std::unique_ptr<PfspState>> &GetNeighboursInsertFI(PfspProblem &c_problem, PfspState &c_state);


/****************************************/
/****************************************/

// EVALUATION FUNCTION:
// Used to evaluate the score of a state.

// Just a test.
const long int EvaluateStateTest(PfspProblem &c_problem, PfspState &c_state);
// Compute the weighted completion time, given a state and the parameters specified by the problem.
const long int ComputeWCT(PfspProblem &c_problem, PfspState &c_state);
// Compute the weighted completion time, given a state and the explicit parameters.
// This function can be used if some of the parameters differ from the ones contained in the problem.
const long int ComputeWCTInternals(
    const arma::Col<int> &solution,
    int n_number_of_jobs,
    int n_number_of_machines,
    arma::Mat<long int> &c_processing_time_matrix,
    arma::Col<long int> &vec_priorities);


    
/****************************************/
/****************************************/

// FIRST STATE GENERATION:
// Used to initialize the first state;

// Generate a random initial solution, returns it as a vector.
arma::Col<int> RandomPermutation(int number_of_jobs);
// Generate a random initial solution, returns it as a state.
PfspState RandomInitialState(PfspProblem &c_problem);
// Simplified RZ heuristic
PfspState RZHeuristic(PfspProblem &c_problem);


/****************************************/
/****************************************/

// DESTRUCTION-CONSTRUCTION
// Used to perturbate a state in IGA;
PfspState DefaultDestrConstr(PfspProblem &c_problem, PfspState &c_initial_state, const int n_destr_vec_size);


/****************************************/
/****************************************/

// TEMPERATURE FUNCTION
// Used in IGA; 
float DefaultTemperature(PfspProblem &c_problem, const float lambda);


/****************************************/
/****************************************/


// DISTRIBUTION WEIGHTS
// Used in Genetic Algorithms: given a vector with the scores of the states,
// these functions compute a vector with weights that will be used in a discrete probability.
// States with high score will have higher probability to be picked for crossover and mutation.

// Probability of being picked is directly proportional to the scores.
arma::Col<float> UniformWeights(arma::Col<float> vec_states_scores);
// Apply softmax to the weights, the probabilities associated to better states will be amplified.
arma::Col<float> SoftMaxWeights(arma::Col<float> vec_states_scores);
// Similar to uniform, but with more amplification towards better states;
arma::Col<float> UniformWeights10(arma::Col<float> vec_states_scores);



/****************************************/
/****************************************/


// CROSSOVER FUNCTION
// Used in Genetic Algorithms: given 2 parent states,
// create 2 new states by combining the parents.
std::vector<PfspState> PMXCrossover(PfspState &c_parent_1, PfspState &c_parent_2);


/****************************************/
/****************************************/


// MUTATION FUNCTION
// Used in Genetic Algorithms: given a state,
// create a new state by modifying the original one.

// Swap the position of 2 jobs at random;
PfspState TransposeMutation(PfspState &c_state);

#endif