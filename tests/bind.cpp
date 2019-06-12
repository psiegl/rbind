// Copyright 2019 - , Dr.-Ing. Patrick Siegl
// SPDX-License-Identifier: Apache-2.0

#include <cassert>
#include <cstring>
#include <vector>
#include "rbind.h"

void fnc( char c, short s, int i, void* v )
{
  std::cout << "global " << __FUNCTION__ << " called with parms '"
            << c << "', '"
            << s << "', '"
            << i << "', '"
            << v << "'" << std::endl;
}

class obj
{
public:
  void fnc( char c, short s, int i, void* v )
  {
    std::cout << "object " << __FUNCTION__ << " called with parms '"
              << c << "', '"
              << s << "', '"
              << i << "', '"
              << v << "'" << std::endl;
  }
};

int main(void)
{
  struct __attribute__((__packed__)) {
    char c;
    short s;
    int i;
    void* v;
  } parms;


  std::memset(&parms, 0, sizeof(parms));
  rbind::rbind* pfnc = rbind::bind( fnc );
  auto pfnc_sizeparms = pfnc->sizeparms();
  std::cout << "func 'fnc' requires " << pfnc_sizeparms << " bytes of parameter space." << std::endl;
  assert(sizeof(parms) == pfnc_sizeparms);

  parms.c = 'c';
  parms.s = 66;
  parms.i = 100;
  parms.v = (void*)0xDEADBEEF;
  pfnc->setparms((char*)&parms);
  pfnc->exec();

  parms.i = 513;
  pfnc->setparms((char*)&parms);
  (*pfnc)();

  delete pfnc;


  std::memset(&parms, 0, sizeof(parms));
  obj o;
  rbind::rbind* opfnc = rbind::bind( &obj::fnc, &o );
  auto opfnc_sizeparms = opfnc->sizeparms();
  std::cout << "func 'obj::fnc' requires " << opfnc_sizeparms << " bytes of parameter space." << std::endl;
  assert(sizeof(parms) == opfnc_sizeparms);

  parms.c = 'c';
  parms.s = 66;
  parms.i = 100;
  parms.v = (void*)0xDEADBEEF;
  opfnc->setparms((char*)&parms);
  opfnc->exec();

  parms.i = 513;
  opfnc->setparms((char*)&parms);
  (*opfnc)();

  delete opfnc;

  return 0;
}
