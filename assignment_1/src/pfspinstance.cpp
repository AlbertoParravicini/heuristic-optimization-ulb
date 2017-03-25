#include "pfspinstance.h"
#include <iostream>
#include <fstream>

/****************************************/
/****************************************/

PfspInstance::PfspInstance()
{
}

/****************************************/
/****************************************/

PfspInstance::~PfspInstance()
{
}

/****************************************/
/****************************************/

const int PfspInstance::GetNumberOfJobs() const
{
  return m_nNumberOfJobs;
}

/****************************************/
/****************************************/

const int PfspInstance::GetNumberOfMachines() const
{
  return m_nNumberOfMachines;
}

/****************************************/
/****************************************/

arma::Col<long int>& PfspInstance::GetDueDates() 
{
  return m_vecDueDates;
}

/****************************************/
/****************************************/

arma::Col<long int>& PfspInstance::GetPriorities() 
{
  return m_vecPriorities;
}

/****************************************/
/****************************************/

arma::Mat<long int>& PfspInstance::GetProcessingTimeMatrix()
{
  return m_cProcessingTimeMatrix;
}

/****************************************/
/****************************************/

void PfspInstance::InstantiateMatrix(const int number_of_jobs, const int number_of_machines)
{
  m_vecDueDates.zeros(number_of_jobs);
  m_vecPriorities.zeros(number_of_jobs);
  m_cProcessingTimeMatrix.zeros(number_of_jobs, number_of_machines);
}

/****************************************/
/****************************************/

const bool PfspInstance::ReadDataFromFile(char* fileName)
{
  bool bEverythingOk = true;
  int j, m; // iterators
  // Used to temporarily store unused values from the file.
  long int nDiscardValue;
  std::string str;
  std::ifstream fileIn;
  char *aux2;
  char fileNameOK[100] = "";

  aux2 = (strrchr(fileName, '/'));

  if (aux2 == NULL)
    aux2 = fileName;
  else
    aux2 += 1;

  strcat(fileNameOK, aux2);

  std::cout << "name : " << fileNameOK << std::endl;
  std::cout << "file : " << fileName << std::endl;

  fileIn.open(fileName);

  if (fileIn.is_open())
  {
    std::cout << "File " << fileName << " is now open, start to read..." << std::endl;

    fileIn >> m_nNumberOfJobs;
    std::cout << "Number of jobs : " << m_nNumberOfJobs << std::endl;
    fileIn >> m_nNumberOfMachines;
    std::cout << "Number of machines : " << m_nNumberOfMachines << std::endl;

    std::cout << "Allow memory for the matrix..." << std::endl;
    // Instantiate the process time matrix and the vectors.
    InstantiateMatrix(m_nNumberOfJobs, m_nNumberOfMachines);
    std::cout << "Memory allowed." << std::endl;

    std::cout << "Start to read matrix..." << std::endl;
    for (j = 0; j < m_nNumberOfJobs; j++)
    {
      for (m = 0; m < m_nNumberOfMachines; m++)
      {
				// In each line we have tuples of the form (nth_machine, machine_number), e.g. 1 10 2 20 3 15, etc...
				// We can discard the value used to store unused th_machine value, as it's not useful!
				fileIn >> nDiscardValue; // The number of each machine, not important !
				fileIn >> m_cProcessingTimeMatrix(j, m); // Process Time
      }
    }
    fileIn >> str; // "Reldue": this is not read

    for (j = 0; j < m_nNumberOfJobs; j++)
    {
      fileIn >> nDiscardValue; // -1, discarded
      fileIn >> m_vecDueDates(j);

      fileIn >> nDiscardValue; // -1, discarded
      fileIn >> m_vecPriorities(j);
    }

    std::cout << "All is read from file." << std::endl;
    fileIn.close();
  }
  else
  {
    std::cout << "ERROR. file:pfspInstance.cpp, method:readDataFromFile, " << "error while opening file " << fileName << std::endl;

    bEverythingOk = false;
  }

  return bEverythingOk;
}

/****************************************/
/****************************************/

const long int PfspInstance::ComputeWCT(const arma::Col<int> &solution) const
{
  long int nWCT = 0;

  // Build the vector with the end times of the first machine.
  // These times are simply given by the sum of the processing numbers;

  // Vector that contains, for every job, the end times at the previous machine.
  arma::Col<long int> vecPreviousMachineEndTimes(m_nNumberOfJobs);

  // 1st Machine;
  vecPreviousMachineEndTimes(0) = m_cProcessingTimeMatrix(solution(0), 0);
  for(int j = 1; j < m_nNumberOfJobs; j++)
  {
    vecPreviousMachineEndTimes(j) = vecPreviousMachineEndTimes(j-1) + m_cProcessingTimeMatrix(solution(j), 0);
  } 

  for(int m = 1; m < m_nNumberOfMachines; m++)
  {
    // The first job will start the next phase as soon as the previous is over.
    vecPreviousMachineEndTimes(0) += m_cProcessingTimeMatrix(solution(0), m);

    for(int j = 1; j < m_nNumberOfJobs; j++)
    {
      vecPreviousMachineEndTimes(j) = m_cProcessingTimeMatrix(solution(j), m) 
                                      + std::max(vecPreviousMachineEndTimes(j-1), vecPreviousMachineEndTimes(j));
    } 
  }
  
  // Compute the weighted completion time.
  for (int j = 0; j < m_nNumberOfJobs; j++)
    nWCT += vecPreviousMachineEndTimes(j) * m_vecPriorities(solution(j));

  return nWCT;
}

