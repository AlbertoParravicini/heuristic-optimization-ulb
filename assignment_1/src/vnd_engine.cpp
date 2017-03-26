#include "vnd_engine.h"

/****************************************/
/****************************************/

VndEngine::VndEngine(PfspProblem &c_problem, std::vector<GetNeighbourFunctionPfsp> &vec_neighbour_functions)
{
  this->m_pcProblem = &c_problem;
  this->m_vecNeighbourFunctions = vec_neighbour_functions;
}

/****************************************/
/****************************************/

VndEngine::~VndEngine()
{
}

/****************************************/
/****************************************/

PfspProblem &VndEngine::GetProblem()
{
  return *(this->m_pcProblem);
}

/****************************************/
/****************************************/

PfspState &VndEngine::GetResultState()
{
  return *(this->m_pcResult);
}

/****************************************/
/****************************************/

const long int VndEngine::GetResultValue() const
{
  return m_dResultValue;
}

/****************************************/
/****************************************/

std::vector<GetNeighbourFunctionPfsp> &VndEngine::GetNeighbourFunctions()
{
  return this->m_vecNeighbourFunctions;
}

/****************************************/
/****************************************/

void VndEngine::PerformSearch()
{
  PfspState *cCurrentState = &(this->m_pcProblem->GetInitialState());
  //std::cout << cCurrentState->GetState().t() << std::endl;
  bool bKeepSearching = true;
  bool bImprovementFound = false;

  // Initial score;
  long int nBestResultValue = this->m_pcProblem->EvaluateState(*(this->m_pcProblem), *cCurrentState);
  while (bKeepSearching)
  {
    // Compute the neighbourhoods with multiple functions.
    // Start from the first (assumed to be the most restrictive),
    // then move to the others if no improvement was found.
    for (GetNeighbourFunctionPfsp fNeighFunction : m_vecNeighbourFunctions)
    {
      std::vector<PfspState *> vecNeighbours = fNeighFunction(*cCurrentState);
      //std::cout << "current state: " << cCurrentState->GetState().t() << std::endl;
      bImprovementFound = false;
      // Evaluate the neighbours.
      for (PfspState *cNeigh : vecNeighbours)
      {
        long int nTempScore = m_pcProblem->EvaluateState(*(this->m_pcProblem), *cNeigh);
        //std::cout << "neigh: " << cNeigh->GetState().t() << std::endl;

        // Minimization;
        // If the current neighbourhood function finds an improvement,
        // go to the next search step.
        if (nTempScore < nBestResultValue)
        {
          nBestResultValue = nTempScore;
          cCurrentState = cNeigh;
          bImprovementFound = true;
          break;
        }
      }
      // If an improvement was found, go directly to the next search step,
      // without building the other neighbourhoods.
      if (bImprovementFound)
      {
        break;
      }
    }

    // If no improvement was found by any neighbourhood function,
    // we have found a local optimum.
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