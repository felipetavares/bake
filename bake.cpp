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
#include <pwd.h>
#include <clocale>
#include <sstream>
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

		return info1.st_mtime > info2.st_mtime;
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

	bool is_directory (string filename) {
		struct stat info;
		if (stat(filename.c_str(), &info)) {
			return false;
		}
		return S_ISDIR(info.st_mode);
	}

	string get_directory (string original) {
		for (int i=original.size()-1;i>=0;i--) {
			if (original[i]=='/')
				return original.substr(0, i+1);
		}

		if (is_directory(original))
			return original+"/";
		else
			return "./";
	}

	string generate_netrec_stream (string id, Configuration &conf, uint32_t &tw, uint32_t &th) {
		auto oname = get_directory(conf.get("output"));

		stringstream hashs;

		unsigned long hash = 5381;
		int c;

		for (unsigned int i=0;i<id.size();i++) {
				c = id[i];
				hash = ((hash << 5) + hash) + c;
		}

		hashs << hash;

		// Files
		fstream *file = new fstream (id, ios::in | ios::binary);
		fstream *output = new fstream (oname+hashs.str()+".json", ios::out | ios::binary);

		// Data length
		uint64_t dlen;
		// Read data
		uint64_t rdata = 0;

		if (file->is_open() && output->is_open()) {
			file->read((char*)&th, sizeof(th));
			file->read((char*)&tw, sizeof(tw));
			file->read((char*)&dlen, sizeof(dlen));

			*output << "{" << endl;
			*output << "\"stream\": [";

			while (rdata < dlen) {
				char c;
				file->read (&c, 1);

				if (*file)
					rdata++;
				else
					break;

				if (rdata > 1)
					*output << ",";
				*output << (int)c;
			}

			*output << "]," << endl;

			uint64_t timestamp;
			uint16_t chars;

			*output << "\"timing\": [";

			bool first = true;
			while (*file) {
				file->read ((char*)&timestamp, sizeof(timestamp));
				file->read ((char*)&chars, sizeof(chars));

				if (*file) {
					if (!first)
						*output << ",";
					first = false;
					float ftm = ((float)timestamp/1000.0f);
					// "Fast forward" if it takes more than 15 seconds
					if (ftm > 15)
						ftm = 1;
					*output << "[" << ftm << "," << (int)chars << "]";
				}
			}

			*output << "]" << endl;
			*output << "}" << endl;

			file->close();
			output->close();
		}

		cout << "bake: netrec: terminal (" << tw << ", " << th << ")" << endl;

		delete file;
		delete output;

		return hashs.str();
	}

	string escape_html (string html) {
		string escaped = "";

		for (auto c :html) {
			if (c == '>') {
				escaped += "&gt;";
			} else
			if (c == '<') {
				escaped += "&lt;";
			} else
			if (c == '&') {
				escaped += "&amp;";
			} else {
				escaped += c;
			}
		}

		return escaped;
	}

	/*
		Convert the file in 'filename' to an html string and return it.
		'author' and 'date' are filled if present in the file.
	*/
	string to_html (string& filename, string& author, string& date, bool escape) {
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
			struct stat info;
			struct passwd *pw;
			stat(filename.c_str(), &info);
			pw = getpwuid(info.st_uid);
			// Set author for system's user name
			author = string(pw->pw_name);
			fseek(in, 0, SEEK_SET);
		}
		c = getc(in);
		if (c == '@') {
			while ((c = getc(in)) != EOF && c != '\n') {
				if (!escape)
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

		if (escape) {
			return escape_html (result);
		}
		return result;
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

			bool escape = conf.get("feed") == "true";

			if (!S_ISDIR(info.st_mode)) {
				if (is_extension(filename, "markdown")) {
					cout << "bake: processing \"" << ep->d_name << "\"" << endl;
					string author, date;
					string html = to_html(filename, author, date, escape);
					string p_name = string(ep->d_name);
					if (escape)
						p_name = escape_html(p_name);
					posts.push_back(new Post(info.st_mtime, p_name, html, author, date, multiple_files, conf));
				} else
				if (is_extension(filename, "netrec")) {
					cout << "bake: processing \"" << ep->d_name << "\"" << endl;
					string author, date;
					string id = string(ep->d_name);
					string html, host=conf.get("host");
					uint32_t size_w;
					uint32_t size_h;

					string nid = generate_netrec_stream(id, conf, size_w, size_h);
					stringstream ssw, ssh;
					ssw << size_w;
					ssh << size_h;

					string url = host+((host[host.size()-1]=='/')?"":"/")+nid+".json";

					html += "<div class=\"terminal-container\"><pre class=\"terminal\" id=\""+nid+"\"></pre></div>\n";
					html += "<script type=\"text/javascript\">\n";
					html += "new term.Terminal("+ssw.str()+","+ssh.str()+",\""+nid+"\", \""+url+"\");\n";
					html += "</script>\n";

					string p_name = string(ep->d_name);

					if (escape)
						p_name = escape_html(p_name);

					posts.push_back(new Post(info.st_mtime, p_name, html, author, date, multiple_files, conf));
				}
			}

			free(dir_list[dir_num]);
		}

		free(dir_list);

		return posts;
	}
}

int main (int argc, char **argv) {
	cout << "bake: Copyright © 2015 Felipe Tavares" << endl;
	cout << "bake: See the LICENSE file distributed with the source for licensing." << endl;

	string configfile = "bakefile";

	if (argc > 1)
		configfile = string(argv[1]);

	auto conf = Bakefile::read(configfile);

	if (conf.get("lang") != "") {
		setlocale(LC_ALL, conf.get("lang").c_str());
		cout << "bake: locale: " << setlocale(LC_ALL, NULL) << endl;
	}

	auto reader = Template(conf.get("template"));
	auto temp = reader.read();

	Template *post_template_reader;
	Tree* post_temp = NULL;

	if (conf.get("post_template") != "") {
		post_template_reader = new Template(conf.get("post_template"));
		post_temp = post_template_reader->read();
	}

	if (temp) {
		vector <Post*> posts;

		if (is_directory(conf.get("output"))) {
			posts = get_posts(conf, true);
			auto printer = Printer(conf.get("output")+"index.html");

			// Print the main index.html page
			printer.print(temp, posts);

			for (auto post :posts) {
				auto post_printer = Printer(conf.get("output")+post->id);
				if (post_temp)
					post_printer.print(post_temp, post);
				else
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

	if (conf.get("post_template") != "")
		delete post_template_reader;
}
