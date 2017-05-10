#ifndef _IGAENGINE_H_
#define _IGAENGINE_H_

#include "pfsp_problem.h"
#include "pfsp_state.h"
#include "engine.h"
#include "ii_engine.h"
#include "support_functions.h"

using TemperatureFunction = float (*)(PfspProblem &c_problem, const float n_lambda_temp);

/*
* IgaEngine:
*   Iterative Greedy engine.
*/
class IgaEngine : public Engine 
{
  private:
    PfspState m_cCurrentState;
    InitialStateFunctionPfsp HeuristicStateFunction;
    int m_nDestrVectorSize;
    float m_nLambdaTemp;
    TemperatureFunction GetTemperature;
    long int m_nMaxTime;
    bool m_bWriteTempData;

    

  public:
    IgaEngine(
              PfspProblem &c_problem,
              InitialStateFunctionPfsp heuristic_state,
              TemperatureFunction temperature_function,
              long int n_max_time,
              int n_destr_vector_size = 3,
              float lambda_temp = 3,
              bool b_write_data = false
             );
    ~IgaEngine();

    // Start the optimization process.
    void PerformSearch();

    // Getter/Setter for the parameters.
    const int GetDestrVectorSize() const;
    const float GetLambdaTemp() const;
    const long int GetMaxTime() const;

    void SetDestrVectorSize(const int n_destr_vector_size);
    void SetLambdaTemp(const float lambda_temp);
    void SetMaxTime(const long int n_max_time);


};

#endif