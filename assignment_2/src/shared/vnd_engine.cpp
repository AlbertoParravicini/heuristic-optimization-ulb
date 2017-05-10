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

std::vector<GetNeighbourFunctionPfsp> &VndEngine::GetNeighbourFunctions()
{
  return this->m_vecNeighbourFunctions;
}

/****************************************/
/****************************************/

void VndEngine::PerformSearch()
{
  // Store the current state, i.e. the current candidate solution.
  std::shared_ptr<PfspState>cCurrentState = &(this->m_pcProblem->GetInitialState());
  bool bKeepSearching = true;
  bool bImprovementFound = false;

  // Initial score;
  long int nBestResultValue = this->m_pcProblem->EvaluateState(*(this->m_pcProblem), *cCurrentState);
  // Keep searching until a local optimum is found.
  while (bKeepSearching)
  {
    // Compute the neighbourhoods with multiple functions.
    // Start from the first (assumed to be the most restrictive),
    // then move to the others if no improvement was found.
    for (GetNeighbourFunctionPfsp fNeighFunction : m_vecNeighbourFunctions)
    {
      std::vector<std::shared_ptr<PfspState>> vecNeighbours = fNeighFunction(*(this->m_pcProblem), *cCurrentState);

      bImprovementFound = false;
      // Evaluate the neighbours.
      for (std::shared_ptr<PfspState>cNeigh : vecNeighbours)
      {
        long int nTempScore = m_pcProblem->EvaluateState(*(this->m_pcProblem), *cNeigh);

        // Minimization;
        // If the current neighbourhood function finds an improvement,
        // go to the next search step.
        if (nTempScore < nBestResultValue)
        {
          nBestResultValue = nTempScore;
          cCurrentState = cNeigh;
          bImprovementFound = true;
          // A better candindate solution is found,
          // move to the next step.
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