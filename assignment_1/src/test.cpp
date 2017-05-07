#include <iostream>
#include <armadillo>

using namespace std;
using namespace arma;

#include "pfsp_state.h"
#include "pfsp_problem.h"
#include "support_functions.h"

int main()
{

  PfspState a = PsfpState(arma::linspace<arma::Col<int>>(0, 4, 5));
  cout << a.GetState() << endl;

  return 0;
}