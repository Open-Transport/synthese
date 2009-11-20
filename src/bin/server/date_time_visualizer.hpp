// Copyright Filip Konvièka 2007 - 2009. Use, modification and distribution are subject
// to the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef DATE_TIME_VISUALIZER_HPP
#define DATE_TIME_VISUALIZER_HPP

#if defined(_DEBUG) && defined(_MSC_VER) && (_MSC_VER<1400)

inline char* _print_ptime(boost::posix_time::ptime& pt) {
  static char str[200];
  std::string s=boost::posix_time::to_simple_string(pt);
  strcpy(str+0, s.c_str());
  return str+0;
}

inline char* _print_time_duration(boost::posix_time::time_duration& td) {
  static char str[200];
  std::string s=boost::posix_time::to_simple_string(td);
  strcpy(str+0, s.c_str());
  return str+0;
}

// ensure that this is called in each translation unit...
namespace {
  template<typename T> struct _print_ptime_helper {
    _print_ptime_helper() {
      boost::posix_time::ptime pt;
      _print_ptime(pt);
      boost::posix_time::time_duration td;
      _print_time_duration(td);
    }
  };
  typedef _print_ptime_helper<void> _print_ptime_helper_2;
  _print_ptime_helper_2 _print_ptime_helper_2i;
}

#endif

#endif
