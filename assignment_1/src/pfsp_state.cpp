#include "pfsp_state.h"

/****************************************/
/****************************************/

PfspState::PfspState()
{
  this->m_vecState = arma::zeros<arma::Col<int>>(0);
  this->m_nStateValue = std::numeric_limits<long int>::max();
}

/****************************************/
/****************************************/

PfspState::PfspState(const int n_number_of_jobs)
{
  this->m_vecState = arma::zeros<arma::Col<int>>(n_number_of_jobs);
  this->m_nStateValue = std::numeric_limits<long int>::max();
}

/****************************************/
/****************************************/

PfspState::PfspState(arma::Col<int> vec_new_state)
{
  this->m_vecState = vec_new_state;
  this->m_nStateValue = std::numeric_limits<long int>::max();
}

/****************************************/
/****************************************/

PfspState::~PfspState()
{
}

/****************************************/
/****************************************/

arma::Col<int>& PfspState::GetState()
{
  return this->m_vecState;
}


