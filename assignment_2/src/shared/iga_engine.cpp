#include "iga_engine.h"

/****************************************/
/****************************************/

IgaEngine::IgaEngine(
                  PfspProblem &c_problem,
                  InitialStateFunctionPfsp heuristic_state,
                  TemperatureFunction temperature_function,
                  long int n_max_steps,
                  int n_destr_vector_size,
                  float lambda_temp
                  )
{
  this->m_pcProblem = &c_problem;
  this->HeuristicStateFunction = heuristic_state;
  this->m_nDestrVectorSize = n_destr_vector_size;
  this->m_nLambdaTemp = lambda_temp;
  this->GetTemperature = temperature_function;
  this->m_nMaxSteps = n_max_steps;
}

/****************************************/
/****************************************/

IgaEngine::~IgaEngine()
{
}

/****************************************/
/****************************************/

const int IgaEngine::GetDestrVectorSize() const
{
  return this->m_nDestrVectorSize;
}

/****************************************/
/****************************************/

void IgaEngine::SetDestrVectorSize(const int n_destr_vector_size)
{
  this->m_nDestrVectorSize = n_destr_vector_size;
}

/****************************************/
/****************************************/

const float IgaEngine::GetLambdaTemp() const
{
  return this->m_nLambdaTemp;
}

/****************************************/
/****************************************/

void IgaEngine::SetLambdaTemp(const float lambda_temp)
{
  this->m_nLambdaTemp = lambda_temp;
}

/****************************************/
/****************************************/

const long int IgaEngine::GetMaxSteps() const
{
  return this->m_nMaxSteps;
}

/****************************************/
/****************************************/

void IgaEngine::SetMaxSteps(const long int n_max_steps)
{
  this->m_nMaxSteps = n_max_steps;
}

/****************************************/
/****************************************/

void IgaEngine::PerformSearch()
{
  // Compute the initial state of the search by using the given heuristic.
  this->m_cCurrentState = this->HeuristicStateFunction(*this->m_pcProblem);
  // Set the problem state as the one computed above, before doing the sub-search.
  this->m_pcProblem->SetInitialState(this->m_cCurrentState);

  // Perform local search.
  IIEngine* cLocalEngine = new IIEngine(*this->m_pcProblem, true);
  cLocalEngine->PerformSearch();

  // Store the result of the local search.
  PfspState cIncumbentState = cLocalEngine->GetResultState();
  long int cIncumbentValue = cLocalEngine->GetResultValue();

  // Store the final result.
  m_pcResult = &cIncumbentState;
  m_dResultValue = cIncumbentValue;

  double nCurrTemp = this->GetTemperature(*this->m_pcProblem, this->m_nLambdaTemp);
  long int nStepCount = 0;
  long int nPlateauSize = 0;


  while ((nStepCount < m_nMaxSteps) && (nPlateauSize < m_nMaxSteps / 10)) 
  {
    std::cout << "STEP: " << nStepCount << " - VALUE: " << m_dResultValue << std::endl;

    // Destruction-Construction.
    PfspState cDestrConstrState = DefaultDestrConstr(*this->m_pcProblem, cIncumbentState, this->m_nDestrVectorSize);
    // Evaluate the Destruction-Construction state.
    const long int cDestrConstrValue = cDestrConstrState.GetStateValue();


    // Local search.
    // Set the problem state as the one computed above, before doing the sub-search.
    this->m_pcProblem->SetInitialState(cDestrConstrState);
    cLocalEngine->PerformSearch();

    PfspState cLocalSearchState = cLocalEngine->GetResultState();
    // Evaluate the Destruction-Construction state.
    const long int cLocalSearchValue = cLocalEngine->GetResultValue();

    if (cLocalSearchValue < cIncumbentValue)
    {
      cIncumbentState = cLocalSearchState;
      cIncumbentValue = cLocalSearchValue;
      if (cLocalSearchValue < m_dResultValue) 
      {
        this->m_pcResult = &cLocalSearchState;
        this->m_dResultValue = cLocalSearchValue;
        nPlateauSize = 0;
      }
      nPlateauSize++;
    }
    else if (((double) rand() / (RAND_MAX)) < std::exp((cIncumbentValue - cLocalSearchValue) / nCurrTemp)) 
    {
        cIncumbentState = cLocalSearchState;
        cIncumbentValue = cLocalSearchValue;

        nPlateauSize++;
    }
    else 
    {
      nPlateauSize++;
    }
    nStepCount++;
  }
}