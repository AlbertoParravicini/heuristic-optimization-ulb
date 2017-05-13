#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <fstream>

#include "../shared/pfspinstance.h"
#include "../shared/pfsp_state.h"
#include "../shared/pfsp_problem.h"
#include "../shared/support_functions.h"
#include "../shared/engine.h"
#include "../shared/gen_engine.h"

#include "../include/cxxopts.hpp"

int main(int argc, char *argv[])
{
  
  // Random seed to be used;
  srand(time(NULL));
  int nRngSeed = rand();

  // Filename of the instance;
  std::string sFileName;

  // PFSP Problem, to be instantiated later based on the input parameters;
  PfspProblem *cProblem = NULL;

  // Optimization engine, to be set later based on the input parameters;
  Engine *cSearchEngine = NULL;

  // Population Size
  int nPopulationSize = 10;

  // Crossover Probability
  float nCrossoverProb = 0.99;

  // Mutation Probability
  float nMutationProb = 0.05;
  
  // How the distributio weights for sampling are computed;
  DistributionWeightsFunction fWeightsType = &UniformWeights;

  // How the mutation is computed;
  MutationFunctionPfsp fMutationType = &TransposeMutation;

  // Max execution time, in milliseconds;
  long int nMaxTime = 0;

  // If true, write the algorithm execution trace on a file;
  int bWriteTrace = false;

  /****************************************/
  /****************************************/

  // Parse the command line
  try
  {
    cxxopts::Options options("Flowshop optimizer", "Optimize the flowshop scheduling problem using IGA.");

    options.add_options()
      ("f,filename", "Specify the instance filename.", cxxopts::value<std::string>(sFileName))
      ("r,random_seed", "Set the random seed of the algorithm.", cxxopts::value<int>(nRngSeed))
      ("p,population_size", "Set the population size used by the algorithm [0, +inf)", cxxopts::value<int>(nPopulationSize))
      ("c,crossover_prob", "Set the crossover probability [0, 1]", cxxopts::value<float>(nCrossoverProb))
      ("m,mutation_prob", "Set the mutation probability [0, 1]", cxxopts::value<float>(nMutationProb))
      ("w,weights_type", "Set how the candidate states are picked in the algorithm [uni, uni10, sm]", cxxopts::value<std::string>()->default_value("sm"))
      ("n,mutation_type", "Set the type of mutation [tr]", cxxopts::value<std::string>()->default_value("tr"))
      ("t,max_time", "Set the maximum execution time, in milliseconds", cxxopts::value<long int>(nMaxTime))
      ("e,write_exec_trace", "If true, write the execution trace on a file [0, 1]", cxxopts::value<int>(bWriteTrace));
    options.parse(argc, argv);

    // Need an instance filename!
    if (sFileName.empty())
    {
      std::cout << "ERROR: filename not specified!" << std::endl;
      return 1;
    }

    // Set the rng seed;
    arma::arma_rng::set_seed(nRngSeed);
    std::srand(nRngSeed);

    // Weights type
    std::string sWeightsType = options["w"].as<std::string>();

    // Mutation type
    std::string sMutationType = options["n"].as<std::string>();

    /****************************************/
    /****************************************/

    std::cout << "\n-----------------------------------\n\tSETTING SUMMARY" << std::endl;

    std::cout << "Random seed:" << nRngSeed << std::endl;

    std::cout << "Using algorithm: GENETIC" << std::endl;

    // Parse the weights type.
    if (sWeightsType == "sm")
    {
      std::cout << "Using weights type: SOFTMAX" << std::endl;
      fWeightsType = &SoftMaxWeights;
    }
    else if (sWeightsType == "uni10")
    {
      std::cout << "Using weights type: UNIFORM 10" << std::endl;
      fWeightsType = &UniformWeights10;
    }
    else
    {
      std::cout << "Using weights type: UNIFORM" << std::endl;
      fWeightsType = &UniformWeights;
    }

    // Parse the mutation type.
    if (sMutationType == "tr")
    {
      std::cout << "Using mutation type: TRANSPOSE" << std::endl;
      fMutationType = &TransposeMutation;
    }
    else
    {
      std::cout << "Using mutation type: TRANSPOSE" << std::endl;
      fMutationType = &TransposeMutation;
    } 
    
    std::cout << "POPULATION SIZE: " << nPopulationSize << std::endl;
    std::cout << "CROSSOVER PROBABILITY: " << nCrossoverProb << std::endl;
    std::cout << "MUTATION PROBABILITY: " << nMutationProb << std::endl;

    std::cout << "-----------------------------------" << std::endl;

    /****************************************/
    /****************************************/

    // Read data from file;
    std::vector<char> writableFileName(sFileName.begin(), sFileName.end());
    writableFileName.push_back('\0');
    // Initialize the engine;
    cProblem = new PfspProblem(&writableFileName[0], &GetNeighboursInsert, &ComputeWCT, &RZHeuristic);

    // Compute the max execution time used in the algorithm, if it hasn't been manually set.
    if (nMaxTime <= 0) {
      nMaxTime = cProblem->GetProblemInstance().GetNumberOfJobs() <= 50 ? 500 * 200 : 500 * 1000;
    }

    cSearchEngine = new GenEngine(*cProblem, nMaxTime, &RandomInitialState, fWeightsType, &PMXCrossover, fMutationType, nPopulationSize, nCrossoverProb, nMutationProb, bWriteTrace);
    
    
    /****************************************/
    /****************************************/

    // Perform the optimization;

    // Measure execution time.
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    cSearchEngine->PerformSearch();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    long int nExecTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "\tRESULTS GEN" << std::endl;
    std::cout << "Value:" << cSearchEngine->GetResultValue() << std::endl;
    std::cout << "Execution time:" << nExecTime << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    std::ofstream output_file;
    output_file.open("./results/results_gen.csv", std::ios_base::app);

    // Write results
    output_file << sFileName << ", " 
      << cProblem->GetProblemInstance().GetNumberOfJobs() << ", " 
      << cProblem->GetProblemInstance().GetNumberOfMachines() << ", " 
      << "gen" << ", " 
      << nRngSeed << ", " 
      << nExecTime << ", " 
      << cSearchEngine->GetResultValue() << ", "
      << nPopulationSize << ", "
      << nCrossoverProb << ", "
      << nMutationProb << ", "
      << sWeightsType << ", "
      << sMutationType << ", "
      << nMaxTime
      << std::endl;



    output_file.close();

    return 0;
  }
  catch (std::exception &e)
  {
    std::cerr << "Unhandled Exception reached the top of main: "
              << e.what() << ", application will now exit" << std::endl;
    return 1;
  }
}
