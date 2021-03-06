/*
 
This file is part of AVCB.

AVCB is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

AVCB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with AVCB.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "version.h"
#include "sequence.h"
#include "variable.h"
#include "varmanager.h"
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>

std::string getCoefficient(double i) {
  if (i == 1)
    return "";
  std::stringstream s;
  if (i < 0)
    s << -i;
  else
    s << i;
  return s.str();
}

// this function is SUPPOSED to do the quadratic function, but it's broken. Fortunately, it's only a test case fo the action command.
score_t testAction(double a, double b, double c) {
  double sqrtTerm = std::pow(b,2) - (4*a*c);
  sqrtTerm = std::sqrt(sqrtTerm);
  double result1 = (0-b)+sqrtTerm;
  double result2 = (0-b)-sqrtTerm;
  result1 /= (2*a);
  result2 /= (2*a);
  std::cout << getCoefficient(a) << "x^2 " << (b > 0 ? "+ " : "- ") << getCoefficient(b) << "x " << (c > 0 ? "+ " : "- ") << (c != 1 ? getCoefficient(c) : "1") << " = " << result1 << " or " << result2 << std::endl;
  return result1;
}

score_t testAction2(double a, double b, double c) {
  return std::sqrt(a) + ((0-(std::pow(b,2))) + (4*b) + 1);
}

int main(int argc, char **argv) {
  std::cout << "You are running AVCB version " << getVersion() << std::endl;

  action a = &testAction2;
  ActionList aList;
  aList.push_back(a);
  InputList iList;
  iList.push_back(Input(5,5,0));
  UnexecutedSequence sequence (aList,iList);
  UnexecutedSequence *sequencePointer = &sequence;

  score_t score = sequence.execute().score;
  Variable v(true,0,1);
  VariableManager vm(sequencePointer,v,score);
 
  std::cout << "Initial Score: " << score << std::endl;
  std::cout << "How many cycles: ";
  int cycles;
  std::cin >> cycles;
  for (int i = 0; i < cycles; i++) {
    vm.step();
  }
  std::cout << "After " << cycles << " turns: " << vm.lastScore << std::endl;

  return 0;
}
