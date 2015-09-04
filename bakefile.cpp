/*
        Copyright © 2015 Felipe Tavares <felipe.oltavares@gmail.com>

        This work is free. You can redistribute it and/or modify it under the
        terms of the Do What The Fuck You Want To Public License, Version 2,
        as published by Sam Hocevar. See the LICENSE file for more details.
*/

#include <fstream>
#include <iostream>
#include <cstdlib>

#include "bakefile.hpp"
using namespace bake;

void Configuration::print_all () {
	for (auto i :mapper) {
		cout << i.first << "=\"" << i.second <<"\"" << endl;
	}
}

void Configuration::set (string k, string v) {
	mapper[k] = v;
}

string Configuration::get (string k) {
	return mapper[k];
}

unsigned int Bakefile::state = 0;

Configuration Bakefile::read (string path) {
	Configuration conf;
	char c;

	fstream file(path, ios::in);

	if (file.is_open()) {
			Bakefile::state = 0;

			while (file.good()) {
				file.read(&c, 1);

				Bakefile::process(c, conf);
			}

			Bakefile::process('\n', conf);
	} else {
		cout << "bake: no bakefile found." << endl;
		exit(1);
	}

	return conf;
}

void Bakefile::process(char c, Configuration& conf) {
	static string key = string();
	static string value = string();

	switch (state) {
			case 0:
					switch (c) {
						case '=':
							state = 1;
						break;
						case ' ': case '\t':
						break;
						default:
							key += c;
					}
			break;
			case 1:
				switch (c) {
					case ' ': case '\t': break;
					case '"':
						state = 2;
						value = string();
					break;
					case '\n':
						state = 0;
						if (key != "")
								conf.set(key, value);
						key = string();
						value = string();
					break;
					default:
						value += c;
				}
			break;
			case 2:
				switch (c) {
					case '"':
						state = 1;
					break;
					case '\\':
						state = 3;
					break;
					default:
						value += c;
				}
			break;
			case 3:
				value += c;
				state = 2;
			break;
			default: break;
	}
}
