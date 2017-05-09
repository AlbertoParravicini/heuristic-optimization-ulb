#ifndef _GENENGINE_H_
#define _GENENGINE_H_

#include "pfsp_problem.h"
#include "pfsp_state.h"
#include "engine.h"
#include "ii_engine.h"
#include "support_functions.h"

using DistributionWeightsFunction = arma::Col<float> (*)(arma::Col<float> vec_states_scores);
using CrossoverFunctionPfsp = std::vector<PfspState> (*)(PfspState &c_parent_1, PfspState &c_parent_2);
using MutationFunctionPfsp = PfspState (*)(PfspState &c_state);

/*
* Gen Engine:
*   Genetic Algorithm engine.
*/
class GenEngine : public Engine 
{
  private:
    PfspState m_cCurrentState;
    long int m_nMaxTime;
    InitialStateFunctionPfsp GenerateState;
    DistributionWeightsFunction ComputeDistrWeights;
    CrossoverFunctionPfsp CrossoverFunction;
    MutationFunctionPfsp MutationFunction;
    int m_nPopulationSize;
    float m_nCrossoverProb;
    float m_nMutationProb;
    bool m_bWriteTempData;
    

  public:
    GenEngine(
              PfspProblem &c_problem,
              long int n_max_time,
              InitialStateFunctionPfsp GenerateState,
              DistributionWeightsFunction distr_weights,
              CrossoverFunctionPfsp crossover_fun,
              MutationFunctionPfsp mutation_fun,
              int n_population_size = 20,
              float n_crossover_prob = 0.8,
              float n_mut_prob = 0.2,
              bool b_write_data = false
             );
    ~GenEngine();

    // Start the optimization process.
    void PerformSearch();

    // Getter/Setter for the parameters.
    const long int GetMaxTime() const;

    void SetMaxTime(const long int n_max_time);

};

#endif