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

#include <map>
#include <algorithm>

#include "bot.h"
#include "AVCB_config.h"

AVCB::AVCB(UnexecutedSequence *us,std::vector<bool> relevancy,std::vector<double> baseline) : sequenceTemplate(us), stepIndex(0), stage(0) {
  int actionIndex = 0;
  int inputIndex = 0;
  for (int i = 0; i < relevancy.size(); i++) {
    bool b = relevancy[i];
    double d = baseline[i];
    Variable v (b,actionIndex,inputIndex);
    if (v.isRelevant) {
      varmanagers.push_back(VariableManager(us,v,d));
    }
    switch (inputIndex) {
      case 2: inputIndex = 0; actionIndex++; break;
      default: inputIndex++; break;
    }
  }
}

double average(std::vector<double> terms) {
  double sum = 0.0;
  for (double d : terms) sum += d;
  return sum / terms.size();
}

typedef std::map<int,double> vmdmap;

vmdmap createMap(std::vector<int> v1, std::vector<double> v2) {
  vmdmap val;
  for (int i = 0; i < v1.size(); i++) {
    val[v1[i]] = v2[i];
  }
  return val;
}

class intDoublePair {
  public:
    int i;
    double d;
    intDoublePair(int n, double o) : i(n), d(o) { }
};

bool operator<(intDoublePair const & a, intDoublePair const & b)
{
    return a.d < b.d;
}

std::vector<int> getRange(int bottom, int top) {
  std::vector<int> vec;
  for (int i = bottom; i < top; i++) vec.push_back(i);
  return vec;
}

vmdmap sortMapByValue(vmdmap map) {
  std::vector<intDoublePair> idps;   
  for (std::pair<int,double> pair : map) {
    idps.push_back(intDoublePair(pair.first, pair.second));
  }
  std::sort(idps.begin(),idps.end()); 
  map.clear();
  for (intDoublePair idp : idps) {
    map[idp.i] = idp.d;
  }
  return map;
}

std::vector<int> getVMIndexes(vmdmap map) {
  std::vector<int> indexes;
  for (std::pair<int,double> pair : map) indexes.push_back(pair.first);
  return indexes;
}

void AVCB::step() {
  if (stage == 0) {
    if (runIndex < varmanagers.size()) {
      if (stepIndex < RUN_CYCLE_COUNT) {
        lastScore = curScore;
        curScore = varmanagers[runIndex].step(); 
        diffs.push_back(curScore - lastScore);
        stepIndex++; 
      }
      else {
        curScore = 0.0;
        lastScore = 0.0;
        varAdjustmentScores[runIndex] = average(diffs);
        diffs.clear(); 
        stepIndex = 0;
        runIndex++;
      } 
    }
    else {
      vmdmap map = createMap(getRange(0,varmanagers.size() - 1),varAdjustmentScores); 
      map = sortMapByValue(map);   
      orderOfActions = getVMIndexes(map);
      stage = 1;
      runIndex = 0;
    }
  } 
  else {
    if (runIndex < varmanagers.size()) {
      runIndex++;
      varmanagers[orderOfActions[runIndex]].step();
    }
    else {
      runIndex = 1;
      varmanagers[orderOfActions[0]].step();
    }
  }
}
