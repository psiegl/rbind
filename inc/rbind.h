// Copyright 2019 - , Dr.-Ing. Patrick Siegl
// SPDX-License-Identifier: Apache-2.0

#ifndef _RBIND_H_
#define _RBIND_H_

#include <tuple>
#include <iostream>
#include <type_traits> // std::enable_if

// in case of clang++
#ifndef typeof
# define typeof __typeof__
#endif /* #ifndef typeof */

namespace rbind {
//////////////// INTERFACE
  class rbind {
  public:
    rbind( void ) {}
    virtual ~rbind( void ) = 0;
    virtual void setparms( char* c_parms ) = 0;
    virtual void exec( void ) = 0;
    virtual void operator() ( void ) = 0;
    virtual unsigned int sizeparms( void ) = 0;
  };
  rbind::~rbind() {} // LCOV_EXCL_LINE

//////////////// HELPER
  namespace helper {
    // create sequence
    template<int ...>
    struct __seq { };

    template<int N, int ...S>
    struct __genseq : __genseq<N-1, N-1, S...> { };

    template<int ...S>
    struct __genseq<0, S...>
    {
       typedef __seq<S...> type;
    };

    // sets arguments being later used for the function into tuple
    template<std::size_t I = 0, typename... Tp>
    inline typename std::enable_if< I == sizeof...(Tp), void>::type
      __setparms( std::tuple<Tp...>&, char *parms ) // Unused arguments are given no names.
      {
      }

    template<std::size_t I = 0, typename... Tp>
    inline typename std::enable_if< 0 <= I && I < sizeof...(Tp), void>::type
      __setparms(std::tuple<Tp...>& t, char *parms )
      {
        std::get<I>(t) = *( typeof(std::get<I>(t))* )parms;
        __setparms<I + 1, Tp...>( t, parms + sizeof(std::get<I>(t)) );
      }

    // calculates the amount of nbytes required to store all function arguments
    // based on the tuple definition!
    template<std::size_t I = 0, typename... Tp>
    inline typename std::enable_if< I == sizeof...(Tp), void>::type
      __sizeparms( std::tuple<Tp...>&, unsigned int& )
      {
      }

    template<std::size_t I = 0, typename... Tp>
    inline typename std::enable_if< I < sizeof...(Tp), void>::type
      __sizeparms(std::tuple<Tp...>& t, unsigned int& len )
      {
        len += sizeof(std::get<I>(t));
        __sizeparms<I + 1, Tp...>( t, len );
      }
  };

//////////////// HANDLER TO RETRIEVE PARMS FROM CALLED VARIADIC FUNC.
  // calculates the amount of nbytes required to store all function arguments
  // based on the variadic arguments supplied to function!
  template<typename T>
  unsigned int __sizeparms(unsigned int nbytes, T arg)
  {
    return nbytes + sizeof(arg);
  }

  template<typename T, typename T2, typename... Ts>
  unsigned int __sizeparms(unsigned int nbytes, T arg, T2 arg2, Ts... parms)
  {
    return __sizeparms(nbytes + sizeof(arg), arg2, parms...);
  }

  template<typename... Ts>
  unsigned int sizeparms(Ts... parms)
  {
    return __sizeparms(0, parms ...);
  }

  // extracts the arguments and puts them onto memory allocation
  template<typename T>
  unsigned int __extractparms(char* cnt, unsigned int nbytes, T arg)
  {
    (*(typeof(arg)*)cnt) = arg;
    return nbytes + sizeof(arg);
  }

  template<typename T, typename T2, typename... Ts>
  unsigned int __extractparms(char* cnt, unsigned int nbytes, T arg, T2 arg2, Ts... parms)
  {
    (*(typeof(arg)*)cnt) = arg;
    return __extractparms(cnt + sizeof(arg), nbytes + sizeof(arg), arg2, parms...);
  }

  template<typename... Ts>
  unsigned int extractparms(char* cnt, Ts... parms)
  {
    return __extractparms(cnt, 0, parms ...);
  }

//////////////// HANDLER FOR REGULAR FNC PTR
  template <typename> class _Bind;
  template <typename Result, typename... Parms>
  class _Bind<Result (*)(Parms...)> : public rbind {
  private:
    Result              (*fnc)(Parms...);
    std::tuple<Parms...> parms;

    template<int ...S>
    Result execfnc(helper::__seq<S...>)
    {
      return this->fnc(std::get<S>(this->parms) ...);
    }

  public:
    _Bind(Result (*fnc)(Parms...)) :
      rbind(),
      fnc(fnc)
    {
    }

    ~_Bind(void)
    {
    }

    void setparms(char* c_parms)
    {
      helper::__setparms(this->parms, c_parms);
    }

    void exec( void )
    {
      (*this)();
    }

    void operator()(void)
    {
      this->execfnc(typename helper::__genseq<sizeof...(Parms)>::type());
    }

    unsigned int sizeparms(void)
    {
      unsigned int size = 0;
      helper::__sizeparms(this->parms, size);
      return size;
    }
  };

  template <typename Result, typename... Parms>
  rbind* bind(Result (*fnc)(Parms...)) {
    return new _Bind<Result(*)(Parms...)>(fnc);
  }


//////////////// HANDLER FOR OBJECT FNC PTR
  template <typename> class _Bind;
  template <typename Result, typename Object, typename... Parms>
  class _Bind<Result (Object::*)(Parms...)> : public rbind {
  private:
    Object              *obj;
    Result              (Object::*fnc)(Parms...);
    std::tuple<Parms...> parms;

    template<int ...S>
    Result execfnc(helper::__seq<S...>)
    {
      return (this->obj->*this->fnc)(std::get<S>(this->parms) ...);
    }

  public:
    _Bind(Result (Object::*fnc)(Parms...), Object *obj) :
      rbind(),
      fnc(fnc),
      obj(obj)
    {
    }

    ~_Bind(void)
    {
    }

    void setparms(char* c_parms)
    {
      helper::__setparms(this->parms, c_parms);
    }

    void exec(void)
    {
      (*this)();
    }

    void operator()(void)
    {
      this->execfnc(typename helper::__genseq<sizeof...(Parms)>::type());
    }

    unsigned int sizeparms(void)
    {
      unsigned int size = 0;
      helper::__sizeparms(this->parms, size);
      return size;
    }
  };

  template <typename Result, typename Object, typename... Parms>
  rbind* bind(Result (Object::*fnc)(Parms...), Object* obj) {
    return new _Bind<Result(Object::*)(Parms...)>(fnc, obj);
  }
};

#endif /* #ifndef _RBIND_H_ */
