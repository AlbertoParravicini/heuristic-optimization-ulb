#ifndef _PFSPSTATE_H_
#define _PFSPSTATE_H_



#include <armadillo>
#include <limits>

class PfspState 
{
  private:
    arma::Col<int> m_vecState;
    long int m_nStateValue;

  public:
    PfspState();
    PfspState(const int n_number_of_jobs);
    PfspState(arma::Col<int> vec_new_state);
    ~PfspState();

    arma::Col<int>& GetState();
    const long int GetStateValue() const;
    void SetStateValue(const long int n_new_state_value); 
};

#endif
