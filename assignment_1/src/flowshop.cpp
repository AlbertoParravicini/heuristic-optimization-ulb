

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include "pfspinstance.h"
#include "pfsp_state.h"
#include "pfsp_problem.h"
#include "local_search_engine.h"
#include "vnd_engine.h"

#include "support_functions.h"

int main(int argc, char *argv[])
{
  if (argc == 1)
  {
    std::cout << "\nUsage: ./flowshop <instance_file> -algorithm -starting_state -algorithms_parameters" << std::endl;
    std::cout << "-------------------------\n";
    std::cout << "-algorithm:" << "\n\t-ls: Local Search (default)" << "\n\t-vnd: Variable Neighbourhood Search" << std::endl;
    std::cout << "-starting-state:" << "\n\t-random: Randomized Initial State" << "\n\t-rz: Simplified RZ Heuristic (default)\n";
    std::cout << "-algorithms_parameters:" << "\n\t-f: First Improvement Local Search (default) (only for Local Search)"; 
    std::cout << "\n\t-b: Best Improvement Local Search (only for Local Search)";
    std::cout << "\n\n\t-tei: Use <Transpose, Exchange, Insert> in VND (default)";
    std::cout << "\n\t-tie: Use <Transpose, Insert, Exchange> in VND";
    std::cout << "\n-------------------------\n" << std::endl;

    return 0;
  }

  // initialize random seed;
  arma::arma_rng::set_seed_random();

  // Create instance object;
  PfspInstance instance;

  // Read data from file;
  if (!instance.ReadDataFromFile(argv[1]))
    return 1;

  // Create problem instance;
  PfspProblem problem(argv[1], &GetNeighboursInsert, &ComputeWCT, &RZHeuristic);

  // Vector of neighboorhood functions to be used by VND.
  std::vector<GetNeighbourFunction> vecNeighs = {&GetNeighboursTranspose, &GetNeighboursExchange, &GetNeighboursInsert};

  // Create a new search engine5
  VndEngine local_search(problem, vecNeighs);

  local_search.PerformSearch();
  std::cout << "initial state\n" << local_search.GetProblem().GetInitialState().GetState().t() << std::endl;
  std::cout << "initial score\n" << local_search.GetProblem().EvaluateState(local_search.GetProblem(), local_search.GetProblem().GetInitialState()) << std::endl;
  std::cout << "best state\n" << local_search.GetResultState().GetState().t() << std::endl;
  std::cout << "best res\n" << local_search.GetResultValue() << std::endl;


  return 0;
}
