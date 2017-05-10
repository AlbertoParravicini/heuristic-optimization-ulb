#include "iga_engine.h"

/****************************************/
/****************************************/

IgaEngine::IgaEngine(
                  PfspProblem &c_problem,
                  InitialStateFunctionPfsp heuristic_state,
                  TemperatureFunction temperature_function,
                  long int n_max_time,
                  int n_destr_vector_size,
                  float lambda_temp,
                  bool b_write_data
                  )
{
  this->m_pcProblem = &c_problem;
  this->HeuristicStateFunction = heuristic_state;
  this->m_nDestrVectorSize = n_destr_vector_size;
  this->m_nLambdaTemp = lambda_temp;
  this->GetTemperature = temperature_function;
  this->m_nMaxTime = n_max_time;
  this->m_bWriteTempData = b_write_data;
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

const long int IgaEngine::GetMaxTime() const
{
  return this->m_nMaxTime;
}

/****************************************/
/****************************************/

void IgaEngine::SetMaxTime(const long int n_max_time)
{
  this->m_nMaxTime = n_max_time;
}

/****************************************/
/****************************************/

void IgaEngine::PerformSearch()
{
  // Count the number of steps;
  int nStepCount = 0;
  // Count the elaples time, in milliseconds;
  long int nElapsedTime = 0;
  // Stop the algorithm if we don't improve after a certain amount of steps;
  // This number is m_nMaxTime / 50;
  int nPlateauSize = 0;
  // Keep track of whether an improvement was found in the current iteration;
  bool bImprovementFound = false;

  // Used to identify this algorithm instance in the execution trace file.
  auto nTimeStamp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();

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
  m_cResult = cIncumbentState;
  m_dResultValue = cIncumbentValue;

  double nCurrTemp = this->GetTemperature(*this->m_pcProblem, this->m_nLambdaTemp);


  while ((nElapsedTime < m_nMaxTime) && (nPlateauSize < m_nMaxTime / 50))
  {
    // Measure time
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    bImprovementFound = false;
    
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

    // If the result is better, store it.
    if (cLocalSearchValue < cIncumbentValue)
    {
      cIncumbentState = cLocalSearchState;
      cIncumbentValue = cLocalSearchValue;
      if (cLocalSearchValue < m_dResultValue) 
      {
        this->m_cResult = cLocalSearchState;
        this->m_dResultValue = cLocalSearchValue;
        bImprovementFound = true;
        nPlateauSize = 0;
      }
      bImprovementFound = false;
    }
    // If not, store it with a certain probability.
    else if (((double) rand() / (RAND_MAX)) < std::exp((cIncumbentValue - cLocalSearchValue) / nCurrTemp)) 
    {
        cIncumbentState = cLocalSearchState;
        cIncumbentValue = cLocalSearchValue;

        bImprovementFound = false;
    }
    else 
    {
      bImprovementFound = false;
    }

    if (nStepCount % 100 == 0) {
      std::cout << nStepCount << ") " 
            << "TIME: " << nElapsedTime << " / " << this->m_nMaxTime 
            << " - BEST: " << this->m_dResultValue 
            << " - plateau: " << nPlateauSize << " out of " << this->m_nMaxTime / 50
            <<  std::endl;
    }
    
    nStepCount++;

    // Add the iteration time;
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    nElapsedTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    nPlateauSize = bImprovementFound ? 0 : (nPlateauSize + std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());

    // Write the temporary results on a file;
    if (m_bWriteTempData && nStepCount % 100 == 0)
    {
      std::ofstream output_file;
      output_file.open("../results/results_details_iga.csv", std::ios_base::app);

      // Write results
      output_file << nTimeStamp << ", " 
        << this->m_pcProblem->GetProblemInstance().GetNumberOfJobs() << ", " 
        << this->m_pcProblem->GetProblemInstance().GetNumberOfMachines() << ", " 
        << "iga" << ", " 
        << this->m_nLambdaTemp << ", "
        << this->m_nDestrVectorSize << ", "
        << this->m_nMaxTime << ", "
        << nStepCount << ", "
        << nElapsedTime << ", "
        << this->m_dResultValue << ", "
        << nPlateauSize
        << std::endl;
      output_file.close();
    }
  }
}