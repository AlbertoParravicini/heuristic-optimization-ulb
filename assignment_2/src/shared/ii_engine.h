#ifndef _IIENGINE_H_
#define _IIENGINE_H_

#include "pfsp_problem.h"
#include "pfsp_state.h"
#include "engine.h"
#include <memory>

/*
* IIEngine:
*   Iterative improvement engine, it can perform First or Best improvement.
*/
class IIEngine : public Engine 
{
  private:
    // If true, use Best improvement instead of First improvement.
    bool m_bBestImprovement;

  public:
    IIEngine(PfspProblem& c_problem, bool best_improvement = true);
    ~IIEngine();

    // Get/Set whether to use Best improvement.
    const bool GetBestImprovementValue() const;
    void SetBestImprovementValue(bool best_improvement);

    // Start the optimization process.
    void PerformSearch();
};

#endif