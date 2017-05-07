#ifndef _PFSPINSTANCEWT_H_
#define _PFSPINSTANCEWT_H_


#include <armadillo>


class PfspInstance
{
  private:
    int m_nNumberOfJobs;
    int m_nNumberOfMachines;

    // Vector of due dates;
    arma::Col<long int> m_vecDueDates;
    // Vector of priorities;
    arma::Col<long int> m_vecPriorities; 


  public:
      // Matrix of processing times;
    arma::Mat<long int> m_cProcessingTimeMatrix;
    PfspInstance();
    ~PfspInstance();

    const int GetNumberOfJobs() const;
    const int GetNumberOfMachines() const;

    // Access the processing time matrix and the vectors;
    // These objects are writable, for simplicity.
    arma::Col<long int>& GetDueDates();
    arma::Col<long int>& GetPriorities();
    arma::Mat<long int>& GetProcessingTimeMatrix();

    // Instantiate the processing times matrix and the vectors;
    void InstantiateMatrix(const int number_of_jobs, const int number_of_machines);

    // Read data from a file;
    const bool ReadDataFromFile(char* fileName);

    // Compute the weighted completion time;
    const long int ComputeWCT(const arma::Col<int> &solution) const;
};


#endif
