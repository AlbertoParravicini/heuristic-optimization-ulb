#include "local_search_engine.h"

/****************************************/
/****************************************/

LocalSearchEngine::LocalSearchEngine(PfspProblem &c_problem, bool best_improvement)
{
  this->m_pcProblem = &c_problem;
  this->m_bBestImprovement = best_improvement; 
}

/****************************************/
/****************************************/

LocalSearchEngine::~LocalSearchEngine()
{
}

/****************************************/
/****************************************/

PfspProblem& LocalSearchEngine::GetProblem()
{
  return *(this->m_pcProblem);
}

/****************************************/
/****************************************/

PfspState& LocalSearchEngine::GetResultState()
{
  return *(this->m_pcResult);
}

/****************************************/
/****************************************/

const long int LocalSearchEngine::GetResultValue() const
{
  return m_dResultValue;
}

/****************************************/
/****************************************/

const bool LocalSearchEngine::GetBestImprovementValue() const
{
  return this->m_bBestImprovement;
}


/****************************************/
/****************************************/

void LocalSearchEngine::SetBestImprovementValue(bool best_improvement)
{
  this->m_bBestImprovement = best_improvement;
}


/****************************************/
/****************************************/

void LocalSearchEngine::PerformSearch()
{
  PfspState* cCurrentState = &(this->m_pcProblem->GetInitialState());
  std::cout << cCurrentState->GetState().t() << std::endl;
  bool bKeepSearching = true;
  bool bImprovementFound = false;

  // Initial score;
  long int nBestResultValue = this->m_pcProblem->EvaluateState(*(this->m_pcProblem), *cCurrentState);
  while (bKeepSearching)
  {
    std::vector<PfspState*> vecNeighbours = m_pcProblem->GetNeighbours(*cCurrentState);
    //std::cout << "current state: " << cCurrentState->GetState().t() << std::endl;
    bImprovementFound = false;
    // Evaluate the neighbours.
    for (PfspState* cNeigh : vecNeighbours)
    {
      long int nTempScore = m_pcProblem->EvaluateState(*(this->m_pcProblem), *cNeigh);
      //std::cout << "neigh: " << cNeigh->GetState().t() << std::endl;

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