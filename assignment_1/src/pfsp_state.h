#ifndef _PFSPSTATE_H_
#define _PFSPSTATE_H_


//#include "state.h"
#include <armadillo>
#include <limits>

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
};

#endif
