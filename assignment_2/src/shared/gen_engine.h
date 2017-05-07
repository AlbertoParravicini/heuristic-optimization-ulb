#ifndef _GENENGINE_H_
#define _GENENGINE_H_

#include "pfsp_problem.h"
#include "pfsp_state.h"
#include "engine.h"
#include "ii_engine.h"
#include "support_functions.h"

using TemperatureFunction = float (*)(PfspProblem &c_problem, const float n_lambda_temp);

/*
* Gen Engine:
*   Genetic Algorithm engine.
*/
class GenEngine : public Engine 
{
  private:
    PfspState m_cCurrentState;
    long int m_nMaxSteps;
    

  public:
    IgaEngine(
              PfspProblem &c_problem,
              long int n_max_steps,
             );
    ~IgaEngine();

    // Start the optimization process.
    void PerformSearch();

    // Getter/Setter for the parameters.
    const long int GetMaxSteps() const;

    void SetMaxSteps(const long int n_max_steps);

};

#endif