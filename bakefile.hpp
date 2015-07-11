/*
        Copyright © 2015 Felipe Tavares <felipe.oltavares@gmail.com>

        This work is free. You can redistribute it and/or modify it under the
        terms of the Do What The Fuck You Want To Public License, Version 2,
        as published by Sam Hocevar. See the LICENSE file for more details.
*/

#ifndef BAKEFILE_H
#define BAKEFILE_H

#include <string>
#include <map>
using namespace std;

namespace bake {

class Configuration {
	map <string, string> mapper;
public:
	string get(string);
	void set(string, string);

	void print_all();
};

class Bakefile {
	static unsigned int state;
public:
	static Configuration read(string);
	static void process(char, Configuration&);
};

}

#endif /* BAKEFILE_H */
