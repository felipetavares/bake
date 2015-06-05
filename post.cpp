/*
        Copyright © 2015 Felipe Tavares <felipe.oltavares@gmail.com>

        This work is free. You can redistribute it and/or modify it under the
        terms of the Do What The Fuck You Want To Public License, Version 2,
        as published by Sam Hocevar. See the LICENSE file for more details.
*/

#include "post.hpp"
#include <ctime>
#include <sstream>
using namespace bake;

Post::Post (time_t edit_time, string title, string content, string author, Configuration &conf) {
  this->edit_time = edit_time;
  this->content = content;
  this->author = author;

  set_time(edit_time, conf.get("date"));
  set_title(title);
  set_id(title);
  set_link(conf.get("host"), conf.get("output"));
}

void Post::set_time (time_t &t, string format) {
  char buffer[255];
  strftime(buffer, 255, format.c_str(), localtime(&t));
  time = string(buffer);
}

void Post::set_id (string &title) {
  /*
  unsigned long hash = 5381;
  int c;

  for (unsigned int i=0;i<title.size();i++) {
      c = title[i];
      hash = ((hash << 5) + hash) + c;
  }

  stringstream ss;
  ss << hash;

  id = ss.str();
  */

  id = "";
  for (char c :title) {
    if (c == '.')
      break;

    if (c != ' ')
      id += c;
  }
}

void Post::set_link (string host, string output) {
  if (host.size() > 0)
    if (host[host.size()-1] != '/')
      host += '/';

  link = host + get_file (output) + "#" + id;
}

void Post::set_title (string &t) {
  title = "";

  for (auto c :t) {
    if (c == '-' || c == '_' || c == ' ') {
      title += ' ';
    } else
    if (c == '.') {
      break;
    } else {
      title += c;
    }
  }
}

string Post::get_file (string path) {
  if (path.size() > 0) {
    for (int i=path.size()-1;i>=0;i--) {
      if (path[i] == '/') {
        return path.substr(i, path.size()-i);
      }
    }
    return path;
  } else {
    return "";
  }
}
