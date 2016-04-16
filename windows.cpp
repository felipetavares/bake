/*
        Copyright © 2015 Felipe Tavares <felipe.oltavares@gmail.com>

        This work is free. You can redistribute it and/or modify it under the
        terms of the Do What The Fuck You Want To Public License, Version 2,
        as published by Sam Hocevar. See the LICENSE file for more details.
 */

#include "windows.hpp"
#include <algorithm>
#include <iostream>
#include <sys/stat.h>
using namespace std;

#ifdef _WIN32
int bake::scandir(char *directory, struct dirent ***dir_list, char *blank,
                  int (*sort)(const struct dirent **,
                              const struct dirent **)) {
  vector<struct dirent> info;
  int dir_num = 0;

  WIN32_FIND_DATA file;
  HANDLE found_file;

  if ((found_file = FindFirstFile(".\\*", &file)) != INVALID_HANDLE_VALUE) {
    do {
      struct dirent d;
      strcpy(d.d_name, file.cFileName);
      info.push_back(d);
    }
    while (FindNextFile(found_file, &file) != 0);
  }

  std::sort(info.begin(), info.end(), [] (struct dirent d1, struct dirent d2) {
              struct stat info1, info2;
              stat(d1.d_name, &info1);
              stat(d2.d_name, &info2);

              return info1.st_mtime < info2.st_mtime;
            }
            );

  *dir_list = (struct dirent **)malloc(sizeof(struct dirent *) * info.size());

  for (int i = 0; i < int(info.size()); i++) {
    (*dir_list)[i] = (struct dirent *)malloc(sizeof(struct dirent));
    strcpy((*dir_list)[i]->d_name, info[i].d_name);
    dir_num++;
  }

  return dir_num;
}

#endif // ifdef _WIN32
