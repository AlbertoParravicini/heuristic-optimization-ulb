#include "pfsp_problem.h"

/****************************************/
/****************************************/

PfspProblem::PfspProblem(char *instance_filename, GetNeighbourFunctionPfsp neigh_fct, EvaluateStateFunctionPfsp eval_fct, InitialStateFunctionPfsp init_state)
{
  this->GetNeighbours = neigh_fct;
  this->EvaluateState = eval_fct;
  this->m_cProblemInstance = PfspInstance();
  if (!this->m_cProblemInstance.ReadDataFromFile(instance_filename))
    std::cout << "ERROR PARSING THE FILE!" << std::endl;

  this->InitState = init_state;
  this->m_cInitialState = init_state(*this);
}

/****************************************/
/****************************************/

PfspProblem::~PfspProblem()
{
}

/****************************************/
/****************************************/

PfspInstance &PfspProblem::GetProblemInstance()
{
  return this->m_cProblemInstance;
}


/****************************************/
/****************************************/

PfspState PfspProblem::GetInitialState()
{
  return this->m_cInitialState;
}

/****************************************/
/****************************************/

void PfspProblem::SetInitialState(PfspState& new_state)
{
  this->m_cInitialState = new_state;
}


