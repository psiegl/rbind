// Copyright 2019 - , Dr.-Ing. Patrick Siegl
// SPDX-License-Identifier: Apache-2.0

#include <cassert>
#include <cstring>
#include <vector>
#include "rbind.h"


void callee(int i, char c, void* v)
{
    std::cout << __FUNCTION__ << " called with parms '"
              << c << "', '"
              << i << "', '"
              << v << "'" << std::endl;
}

template<typename ...parm>
void caller(parm... parms)
{
  std::vector<char> cparms(rbind::sizeparms(parms...));

  // serialize all parms into vector
  rbind::extractparms(cparms.data(), parms...);

  // bind new functions
  rbind::rbind* fnc = rbind::bind( callee );

  // fill in parms kept in vector
  fnc->setparms(cparms.data());

  // execute
  (*fnc)();

  delete fnc;
}

int main(void)
{
  caller((int)10, (char)'d', (void*)0xDEADBEEF);

  return 0;
}
