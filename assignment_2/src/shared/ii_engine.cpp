#include "ii_engine.h"


#include "pfsp_state.h"

/****************************************/
/****************************************/

IIEngine::IIEngine(PfspProblem &c_problem, bool best_improvement)
{
  this->m_pcProblem = &c_problem;
  this->m_bBestImprovement = best_improvement; 
}

/****************************************/
/****************************************/

IIEngine::~IIEngine()
{
}

/****************************************/
/****************************************/

const bool IIEngine::GetBestImprovementValue() const
{
  return this->m_bBestImprovement;
}


/****************************************/
/****************************************/

void IIEngine::SetBestImprovementValue(bool best_improvement)
{
  this->m_bBestImprovement = best_improvement;
}


/****************************************/
/****************************************/

void IIEngine::PerformSearch()
{
  // Store the current state, i.e. the current candidate solution.
  PfspState cCurrentState = this->m_pcProblem->GetInitialState();
  bool bKeepSearching = true;
  bool bImprovementFound = false;


  // Initial score;
  long int nBestResultValue = this->m_pcProblem->EvaluateState(*(this->m_pcProblem), cCurrentState);
  // Keep searching until a local optimum is found.
  while (bKeepSearching)
  {
    auto& vecNeighbours = m_pcProblem->GetNeighbours(*(this->m_pcProblem), cCurrentState);
    bImprovementFound = false;
    // Evaluate the neighbours.
    for (auto& cNeigh : vecNeighbours)
    {
      long int nTempScore = m_pcProblem->EvaluateState(*(this->m_pcProblem), *cNeigh);
      // Minimization
      if (nTempScore < nBestResultValue)
      {
        nBestResultValue = nTempScore;
        cCurrentState = *cNeigh;
        bImprovementFound = true;
        // A better candindate solution is found,
        // move to the next step.
        // Done only in First improvement.
        if(!this->m_bBestImprovement)
          break;
      }
    }
    // If no improvement, we have found a local optimum.
    // Stop the search.
    if (!bImprovementFound)
    {
      bKeepSearching = false;
    }
  }
  // Store the result.
  m_cResult = cCurrentState;
  m_dResultValue = nBestResultValue;
}