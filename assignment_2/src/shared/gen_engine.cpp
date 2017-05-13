#include "gen_engine.h"

/****************************************/
/****************************************/

GenEngine::GenEngine(
                  PfspProblem &c_problem,
                  long int n_max_time,
                  InitialStateFunctionPfsp generate_state,
                  DistributionWeightsFunction distr_weights,
                  CrossoverFunctionPfsp crossover_fun,
                  MutationFunctionPfsp mutation_fun,
                  int n_population_size,
                  float n_crossover_prob,
                  float n_mut_prob,
                  bool b_write_data
                  )
{
  this->m_pcProblem = &c_problem;
  this->m_nMaxTime = n_max_time;
  this->GenerateState = generate_state;
  this->ComputeDistrWeights = distr_weights;
  this->CrossoverFunction = crossover_fun;
  this->MutationFunction = mutation_fun;
  this->m_nPopulationSize = n_population_size;
  this->m_nCrossoverProb = n_crossover_prob;
  this->m_nMutationProb = n_mut_prob;
  this->m_bWriteTempData = b_write_data;
}

/****************************************/
/****************************************/

GenEngine::~GenEngine()
{
}

/****************************************/
/****************************************/

const long int GenEngine::GetMaxTime() const
{
  return this->m_nMaxTime;
}

/****************************************/
/****************************************/

void GenEngine::SetMaxTime(const long int n_max_time)
{
  this->m_nMaxTime = n_max_time;
}

/****************************************/
/****************************************/

void GenEngine::PerformSearch()
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

  // Initialize the result to a random state;
  this->m_cResult = this->m_pcProblem->GetInitialState();
  this->m_dResultValue = this->m_pcProblem->EvaluateState(*this->m_pcProblem, this->m_cResult);

  // Initialize local search engine;
  IIEngine* cLocalEngine = new IIEngine(*this->m_pcProblem, true);

  // Used to identify this algorithm instance in the execution trace file.
  auto nTimeStamp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();


  /****************************************/
  // INITIAL POPULATION
  /****************************************/

  // Vector that store the population of the algorithm;
  std::vector<PfspState> vecPopulation(m_nPopulationSize);
  // Vector that is used to compute the probability of each state to be picked.
  arma::Col<float> vecProb = arma::zeros<arma::Col<float>>(m_nPopulationSize);

  // Generate a random initial population;
  for (int i = 0; i < m_nPopulationSize; i++) 
  {
    vecPopulation[i] = this->GenerateState(*this->m_pcProblem);
    long int nStateValue = this->m_pcProblem->EvaluateState(*this->m_pcProblem, vecPopulation[i]);
    vecPopulation[i].SetStateValue(nStateValue);
    vecProb(i) = nStateValue;

    // Check if any of the random initial states is the new best state;
    if (nStateValue < this->m_dResultValue)
    {
      this->m_cResult = vecPopulation[i];
      this->m_dResultValue = nStateValue;
    }
  }

  // Generate the weights for the distribution;
  vecProb = this->ComputeDistrWeights(vecProb);
  // Generate a discrete distribution based on the scores of the population;
  std::default_random_engine cGenerator;
  std::discrete_distribution<int> cDistribution (vecProb.begin(), vecProb.end());

  


  /****************************************/
  // MAIN LOOP
  /****************************************/

  while ((nElapsedTime < m_nMaxTime) && (nPlateauSize < m_nMaxTime / 50))
  {
    // Measure time
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    bImprovementFound = false;

    /****************************************/
    // CROSSOVER
    /****************************************/

    // Vector that store the population generated from crossover;
    std::vector<PfspState> vecCrossoverPop(m_nPopulationSize);
    // Vector that is used to compute the probabilities associated to the children states;
    // These probabilities will be used when building the final population;
    arma::Col<float> vecCrossProb = arma::zeros<arma::Col<float>>(m_nPopulationSize);

    // Build crossover population;
    for (int i = 0; i < m_nPopulationSize; i+=2)
    {
      PfspState cParent1 = vecPopulation[cDistribution(cGenerator)];
      PfspState cParent2 = vecPopulation[cDistribution(cGenerator)];

      if (((double) rand() / (RAND_MAX)) < m_nCrossoverProb)
      {
        // Do crossover, with a certain probability;
        std::vector<PfspState> vecChildren = this->CrossoverFunction(cParent1, cParent2);
        vecCrossoverPop[i] = vecChildren[0];
        vecCrossoverPop[i + 1] = vecChildren[1];
      }
      else
      {
        // Copy the parents directly, otherwise;
        vecCrossoverPop[i] = cParent1;
        vecCrossoverPop[i + 1] = cParent2;
      }
      // Set the states values.
      vecCrossoverPop[i].SetStateValue(this->m_pcProblem->EvaluateState(*this->m_pcProblem, vecCrossoverPop[i]));
      vecCrossoverPop[i + 1].SetStateValue(this->m_pcProblem->EvaluateState(*this->m_pcProblem, vecCrossoverPop[i + 1]));
    }


    /****************************************/
    // LOCAL SEARCH, TRANSPOSE
    /****************************************/
    // Apply local search to each of the states of the current population.
    // The results will be the new population.

    // Vector that is used to store the results of local search.
    std::vector<PfspState> vecLocalSearchPop(this->m_nPopulationSize);
    // Vector that is used to compute the probabilities associated to the mutated states;
    arma::Col<float> vecLocalSearchProb = arma::zeros<arma::Col<float>>(this->m_nPopulationSize);

    // Set Transpose as neighbourhood generator for local search;
    this->m_pcProblem->GetNeighbours = &GetNeighboursTranspose;

    // Local search.
    // Set the problem state as the one computed above, before doing the sub-search.
    std::cout << "DOING LOCAL SEARCH: [";
    for (int i = 0; i < this->m_nPopulationSize; i++)
    {
      std::cout << "-";
      this->m_pcProblem->SetInitialState(vecCrossoverPop[i]);
      cLocalEngine->PerformSearch();

      vecLocalSearchPop[i] = cLocalEngine->GetResultState();
      vecLocalSearchPop[i].SetStateValue(cLocalEngine->GetResultValue());
      vecLocalSearchProb(i) = cLocalEngine->GetResultValue();

      // Check if any of the new states is the new best candidate;
      if (vecLocalSearchPop[i].GetStateValue() < this->m_dResultValue)
      {
        std::cout << "FOUND IMPROVEMENT: -- value: " << vecLocalSearchPop[i].GetStateValue() << std::endl;
        this->m_cResult = vecLocalSearchPop[i];
        this->m_dResultValue = vecLocalSearchPop[i].GetStateValue();
        bImprovementFound = true;
      }
    }
    std::cout << "]" << std::endl;

    // Generate the weights for the distribution;
    vecCrossProb = this->ComputeDistrWeights(vecLocalSearchProb);
    // Generate a discrete distribution based on the scores of the population;
    std::discrete_distribution<int> cDistributionCross (vecCrossProb.begin(), vecCrossProb.end());

  

    
    /****************************************/
    // MUTATION
    /****************************************/

    // Build mutated population, from the crossover population.
    // Each candidate solution has a certain probability to be mutated.
    // This will also be the new population.
    std::vector<PfspState> vecMutatedPop(m_nPopulationSize);

    long int tot_value = 0;

    for (int i = 0; i < m_nPopulationSize; i++) 
    {
      // Pick a random element from the population.
      PfspState cCandidateState = vecLocalSearchPop[cDistributionCross(cGenerator)];
      if (((double) rand() / (RAND_MAX)) < m_nMutationProb)
      {
        vecMutatedPop[i] = this->MutationFunction(cCandidateState);
        vecMutatedPop[i].SetStateValue(this->m_pcProblem->EvaluateState(*this->m_pcProblem, vecMutatedPop[i]));
      }
      else
      { 
        // Just copy the candidate;
        vecMutatedPop[i] = cCandidateState;
      }
      tot_value += vecMutatedPop[i].GetStateValue();
    }

    /****************************************/
    // LOCAL SEARCH, Exchange
    /****************************************/
    // Apply local search to each of the states of the current population.
    // The results will be the new population.

    // Vector that is used to store the results of local search.
    vecLocalSearchPop = std::vector<PfspState>(this->m_nPopulationSize);
    // Vector that is used to compute the probabilities associated to the mutated states;
    vecLocalSearchProb = arma::zeros<arma::Col<float>>(this->m_nPopulationSize);

    // Set Insert as neighbourhood generator for local search;
    this->m_pcProblem->GetNeighbours = &GetNeighboursExchange;

    // Local search.
    // Set the problem state as the one computed above, before doing the sub-search.
    std::cout << "DOING LOCAL SEARCH: [" << std::flush;
    for (int i = 0; i < this->m_nPopulationSize; i++)
    {
      std::cout << "-" << std::flush;
      this->m_pcProblem->SetInitialState(vecMutatedPop[i]);
      cLocalEngine->PerformSearch();

      vecLocalSearchPop[i] = cLocalEngine->GetResultState();
      vecLocalSearchPop[i].SetStateValue(cLocalEngine->GetResultValue());
      vecLocalSearchProb(i) = cLocalEngine->GetResultValue();

      // Check if any of the new states is the new best candidate;
      if (vecLocalSearchPop[i].GetStateValue() < this->m_dResultValue)
      {
        std::cout << " FOUND IMPROVEMENT: " << vecLocalSearchPop[i].GetStateValue() << std::flush;
        this->m_cResult = vecLocalSearchPop[i];
        this->m_dResultValue = vecLocalSearchPop[i].GetStateValue();
        bImprovementFound = true;
      }
    }
    std::cout << "]" << std::endl;


    std::cout << nStepCount << ") " 
      << "TIME: " << nElapsedTime << " / " << this->m_nMaxTime 
      << " - AVG QUALITY:" <<  arma::sum(vecLocalSearchProb) / this->m_nPopulationSize 
      << " - BEST: " << this->m_dResultValue 
      << " - plateau: " << nPlateauSize << " out of " << this->m_nMaxTime / 50
      <<  std::endl;

    // Generate the weights for the distribution;
    vecLocalSearchProb = this->ComputeDistrWeights(vecLocalSearchProb);
    // Generate a discrete distribution based on the scores of the population;
    std::discrete_distribution<int> cDistributionLS (vecLocalSearchProb.begin(), vecLocalSearchProb.end());

    

    /****************************************/
    // REPLACE
    /****************************************/

    vecPopulation = vecLocalSearchPop;
    vecProb = vecLocalSearchProb;
    cDistribution = cDistributionLS;

    nStepCount++;
    

    // Add the iteration time;
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    nElapsedTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    nPlateauSize = bImprovementFound ? 0 : nPlateauSize + std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();


    // Write the temporary results on a file;
    if (m_bWriteTempData)
    {
      std::ofstream output_file;
      output_file.open("./results/results_details_gen.csv", std::ios_base::app);

      // Write results
      output_file << nTimeStamp << ", " 
        << this->m_pcProblem->GetProblemInstance().GetNumberOfJobs() << ", " 
        << this->m_pcProblem->GetProblemInstance().GetNumberOfMachines() << ", " 
        << "gen" << ", " 
        << this->m_nPopulationSize << ", "
        << this->m_nCrossoverProb << ", "
        << this->m_nMutationProb << ", "
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