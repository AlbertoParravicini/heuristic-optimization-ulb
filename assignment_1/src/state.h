#ifndef _STATE_H_
#define _STATE_H_


class State 
{
  private:
    long int m_nStateValue;

  public:
    State();
    virtual ~State();

    const long int GetStateValue() const;
    void SetStateValue(const long int n_new_state_value); 
};

#endif
