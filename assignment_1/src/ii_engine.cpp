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

PfspProblem& IIEngine::GetProblem()
{
  return *(this->m_pcProblem);
}

/****************************************/
/****************************************/

PfspState& IIEngine::GetResultState()
{
  return *(this->m_pcResult);
}

/****************************************/
/****************************************/

const long int IIEngine::GetResultValue() const
{
  return m_dResultValue;
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
  PfspState* cCurrentState = &(this->m_pcProblem->GetInitialState());
  bool bKeepSearching = true;
  bool bImprovementFound = false;

  // Initial score;
  long int nBestResultValue = this->m_pcProblem->EvaluateState(*(this->m_pcProblem), *cCurrentState);
  while (bKeepSearching)
  {
    std::vector<PfspState*> vecNeighbours = m_pcProblem->GetNeighbours(*cCurrentState);
    bImprovementFound = false;
    // Evaluate the neighbours.
    for (PfspState* cNeigh : vecNeighbours)
    {
      long int nTempScore = m_pcProblem->EvaluateState(*(this->m_pcProblem), *cNeigh);

      // Minimization
      if (nTempScore < nBestResultValue)
      {
        nBestResultValue = nTempScore;
        cCurrentState = cNeigh;
        bImprovementFound = true;
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
  m_pcResult = cCurrentState;
  m_dResultValue = nBestResultValue;
}