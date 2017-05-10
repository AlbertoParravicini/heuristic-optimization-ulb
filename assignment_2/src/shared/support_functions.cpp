#include "support_functions.h"

/****************************************/
/****************************************/

std::vector<std::unique_ptr<PfspState>> &GetNeighboursTest(PfspProblem &c_problem, PfspState &c_state)
{
  std::cout << "GetNeighTest" << std::endl;
  std::vector<std::unique_ptr<PfspState>> *vecNeighbours = new std::vector<std::unique_ptr<PfspState>>;
  return *vecNeighbours;
}

/****************************************/
/****************************************/

std::vector<std::unique_ptr<PfspState>> &GetNeighboursTranspose(PfspProblem &c_problem, PfspState &c_state)
{
  std::vector<std::unique_ptr<PfspState>> *vecNeighbours = new std::vector<std::unique_ptr<PfspState>>(c_state.GetState().n_elem - 1);
  // Each neighbour is built by swapping the value of element i with the next element i+1
  for (int i = 0; i < c_state.GetState().n_elem - 1; i++)
  {
    arma::Col<int> c_Neigh(c_state.GetState());
    c_Neigh.swap_rows(i, i + 1);
    (*vecNeighbours)[i] = std::unique_ptr<PfspState>(new PfspState(c_Neigh));
  }
  return *vecNeighbours;
}

/****************************************/
/****************************************/

std::vector<std::unique_ptr<PfspState>> &GetNeighboursExchange(PfspProblem &c_problem, PfspState &c_state)
{
  int nNumberOfJobs = c_state.GetState().n_elem;
  // number of neighbours: n * (n - 1) / 2;
  std::vector<std::unique_ptr<PfspState>> *vecNeighbours = new std::vector<std::unique_ptr<PfspState>>();
  // Similar to transpose, but here we swap every combination of elements.
  for (int i = 0; i < nNumberOfJobs - 1; i++)
  {
    for (int j = i + 1; j < nNumberOfJobs; j++)
    {
      arma::Col<int> c_Neigh(c_state.GetState());
      c_Neigh.swap_rows(i, j);
      (*vecNeighbours).push_back(std::unique_ptr<PfspState>(new PfspState(c_Neigh)));
    }
  }
  return *vecNeighbours;
}

/****************************************/
/****************************************/

std::vector<std::unique_ptr<PfspState>> &GetNeighboursInsert(PfspProblem &c_problem, PfspState &c_state)
{
  int nNumberOfJobs = c_state.GetState().n_elem;

  std::vector<std::unique_ptr<PfspState>> *vecNeighbours = new std::vector<std::unique_ptr<PfspState>>();
  // Each neighbour is built by removing an element from the vector,
  // then re-adding it somewhere else.
  for (int i = 0; i < nNumberOfJobs; i++)
  {
    // Save the element to move.
    arma::Col<int> vecElemToInsert = {c_state.GetState()(i)};
    for (int j = 0; j < nNumberOfJobs; j++)
    {
      if (j != i)
      {
        arma::Col<int> vecNeighVec(c_state.GetState());
        // Remove the element i.
        vecNeighVec.shed_row(i);
        // Re-add the element in position j.
        vecNeighVec.insert_rows(j, vecElemToInsert);
        (*vecNeighbours).push_back(std::unique_ptr<PfspState>(new PfspState(vecNeighVec)));
      }
    }
  }

  return *vecNeighbours;
}

/****************************************/
/****************************************/

std::vector<std::unique_ptr<PfspState>> &GetNeighboursTransposeFI(PfspProblem &c_problem, PfspState &c_state)
{
  std::unique_ptr<PfspState> cCurrBest(&c_state);
  // Each neighbour is built by swapping the value of element i with the next element i+1
  for (int i = 0; i < c_state.GetState().n_elem - 1; i++)
  {
    arma::Col<int> c_Neigh(c_state.GetState());
    c_Neigh.swap_rows(i, i + 1);

    std::unique_ptr<PfspState>cTempNeigh = std::unique_ptr<PfspState>(new PfspState(c_Neigh));
    cTempNeigh->SetStateValue(c_problem.EvaluateState(c_problem, *cTempNeigh));
    // A better candidate solution has been found, break the cycle.
    if (cTempNeigh->GetStateValue() < cCurrBest->GetStateValue())
    {
      cCurrBest = std::move(cTempNeigh);
      break;
    }
  }
  std::vector<std::unique_ptr<PfspState>> *vecNeighbours = new std::vector<std::unique_ptr<PfspState>>(1);
  (*vecNeighbours)[0] = std::move(cCurrBest);
  return *vecNeighbours;
}

/****************************************/
/****************************************/

std::vector<std::unique_ptr<PfspState>> &GetNeighboursExchangeFI(PfspProblem &c_problem, PfspState &c_state)
{
  std::unique_ptr<PfspState> cCurrBest(&c_state);

  int nNumberOfJobs = c_state.GetState().n_elem;

  // Similar to transpose, but here we swap every combination of elements.
  for (int i = 0; i < nNumberOfJobs - 1; i++)
  {
    for (int j = i + 1; j < nNumberOfJobs; j++)
    {
      arma::Col<int> c_Neigh(c_state.GetState());
      c_Neigh.swap_rows(i, j);

      std::unique_ptr<PfspState>cTempNeigh = std::unique_ptr<PfspState>(new PfspState(c_Neigh));
      cTempNeigh->SetStateValue(c_problem.EvaluateState(c_problem, *cTempNeigh));
      // A better candidate solution has been found, break the cycle.
      if (cTempNeigh->GetStateValue() < cCurrBest->GetStateValue())
      {
        cCurrBest = std::move(cTempNeigh);
        break;
      }
    }
  }

  std::vector<std::unique_ptr<PfspState>> *vecNeighbours = new std::vector<std::unique_ptr<PfspState>>(1);
  (*vecNeighbours)[0] = std::move(cCurrBest);
  return *vecNeighbours;
}

/****************************************/
/****************************************/

std::vector<std::unique_ptr<PfspState>> &GetNeighboursInsertFI(PfspProblem &c_problem, PfspState &c_state)
{
  std::unique_ptr<PfspState> cCurrBest(&c_state);

  int nNumberOfJobs = c_state.GetState().n_elem;

  // Each neighbour is built by removing an element from the vector,
  // then re-adding it somewhere else.
  for (int i = 0; i < nNumberOfJobs; i++)
  {
    // Save the element to move.
    arma::Col<int> vecElemToInsert = {c_state.GetState()(i)};
    for (int j = 0; j < nNumberOfJobs; j++)
    {
      if (j != i)
      {
        arma::Col<int> c_Neigh(c_state.GetState());
        // Remove the element i.
        c_Neigh.shed_row(i);
        // Re-add the element in position j.
        c_Neigh.insert_rows(j, vecElemToInsert);

        std::unique_ptr<PfspState>cTempNeigh = std::unique_ptr<PfspState>(new PfspState(c_Neigh));
        cTempNeigh->SetStateValue(c_problem.EvaluateState(c_problem, *cTempNeigh));
        // A better candidate solution has been found, break the cycle.
        if (cTempNeigh->GetStateValue() < cCurrBest->GetStateValue())
        {
          cCurrBest = std::move(cTempNeigh);
          break;
        }
      }
    }
  }

  std::vector<std::unique_ptr<PfspState>> *vecNeighbours = new std::vector<std::unique_ptr<PfspState>>(1);
  (*vecNeighbours)[0] = std::move(cCurrBest);
  return *vecNeighbours;
}

/****************************************/
/****************************************/

const long int EvaluateStateTest(PfspProblem &c_problem, PfspState &c_state)
{
  std::cout << "EvalTest" << std::endl;
  return 0;
}

/****************************************/
/****************************************/

const long int ComputeWCT(PfspProblem &c_problem, PfspState &c_state)
{
  // Extract the parameters from the problem.
  const arma::Col<int> solution = c_state.GetState();
  PfspInstance c_instance = c_problem.GetProblemInstance();
  int nNumberOfJobs = c_instance.GetNumberOfJobs();
  int nNumberOfMachines = c_instance.GetNumberOfMachines();
  arma::Mat<long int> cProcessingTimeMatrix = c_instance.GetProcessingTimeMatrix();
  arma::Col<long int> vecPriorities = c_instance.GetPriorities();

  // Evaluate the WCT.
  long int result = ComputeWCTInternals(solution, nNumberOfJobs, nNumberOfMachines, cProcessingTimeMatrix, vecPriorities);
  // Store the value inside the state.
  c_state.SetStateValue(result);
  return result;
}

/****************************************/
/****************************************/

const long int ComputeWCTInternals(const arma::Col<int> &solution, int n_number_of_jobs, int n_number_of_machines, arma::Mat<long int> &c_processing_time_matrix, arma::Col<long int> &vec_priorities)
{
  long int nWCT = 0;

  // Build the vector with the end times of the first machine.
  // These times are simply given by the sum of the processing numbers;

  // Vector that contains, for every job, the end times at the previous machine.
  arma::Col<long int> vecPreviousMachineEndTimes(n_number_of_jobs);

  // 1st Machine;
  vecPreviousMachineEndTimes(0) = c_processing_time_matrix(solution(0), 0);

  // Compute the completion times for the first machine.
  // The first machine can start to work on a job as soon as it ends processing the previous one,
  // So we can just do a rolling sum.
  for (int j = 1; j < n_number_of_jobs; j++)
  {
    vecPreviousMachineEndTimes(j) = vecPreviousMachineEndTimes(j - 1) + c_processing_time_matrix(solution(j), 0);
  }

  // Compute the completion times for all the other machines in the processing line.
  for (int m = 1; m < n_number_of_machines; m++)
  {
    // The first job will start the next phase as soon as the previous is over.
    vecPreviousMachineEndTimes(0) += c_processing_time_matrix(solution(0), m);
    // A job can start to be processed by a machine if it ended the previous phase, and the next machine is ready.
    for (int j = 1; j < n_number_of_jobs; j++)
    {
      vecPreviousMachineEndTimes(j) = c_processing_time_matrix(solution(j), m) + std::max(vecPreviousMachineEndTimes(j - 1), vecPreviousMachineEndTimes(j));
    }
  }

  // Compute the weighted completion time.
  for (int j = 0; j < n_number_of_jobs; j++)
    nWCT += vecPreviousMachineEndTimes(j) * vec_priorities(solution(j));

  return nWCT;
}

/****************************************/
/****************************************/

arma::Col<int> RandomPermutation(int number_of_jobs)
{
  // Create a vector 1..n
  arma::Col<int> solution = arma::linspace<arma::Col<int>>(0, number_of_jobs - 1, number_of_jobs);
  // Shuffle the previous vector.
  return arma::shuffle(solution);
}

/****************************************/
/****************************************/

PfspState RandomInitialState(PfspProblem &c_problem)
{
  PfspInstance c_instance = c_problem.GetProblemInstance();
  PfspState cInitialState = PfspState(RandomPermutation(c_instance.GetNumberOfJobs()));
  // Store the evaluation of the first state.
  cInitialState.SetStateValue(c_problem.EvaluateState(c_problem, cInitialState));
  return cInitialState;
}

/****************************************/
/****************************************/

PfspState RZHeuristic(PfspProblem &c_problem)
{
  PfspInstance c_instance = c_problem.GetProblemInstance();
  int nNumberOfJobs = c_instance.GetNumberOfJobs();
  arma::Col<int> vecSolution = arma::linspace<arma::Col<int>>(0, nNumberOfJobs - 1, nNumberOfJobs);

  // Compute the weighted sum of processing times;
  arma::Col<double> vecWeights(nNumberOfJobs);
  for (int j = 0; j < nNumberOfJobs; j++)
  {
    vecWeights(j) = arma::sum(c_instance.GetProcessingTimeMatrix().row(j)) / (double)c_instance.GetPriorities()(j);
  }

  // Sort the jobs based on their weights;
  // An additional data structure is required to do so.
  struct ValuePair
  {
    int job_number;
    double value;
  };

  struct Comparator
  {
    bool operator()(ValuePair const &a, ValuePair const &b)
    {
      return a.value < b.value;
    }
  };

  std::vector<ValuePair> pairs(nNumberOfJobs);
  for (int i = 0; i < nNumberOfJobs; i++)
  {
    pairs[i] = ValuePair();
    pairs[i].job_number = vecSolution(i);
    pairs[i].value = vecWeights(i);
  }
  // Sort the jobs.
  std::sort(pairs.begin(), pairs.end(), Comparator());
  std::cout << std::endl;

  // Store the current partial candidates;
  std::vector<arma::Col<int>> vecCandidates;
  // Store the current best candidate; new candidates will be built from it;
  arma::Col<int> vecBestCandidate;

  for (int i = 0; i < nNumberOfJobs; i++)
  {
    arma::Col<int> vecElemToInsert = {pairs[i].job_number};
    // Build the new candidates by inserting a new job in the best partial candidate;
    for (int j = 0; j < vecBestCandidate.n_elem + 1; j++)
    {
      arma::Col<int> vecTempCand(vecBestCandidate);
      vecTempCand.insert_rows(j, vecElemToInsert);
      vecCandidates.push_back(vecTempCand);
    }
    // Evaluate the temporay candidates to find the best partial candidate;
    long int nCurrentBestValue = std::numeric_limits<long int>::max();
    for (arma::Col<int> vecTempCand : vecCandidates)
    {
      long int nTempValue = ComputeWCTInternals(
          vecTempCand,
          vecTempCand.n_elem,
          c_instance.GetNumberOfMachines(),
          c_instance.GetProcessingTimeMatrix(),
          c_instance.GetPriorities());
      if (nTempValue < nCurrentBestValue)
      {
        vecBestCandidate = vecTempCand;
        nCurrentBestValue = nTempValue;
      }
    }
    vecCandidates.clear();
  }
  PfspState cInitialState = PfspState(vecBestCandidate);
  cInitialState.SetStateValue(c_problem.EvaluateState(c_problem, cInitialState));
  return cInitialState;
}


/****************************************/
/****************************************/

PfspState DefaultDestrConstr(PfspProblem &c_problem, PfspState &c_initial_state, const int n_destr_vec_size)
{
  PfspInstance c_instance = c_problem.GetProblemInstance();

  arma::Col<int> vecTempState(c_initial_state.GetState());
  arma::Col<int> vecRemoved = arma::zeros<arma::Col<int>>(n_destr_vec_size);

  // Store the value of the temporary candidate.
  long int nCurrentBestValue = std::numeric_limits<long int>::max();

  // Remove "n_destr_vec_size" random elements from the input state, and put them into a vector.
  for(int i = 0; i < n_destr_vec_size; i++)
  {
    // Choose a random element to remove.
    int j = rand() % (int)(vecTempState.n_elem);
    vecRemoved(i) = vecTempState(j);
    // Remove the element.
    vecTempState.shed_row(j);
  }

  // Reinsert each element that was removed by looking at the best possible insertion.
  for(int i = 0; i < vecRemoved.n_elem; i++)
  {
    // Save as vector the element to insert.
    arma::Col<int> vecElemToInsert = {vecRemoved(i)};
    // Save the current best score and index.
    nCurrentBestValue = std::numeric_limits<long int>::max();
    int nBestIndexToInsert = 0;

    // Reinsert the removed element in all possible places, find the best one.
    for (int j = 0; j < vecTempState.n_elem + 1; j++)
    {
      // Try to insert the element.
      vecTempState.insert_rows(j, vecElemToInsert);

      // Evaluate this new state.
      long int nTempValue = ComputeWCTInternals(
          vecTempState,
          vecTempState.n_elem,
          c_instance.GetNumberOfMachines(),
          c_instance.GetProcessingTimeMatrix(),
          c_instance.GetPriorities());
      if (nTempValue < nCurrentBestValue)
      {
        nBestIndexToInsert = j;
        nCurrentBestValue = nTempValue;
      }
      // Remove the inserted element, try the next position at the next iteration.
      vecTempState.shed_row(j);
    }
   
    // Add the element at the best position that was found.
    vecTempState.insert_rows(nBestIndexToInsert, vecElemToInsert);
  }


  // Return the new best state that was found.
  PfspState cNewState = PfspState(vecTempState);
  // Store its value, so it's not recomputed later.
  cNewState.SetStateValue(nCurrentBestValue);
  return cNewState;
}

/****************************************/
/****************************************/

float DefaultTemperature(PfspProblem &c_problem, const float lambda)
{
    int nNumberOfJobs = c_problem.GetProblemInstance().GetNumberOfJobs();
    int nNumberOfMachines = c_problem.GetProblemInstance().GetNumberOfMachines();
    return lambda * arma::accu(c_problem.GetProblemInstance().GetProcessingTimeMatrix()) / (10 * nNumberOfJobs * nNumberOfMachines);
}

/****************************************/
/****************************************/

arma::Col<float> UniformWeights(arma::Col<float> vec_states_scores)
{
  return (1 / vec_states_scores) / arma::sum(1 / vec_states_scores);
}

/****************************************/
/****************************************/

arma::Col<float> SoftMaxWeights(arma::Col<float> vec_states_scores)
{
  arma::Col<double> v = arma::conv_to<arma::Col<double>>::from(vec_states_scores);
  return arma::conv_to<arma::Col<float>>::from((1 / arma::exp(v)) / arma::sum(1 / arma::exp(v)));
}

/****************************************/
/****************************************/

arma::Col<float> UniformWeights10(arma::Col<float> vec_states_scores)
{
  return (1 / arma::pow(vec_states_scores, 10)) / arma::as_scalar(arma::sum(1 / arma::pow(vec_states_scores, 10)));
}

/****************************************/
/****************************************/

std::vector<PfspState> PMXCrossover(PfspState &c_parent_1, PfspState &c_parent_2)
{
  arma::Col<int> vecParent1 = arma::Col<int>(c_parent_1.GetState());
  arma::Col<int> vecParent2 = arma::Col<int>(c_parent_2.GetState());

  // Pick a random crossover point;
  int nXOverPoint = (rand() % (int)(vecParent1.n_elem + 1));

  // Generate (empty) offsprings;
  arma::Col<int> vecOffspring1 = arma::zeros<arma::Col<int>>(vecParent1.n_elem);
  arma::Col<int> vecOffspring2 = arma::zeros<arma::Col<int>>(vecParent1.n_elem);

  // Build offspring 1;
  for (int i = 0; i < nXOverPoint; i++)
  {
    vecOffspring1(i) = vecParent2(i);
    // Swap the elements;
    vecParent1.elem(arma::find(vecParent1 == vecParent2(i))).fill(vecParent1(i));
  }
  for (int i = nXOverPoint; i < vecOffspring1.n_elem; i++)
  {
    vecOffspring1(i) = vecParent1(i);
  }

  // Build offspring 2;
  vecParent1 = arma::Col<int>(c_parent_1.GetState());
  vecParent2 = arma::Col<int>(c_parent_2.GetState());
  for (int i = 0; i < nXOverPoint; i++)
  {
    vecOffspring2(i) = vecParent1(i);
    // Swap the elements;
    vecParent2.elem(arma::find(vecParent2 == vecParent1(i))).fill(vecParent2(i));
  }
  for (int i = nXOverPoint; i < vecOffspring2.n_elem; i++)
  {
    vecOffspring2(i) = vecParent2(i);
  }

  return {PfspState(vecOffspring1), PfspState(vecOffspring2)};
}

/****************************************/
/****************************************/

PfspState TransposeMutation(PfspState &c_state)
{
  PfspState cMutatedState = PfspState(c_state.GetState());
  // Pick 2 random elements;
  int i = (rand() % (int)(cMutatedState.GetState().n_elem)); 
  int j = (rand() % (int)(cMutatedState.GetState().n_elem)); 
  cMutatedState.GetState().swap_rows(i, j);

  return cMutatedState;
}

