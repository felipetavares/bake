#include "printer.hpp"
#include "template.hpp"
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

using namespace bake;

Printer::Printer (string filename) {
	this->filename = filename;
}

void Printer::print (Tree* temp, vector<Post*> posts) {
	fstream file(filename, ios::out);
	if (!file.is_open())
		return;

	file << temp->get_begin();
	auto post_temp = temp->get_post_template();

	for (auto post :posts) {
		string post_str = post_temp->get_post(post);
		file << post_str;
	}

	file << temp->get_end();

	file.close();
}

void Printer::print (Tree* temp, Post* post) {
	fstream file(filename, ios::out);
	if (!file.is_open())
		return;

	file << temp->get_begin();
	auto post_temp = temp->get_post_template();

	string post_str = post_temp->get_post(post);
	file << post_str;

	file << temp->get_end();

	file.close();
}
