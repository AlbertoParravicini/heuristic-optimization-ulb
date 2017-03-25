#include "support_functions.h"

/****************************************/
/****************************************/

std::vector<PfspState *> &GetNeighboursTest(PfspState &c_state)
{
  std::cout << "GetNeighTest" << std::endl;
  std::vector<PfspState *> *vecNeighbours = new std::vector<PfspState *>;
  return *vecNeighbours;
}

/****************************************/
/****************************************/

std::vector<PfspState *> &GetNeighboursTranspose(PfspState &c_state)
{
  std::vector<PfspState *> *vecNeighbours = new std::vector<PfspState *>(c_state.GetState().n_elem - 1);
  for (int i = 0; i < c_state.GetState().n_elem - 1; i++)
  {
    arma::Col<int> c_Neigh(c_state.GetState());
    c_Neigh.swap_rows(i, i + 1);
    (*vecNeighbours)[i] = new PfspState(c_Neigh);
  }
  return *vecNeighbours;
}

/****************************************/
/****************************************/

std::vector<PfspState *> &GetNeighboursExchange(PfspState &c_state)
{
  int nNumberOfJobs = c_state.GetState().n_elem;
  // number of neighbours: n * (n - 1) / 2;
  std::vector<PfspState *> *vecNeighbours = new std::vector<PfspState *>();
  for (int i = 0; i < nNumberOfJobs - 1; i++)
  {
    for (int j = i + 1; j < nNumberOfJobs; j++)
    {
      arma::Col<int> c_Neigh(c_state.GetState());
      c_Neigh.swap_rows(i, j);
      (*vecNeighbours).push_back(new PfspState(c_Neigh));
    }
  }
  return *vecNeighbours;
}

/****************************************/
/****************************************/

std::vector<PfspState *> &GetNeighboursInsert(PfspState &c_state)
{
  int nNumberOfJobs = c_state.GetState().n_elem;

  std::vector<PfspState *> *vecNeighbours = new std::vector<PfspState *>();
  for (int i = 0; i < nNumberOfJobs; i++)
  {
    arma::Col<int> vecElemToInsert = {c_state.GetState()(i)};
    for (int j = 0; j < nNumberOfJobs; j++)
    {
      if (j != i)
      {
        arma::Col<int> vecNeighVec(c_state.GetState());
        vecNeighVec.shed_row(i);
        vecNeighVec.insert_rows(j, vecElemToInsert);
        (*vecNeighbours).push_back(new PfspState(vecNeighVec));
      }
    }
  }

  return *vecNeighbours;
}

/****************************************/
/****************************************/

const long int EvaluateStateTest(PfspState &c_state)
{
  std::cout << "EvalTest" << std::endl;
  return 0;
}

/****************************************/
/****************************************/

const long int ComputeWCT(PfspProblem &c_problem, PfspState &c_state)
{
  const arma::Col<int> solution = c_state.GetState();
  PfspInstance c_instance = c_problem.GetProblemInstance();
  int nNumberOfJobs = c_instance.GetNumberOfJobs();
  int nNumberOfMachines = c_instance.GetNumberOfMachines();
  arma::Mat<long int> cProcessingTimeMatrix = c_instance.GetProcessingTimeMatrix();
  arma::Col<long int> vecPriorities = c_instance.GetPriorities();

  return ComputeWCTInternals(solution, nNumberOfJobs, nNumberOfMachines, cProcessingTimeMatrix, vecPriorities);
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


  for (int j = 1; j < n_number_of_jobs; j++)
  {
    vecPreviousMachineEndTimes(j) = vecPreviousMachineEndTimes(j - 1) + c_processing_time_matrix(solution(j), 0);
  }


  for (int m = 1; m < n_number_of_machines; m++)
  {
    // The first job will start the next phase as soon as the previous is over.
    vecPreviousMachineEndTimes(0) += c_processing_time_matrix(solution(0), m);

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
  arma::Col<int> solution = arma::linspace<arma::Col<int>>(0, number_of_jobs - 1, number_of_jobs);
  return arma::shuffle(solution);
}

/****************************************/
/****************************************/

PfspState RandomInitialState(PfspInstance &c_instance)
{
  return PfspState(RandomPermutation(c_instance.GetNumberOfJobs()));
}

/****************************************/
/****************************************/

PfspState RZHeuristic(PfspInstance &c_instance)
{
  int nNumberOfJobs = c_instance.GetNumberOfJobs();
  arma::Col<int> vecSolution = arma::linspace<arma::Col<int>>(0, nNumberOfJobs - 1, nNumberOfJobs);

  // Compute the weighted sum of processing times;
  arma::Col<double> vecWeights(nNumberOfJobs);
  for (int j = 0; j < nNumberOfJobs; j++)
  {
    vecWeights(j) = arma::sum(c_instance.GetProcessingTimeMatrix().row(j)) / (double)c_instance.GetPriorities()(j);
  }

  // Sort the jobs based on their weights;

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

  return PfspState(vecBestCandidate);
}
