#include "pfsp_problem.h"

/****************************************/
/****************************************/

PfspProblem::PfspProblem(char* instance_filename, GetNeighbourFunction neigh_fct, EvaluateStateFunction eval_fct, InitialStateFunction init_state)
{
  this->GetNeighbours = neigh_fct;
  this->EvaluateState = eval_fct;
  this->m_cProblemInstance = PfspInstance();
  if (!this->m_cProblemInstance.ReadDataFromFile(instance_filename))
    std::cout << "ERROR PARSING THE FILE!" << std::endl;
    
  // this->m_cInitialState = PfspState(m_cProblemInstance.GetNumberOfJobs());
  this->m_cInitialState = init_state(this->m_cProblemInstance);
}

/****************************************/
/****************************************/

PfspProblem::~PfspProblem()
{
}

/****************************************/
/****************************************/

PfspState& PfspProblem::GetInitialState()
{
  return this->m_cInitialState;
}

/****************************************/
/****************************************/

PfspInstance& PfspProblem::GetProblemInstance()
{
  return this->m_cProblemInstance;
}
