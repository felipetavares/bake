/*
        Copyright © 2015 Felipe Tavares <felipe.oltavares@gmail.com>

        This work is free. You can redistribute it and/or modify it under the
        terms of the Do What The Fuck You Want To Public License, Version 2,
        as published by Sam Hocevar. See the LICENSE file for more details.
*/

#ifndef BAKE_POST_H
#define BAKE_POST_H

#include <string>
#include <sys/time.h>
#include "bakefile.hpp"
using namespace std;

namespace bake {

class Post {
  time_t edit_time;
public:
  string title;
  string content;
  string time;
  string id;
  string link;
  string author;

  Post(time_t, string, string, string, Configuration&);
private:
  void set_time(time_t&, string);
  void set_title(string&);
  void set_link(string, string);
  void set_id(string&);

  string get_file (string);
};

}

#endif /* BAKE_POST_H */
