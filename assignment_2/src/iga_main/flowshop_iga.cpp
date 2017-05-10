#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <fstream>

#include "../shared/pfspinstance.h"
#include "../shared/pfsp_state.h"
#include "../shared/pfsp_problem.h"
#include "../shared/ii_engine.h"
#include "../shared/iga_engine.h"
#include "../shared/support_functions.h"
#include "../shared/engine.h"

#include "../include/cxxopts.hpp"

int main(int argc, char *argv[])
{
  // Random seed to be used;
  srand(time(NULL));
  int nRngSeed = rand();

  // Filename of the instance;
  std::string sFileName;

  // Number of element that are perturbated in the Distruction / Construction procedure;
  int nDistrVecSize = 3;

  // Coefficient used in the temperature evaluation of the algorithm, the higher, the fewer diversification.
  float nLambdaTemp = 3;

  // PFSP Problem, to be instantiated later based on the input parameters;
  PfspProblem *cProblem = NULL;

  // Optimization engine, to be set later based on the input parameters;
  Engine *cSearchEngine = NULL;
  
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
      ("d,distr_vec_size", "Set the number of elements that are perturbated by Distruction/Construction.", cxxopts::value<int>(nDistrVecSize))
      ("l,lambda", "Set the temperature coefficient of the algorithm", cxxopts::value<float>(nLambdaTemp))
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


    /****************************************/
    /****************************************/

    std::cout << "\n-----------------------------------\n\tSETTING SUMMARY" << std::endl;

    std::cout << "Random seed:" << nRngSeed << std::endl;

    std::cout << "Using algorithm: IGA" << std::endl;
    std::cout << "Distruction vector size: " << nDistrVecSize << std::endl;
    std::cout << "Lambda:" << nLambdaTemp << std::endl;

    
    std::cout << "-----------------------------------" << std::endl;

    /****************************************/
    /****************************************/

    // Read data from file;
    std::vector<char> writableFileName(sFileName.begin(), sFileName.end());
    writableFileName.push_back('\0');
    // Initialize the engine;
    cProblem = new PfspProblem(&writableFileName[0], &GetNeighboursTranspose, &ComputeWCT, &RandomInitialState);

    // Compute the max execution time used in the algorithm, if it hasn't been manually set.
    if (nMaxTime <= 0) {
      nMaxTime = cProblem->GetProblemInstance().GetNumberOfJobs() <= 50 ? 500 * 600 : 500 * 10000;
    }

    // Safety check on the value of nDistrVecSize;
    if (nDistrVecSize > cProblem->GetProblemInstance().GetNumberOfJobs())
    {
      std::cout << "\nWARNING: limiting --distr_vec_size to " << cProblem->GetProblemInstance().GetNumberOfJobs() << std::endl  << std::endl;
      nDistrVecSize = cProblem->GetProblemInstance().GetNumberOfJobs();
    }

    cSearchEngine = new IgaEngine(*cProblem, &RZHeuristic, &DefaultTemperature, nMaxTime, nDistrVecSize, nLambdaTemp, bWriteTrace);
    
    
    /****************************************/
    /****************************************/

    // Perform the optimization;

    // Measure execution time.
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    cSearchEngine->PerformSearch();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    long int nExecTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "\tRESULTS IGA" << std::endl;
    std::cout << "Value:" << cSearchEngine->GetResultValue() << std::endl;
    std::cout << "Execution time:" << nExecTime << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    std::ofstream output_file;
    output_file.open("./results/results_iga.csv", std::ios_base::app);

    // Write results
    output_file << sFileName << ", " 
      << cProblem->GetProblemInstance().GetNumberOfJobs() << ", " 
      << cProblem->GetProblemInstance().GetNumberOfMachines() << ", " 
      << "iga" << ", " 
      << nRngSeed << ", " 
      << nExecTime << ", " 
      << cSearchEngine->GetResultValue() << ", "
      << nLambdaTemp << ", "
      << nDistrVecSize << ", "
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
