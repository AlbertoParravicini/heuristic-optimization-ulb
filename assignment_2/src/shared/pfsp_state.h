#ifndef _PFSPSTATE_H_
#define _PFSPSTATE_H_


//#include "state.h"
#include <armadillo>
#include <limits>

/*
* PfspState:
*   This class wraps the implementation of the candidate solutions
*   for the PFSP problem.
*   It can also store the score of the candidate solution,
*   which could be useful in some algorithms.
*/
class PfspState //: public State
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
    long int GetStateValue();
    void SetStateValue(long int n_new_value);
};

#endif
