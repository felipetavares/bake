/*
        Copyright © 2015 Felipe Tavares <felipe.oltavares@gmail.com>

        This work is free. You can redistribute it and/or modify it under the
        terms of the Do What The Fuck You Want To Public License, Version 2,
        as published by Sam Hocevar. See the LICENSE file for more details.
*/

#include "bake.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctype.h>
#include <ctime>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <clocale>
extern "C" {
#include "sundown/markdown.h"
#include "sundown/buffer.h"
#include "sundown/html.h"
}
using namespace std;
using namespace bake;

namespace bake {
	/*
		Sort files descending by time
	*/
	int timesort(const struct dirent **d1, const struct dirent **d2) {
		struct stat info1, info2;
		stat((*d1)->d_name, &info1);
		stat((*d2)->d_name, &info2);

		return info1.st_ctime > info2.st_ctime;
	}

	/*
		Verify if the file described by 'name'
		has extension 'g_ext'
	*/
	bool is_extension (string& name, string g_ext) {
		string ext;

		for (int i=name.size()-1;name[i] != '.' && i >= 0; i--) {
			ext += name[i];
		}

		reverse(ext.begin(), ext.end());

		return ext == g_ext;
	}

	/*
		Convert the file in 'filename' to an html string and return it.
		'author' and 'date' are filled if present in the file.
	*/
	string to_html (string& filename, string& author, string& date) {
		struct buf *ib, *ob;
		int ret;
		FILE *in;
		int read_size = 1024;
		int output_size = 64;

		struct sd_callbacks callbacks;
		struct html_renderopt options;
		struct sd_markdown *markdown;

		in = fopen(filename.c_str(), "r");
		if (!in) {
			cout << "bake: could not open \"" << filename << "\"" << endl;
			return string("");
		}

		int c = getc(in);
		if (c == '~') {
			while ((c = getc(in)) != EOF && c != '\n') {
				author += c;
			}
		} else {
			fseek(in, 0, SEEK_SET);
		}
		c = getc(in);
		if (c == '@') {
			while ((c = getc(in)) != EOF && c != '\n') {
				date += c;
			}
		} else {
			fseek(in, -1, SEEK_CUR);
		}


		ib = bufnew(read_size);
		bufgrow(ib, read_size);
		while ((ret = fread(ib->data + ib->size, 1, ib->asize - ib->size, in)) > 0) {
			ib->size += ret;
			bufgrow(ib, ib->size + read_size);
		}

		ob = bufnew(output_size);

		sdhtml_renderer(&callbacks, &options, 0);
		markdown = sd_markdown_new(0, 16, &callbacks, &options);

		sd_markdown_render(ob, ib->data, ib->size, markdown);
		sd_markdown_free(markdown);

		bufrelease(ib);

		string result = string((char*)ob->data, ob->size);

		bufrelease(ob);

		return result;
	}

	bool is_directory (string filename) {
		struct stat info;
		if (stat(filename.c_str(), &info)) {
			return false;
		}
		return S_ISDIR(info.st_mode);
	}

	/*
		Return all posts in the current directory, time-sorted
	*/
	vector <Post*> get_posts(Configuration &conf, bool multiple_files) {
		char dir[] = "./";
		struct dirent **dir_list;
		int dir_num;
		vector <Post*> posts;

		dir_num = scandir(dir, &dir_list, NULL, timesort);

		if (dir_num < 0) {
			cout << "bake: cannot open " << dir << endl;
			return posts;
		}

		while (dir_num--) {
			struct dirent *ep = dir_list[dir_num];
			struct stat info;

			string filename = string(dir)+string(ep->d_name);

			if (stat(filename.c_str(), &info)) {
				break;
			}

			if (!S_ISDIR(info.st_mode)) {
				if (is_extension(filename, "markdown")) {
					cout << "bake: processing \"" << ep->d_name << "\"" << endl;
					string author, date;
					string html = to_html(filename, author, date);
					posts.push_back(new Post(info.st_mtime, string(ep->d_name), html, author, date, multiple_files, conf));
				} else
				if (is_extension(filename, "netrec")) {

				}
			}

			free(dir_list[dir_num]);
		}

		free(dir_list);

		return posts;
	}
}

int main (int argc, char **argv) {
	auto conf = Bakefile::read();

	if (conf.get("lang") != "") {
		setlocale(LC_ALL, conf.get("lang").c_str());
		cout << "bake: locale: " << setlocale(LC_ALL, NULL) << endl;
	}

	auto reader = Template(conf.get("template"));
	auto temp = reader.read();

	if (temp) {
		vector <Post*> posts;

		if (is_directory(conf.get("output"))) {
			posts = get_posts(conf, true);
			auto printer = Printer(conf.get("output")+"index.html");

			// Print the main index.html page
			printer.print(temp, posts);

			for (auto post :posts) {
				auto post_printer = Printer(conf.get("output")+post->id);
				post_printer.print(temp, post);
			}
		} else {
			posts = get_posts(conf, false);
			auto printer = Printer(conf.get("output"));

			// Print the main index.html page
			printer.print(temp, posts);
		}

		for (auto post :posts) {
			delete post;
		}
	}
}
