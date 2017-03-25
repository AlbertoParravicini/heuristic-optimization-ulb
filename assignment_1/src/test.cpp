#include <iostream>
#include <armadillo>

using namespace std;
using namespace arma;

#include "state.h"

int main()
  {
  mat A = randu<mat>(4,5);
  mat B = randu<mat>(4,5);

  // vec c = arma::zeros(3);
  // vec d(c);
  // cout << c << endl;
  // cout << d << endl;
  
  State a = State(3);
  cout << a.GetState() << endl;

  cout << A*B.t() << endl;
  
  return 0;
  }