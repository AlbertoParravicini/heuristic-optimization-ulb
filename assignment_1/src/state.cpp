#include "state.h"

/****************************************/
/****************************************/

State::State()
{
}

/****************************************/
/****************************************/

State::~State()
{
}

/****************************************/
/****************************************/

const long int State::GetStateValue() const
{
 return this->m_nStateValue;
}

/****************************************/
/****************************************/

void State::SetStateValue(const long int n_new_state_value)
{
  this->m_nStateValue = n_new_state_value;
}


