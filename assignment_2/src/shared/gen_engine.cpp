#include "gen_engine.h"

/****************************************/
/****************************************/

GenEngine::GenEngine(
                  PfspProblem &c_problem,
                  long int n_max_steps,
                  )
{
  this->m_pcProblem = &c_problem;
  this->m_nMaxSteps = n_max_steps;
}

/****************************************/
/****************************************/

GenEngine::~GenEngine()
{
}

/****************************************/
/****************************************/

const long int GenEngine::GetMaxSteps() const
{
  return this->m_nMaxSteps;
}

/****************************************/
/****************************************/

void GenEngine::SetMaxSteps(const long int n_max_steps)
{
  this->m_nMaxSteps = n_max_steps;
}

/****************************************/
/****************************************/

void GenEngine::PerformSearch()
{
  
}