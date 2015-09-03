/*
        Copyright © 2015 Felipe Tavares <felipe.oltavares@gmail.com>

        This work is free. You can redistribute it and/or modify it under the
        terms of the Do What The Fuck You Want To Public License, Version 2,
        as published by Sam Hocevar. See the LICENSE file for more details.
*/

#ifndef BAKE_POST_H
#define BAKE_POST_H

#include <string>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include "bakefile.hpp"
using namespace std;

namespace bake {

class Post {
  time_t edit_time;
  bool use_hashed_ids;
  bool multiple_files;
public:
  string title;
  string content;
  string econtent;
  string time;
  string id;
  string link;
  string author;
  string scontent;

  Post(time_t, string, string, string, string, bool, Configuration&);
private:
  void set_time(time_t&, string);
  void set_title(string&);
  void set_link(string, string);
  void set_id(string&);

  string get_file (string);
  /* For facebook descriptions */
  string simplify(string);
};

}

#endif /* BAKE_POST_H */
