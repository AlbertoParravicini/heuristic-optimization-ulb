#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <fstream>


#include "pfspinstance.h"
#include "pfsp_state.h"
#include "pfsp_problem.h"
#include "ii_engine.h"
#include "vnd_engine.h"
#include "support_functions.h"
#include "state.h"
#include "problem.h"

#include "include/cxxopts.hpp"

int main(int argc, char *argv[])
{
  // Random seed to be used;
  srand(time(NULL));
  int nRngSeed = rand();

  // Filename of the instance;
  std::string sFileName;

  // How the initial state is computed;
  InitialStateFunctionPfsp fInitialState = &RZHeuristic;

  // Neighboorhood function used by Iterative Improvement;
  GetNeighbourFunctionPfsp fNeighFunction = &GetNeighboursTranspose;

  // Vector of neighboorhood functions to be used by VND;
  std::vector<GetNeighbourFunctionPfsp> vecNeighs = {&GetNeighboursTranspose, &GetNeighboursExchange, &GetNeighboursInsert};

  // Use First improvement of Best improvement, in Iterative Improvement;
  int bBestImprovement = 0;

  /****************************************/
  /****************************************/

  // Parse the command line
  try
  {
    cxxopts::Options options("Flowshop optimizer", "Optimize the flowshop scheduling problem using Iterative Improvement and VND.");

    options.add_options()
      ("f,filename", "Specify the instance filename.", cxxopts::value<std::string>(sFileName))
      ("a,algorithm", "Set the optimization algorithm.", cxxopts::value<std::string>()->default_value("ii"))
      ("r,random_seed", "Set the random seed of the algorithm.", cxxopts::value<int>(nRngSeed))
      ("n,neighbourhood_function", "Set how the neighbours of a state are generated.", cxxopts::value<std::string>()->default_value("t"))
      ("i,initial_state_function", "Set how the initial state is computed.", cxxopts::value<std::string>()->default_value("rz"))
      ("b,use_best_improvement", "Set of the Iterative Improvement Algorithm should use best increment", cxxopts::value<int>(bBestImprovement))
      ("v,neigh_vector", "Set of sequence of neighbourhood functions used by VND", cxxopts::value<std::string>()->default_value("tei"));
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

    // Parse the command line parameters.

    // Parse the algorithm type.
    std::string sAlgoType = options["a"].as<std::string>();

    // Parse the neighbourhood function type.
    std::string sNeighType = options["n"].as<std::string>();

    // Parse the initial state function type.
    std::string sInitType = options["i"].as<std::string>();

    // Parse the vector of neighbourhood functions type.
    std::string sVecNeighType = options["v"].as<std::string>();

    /****************************************/
    /****************************************/

    std::cout << "\n-----------------------------------\n\tSETTING SUMMARY" << std::endl;

    std::cout << "Random seed:" << nRngSeed << std::endl;

    // Parse the initial state generation type.
    if (sInitType == "random")
    {
      std::cout << "Using initial state generator: Random" << std::endl;
      fInitialState = &RandomInitialState;
    }
    else
    {
      std::cout << "Using initial state generator: Simplified RZ" << std::endl;
      fInitialState = &RZHeuristic;
    }

    // Pick the appropriate algorithm.
    if (sAlgoType == "vnd")
    {
      std::cout << "Using algorithm: VND" << std::endl;

      // Parse the vector of neighbourhood functions type.
      if (sVecNeighType == "tie")
      {
        std::cout << "Using neighbourhood functions: Transpose, Insert, Exchange" << std::endl;
        vecNeighs = {&GetNeighboursTranspose, &GetNeighboursInsert, &GetNeighboursExchange};
      }
      else
      {
        std::cout << "Using neighbourhood functions: Transpose, Exchange, Insert" << std::endl;
        vecNeighs = {&GetNeighboursTranspose, &GetNeighboursExchange, &GetNeighboursInsert};
      }
      std::cout << "-----------------------------------" << std::endl;

      // Read data from file;
      std::vector<char> writableFileName(sFileName.begin(), sFileName.end());
      writableFileName.push_back('\0');
      PfspProblem cProblem(&writableFileName[0], &GetNeighboursTranspose, &ComputeWCT, &RZHeuristic);
      VndEngine cSearchEngine(cProblem, vecNeighs);
      
      // Measure execution time.
      std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
      cSearchEngine.PerformSearch();
      std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

      auto nExecTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
      std::cout << "\tRESULTS VND" << std::endl;
      std::cout << "Value:" << cSearchEngine.GetResultValue() << std::endl;
      std::cout << "Execution time:" << nExecTime << std::endl;
      std::cout << "-----------------------------------" << std::endl;

      std::ofstream output_file;
      output_file.open("./results/results_vnd.csv", std::ios_base::app);

      // Write results
      output_file << sFileName << ", " << 
        cProblem.GetProblemInstance().GetNumberOfJobs() << ", " << 
        cProblem.GetProblemInstance().GetNumberOfMachines() << ", " << 
        sAlgoType << ", " << 
        bBestImprovement << ", " <<
        sInitType << ", " << 
        sNeighType << ", " << 
        nRngSeed << ", " << 
        sVecNeighType << ", " << 
        nExecTime << ", " << 
        cSearchEngine.GetResultValue() << std::endl; 
    
      output_file.close(); 
    }
    else
    {
      std::cout << "Using algorithm: Iterative Improvement" << std::endl;
      std::cout << "Using Best Improvement: " << bBestImprovement << std::endl;

      // Pick the appropriate neighbourhood function.
      if (sNeighType == "e")
      {
        std::cout << "Using neighbourhood function: Exchange" << std::endl;
        fNeighFunction = &GetNeighboursExchange;
      }
      else if (sNeighType == "i")
      {
        std::cout << "Using neighbourhood function: Insert" << std::endl;
        fNeighFunction = &GetNeighboursInsert;
      }
      else
      {
        std::cout << "Using neighbourhood function: Transpose" << std::endl;
        fNeighFunction = &GetNeighboursTranspose;
      }
      std::cout << "-----------------------------------" << std::endl;

      // Read data from file;
      std::vector<char> writableFileName(sFileName.begin(), sFileName.end());
      writableFileName.push_back('\0');
      PfspProblem cProblem(&writableFileName[0], fNeighFunction, &ComputeWCT, fInitialState);
      IIEngine cSearchEngine(cProblem, bBestImprovement);

            // Measure execution time.
      std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
      cSearchEngine.PerformSearch();
      std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

      long int nExecTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
      std::cout << "\tRESULTS II" << std::endl;
      std::cout << "Value:" << cSearchEngine.GetResultValue() << std::endl;
      std::cout << "Execution time:" << nExecTime << std::endl;
      std::cout << "-----------------------------------" << std::endl;

      std::ofstream output_file;
      output_file.open("./results/results_ii.csv", std::ios_base::app);

      // Write results
      output_file << sFileName << ", " << 
        cProblem.GetProblemInstance().GetNumberOfJobs() << ", " << 
        cProblem.GetProblemInstance().GetNumberOfMachines() << ", " << 
        sAlgoType << ", " << 
        bBestImprovement << ", " <<
        sInitType << ", " << 
        sNeighType << ", " << 
        nRngSeed << ", " << 
        sVecNeighType << ", " << 
        nExecTime << ", " << 
        cSearchEngine.GetResultValue() << std::endl;

      output_file.close(); 
    }

    // if (argc == 1)
    // {
    //   std::cout << "\nUsage: ./flowshop <instance_file> -algorithm -starting_state -algorithms_parameters" << std::endl;
    //   std::cout << "-------------------------\n";
    //   std::cout << "-algorithm:"
    //             << "\n\t-ls: Local Search (default)"
    //             << "\n\t-vnd: Variable Neighbourhood Search" << std::endl;
    //   std::cout << "-starting-state:"
    //             << "\n\t-random: Randomized Initial State"
    //             << "\n\t-rz: Simplified RZ Heuristic (default)\n";
    //   std::cout << "-algorithms_parameters:"
    //             << "\n\t-f: First Improvement Local Search (default) (only for Local Search)";
    //   std::cout << "\n\t-b: Best Improvement Local Search (only for Local Search)";
    //   std::cout << "\n\n\t-tei: Use <Transpose, Exchange, Insert> in VND (default)";
    //   std::cout << "\n\t-tie: Use <Transpose, Insert, Exchange> in VND";
    //   std::cout << "\n-------------------------\n"
    //             << std::endl;
    // }

    // // APPLICATION CODE

    // // initialize random seed;
    // arma::arma_rng::set_seed_random();

    // // Create instance object;
    // PfspInstance instance;

    // // Read data from file;
    // if (!instance.ReadDataFromFile(argv[1]))
    //   return 1;

    // // Create problem instance;
    // PfspProblem problem(argv[1], &GetNeighboursInsert, &ComputeWCT, &RZHeuristic);

    // // Vector of neighboorhood functions to be used by VND.
    // std::vector<GetNeighbourFunction> vecNeighs = {&GetNeighboursTranspose, &GetNeighboursExchange, &GetNeighboursInsert};

    // // Create a new search engine5
    // VndEngine local_search(problem, vecNeighs);

    // local_search.PerformSearch();
    // std::cout << "initial state\n"
    //           << local_search.GetProblem().GetInitialState().GetState().t() << std::endl;
    // std::cout << "initial score\n"
    //           << local_search.GetProblem().EvaluateState(local_search.GetProblem(), local_search.GetProblem().GetInitialState()) << std::endl;
    // std::cout << "best state\n"
    //           << local_search.GetResultState().GetState().t() << std::endl;
    // std::cout << "best res\n"
    //           << local_search.GetResultValue() << std::endl;

    return 0;
  }
  catch (std::exception &e)
  {
    std::cerr << "Unhandled Exception reached the top of main: "
              << e.what() << ", application will now exit" << std::endl;
    return 1;
  }
}
