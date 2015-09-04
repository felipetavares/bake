/*
        Copyright © 2015 Felipe Tavares <felipe.oltavares@gmail.com>

        This work is free. You can redistribute it and/or modify it under the
        terms of the Do What The Fuck You Want To Public License, Version 2,
        as published by Sam Hocevar. See the LICENSE file for more details.
*/

#include "template.hpp"
#include <iostream>
#include <fstream>
#include <cstdio>
using namespace bake;

Tree::Tree () {
  ptr = root = new Node(NULL);
}

Tree::Tree (Node *root) {
  this->root = root;
  ptr = root;
}

Tree::~Tree () {
  delete root;
}

void Tree::push_name(char c) {
  if (ptr) {
    ptr->name += c;
  }
}

void Tree::push_text(char c) {
  if (ptr) {
    ptr->text += c;
  }
}

void Tree::up() {
  if (ptr) {
    if (ptr->parent)
      ptr = ptr->parent;
  }
}

void Tree::down() {
  if (ptr) {
    ptr = ptr->child();
  }
}

void Tree::create() {
  if (ptr) {
    ptr->add(new Node(ptr));
  }
}

string Tree::get_begin () {
	return root->get_begin();
}

string Tree::get_end () {
	return root->get_end();
}

Tree* Tree::get_post_template () {
  return new Tree(root->get_post_template());
}

Tree::Node::Node (Node* parent) {
  this->parent = parent;
}

Tree::Node::~Node () {
  for (auto n :children) {
    delete n;
  }
}

unsigned int Tree::Node::count () {
  unsigned int sum = 1;

  for (auto child :children) {
    sum += child->count();
  }

  return sum;
}

unsigned int Tree::count () {
  return root->count();
}

string Tree::get_post (Post *p) {
  return root->get_post(p);
}

string Tree::Node::get_begin () {
	string begin = text;

	for (auto child :children) {
		if (child->text != "") {
			begin += child->text;
		} else {
			break;
		}
	}

	return begin;
}

string Tree::Node::get_end () {
  if (children.size()) {
    return children[children.size()-1]->text;
  } else {
    return "";
  }
}

Tree::Node* Tree::Node::get_post_template () {
	for (auto child :children) {
		if (child->text == "") {
      return child;
		}
	}

	return NULL;
}

string Tree::Node::get_post (Post *p) {
  if (this->name == "content") {
    return p->content;
  } else
  if (this->name == ":content") {
    return p->econtent;
  } else
  if (this->name == "@content") {
  	return p->scontent;
  }
  if (this->name == "title") {
    return p->title;
  } else
  if (this->name == "date") {
    return p->time;
  } else
  if (this->name == "id") {
    return p->id;
  } else
  if (this->name == "link") {
    return p->link;
  } else
  if (this->name == "author") {
    return p->author;
  }

  string post = this->text;

  for (auto child :children) {
    post += child->get_post(p);
  }

  return post;
}

void Tree::Node::add(Node *n) {
  children.push_back(n);
}

Tree::Node* Tree::Node::child() {
  if (children.size() > 0) {
    return children.back();
  } else {
    return this;
  }
}

Template::Template (string path) {
  this->path = path;
  this->state = 0;
  this->zero_cost_state_change = false;
  this->comment = NONE;
  this->tree = new Tree();
}

Template::~Template () {
  delete this->tree;
}

Tree* Template::read () {
  auto success = tree;
  cout << "bake: template from: \"" << path << "\"" << endl;

  fstream file(path.c_str(), ios::in);

  if (file.is_open()) {
    char c;

    while (file.good()) {
      file.read(&c, 1);

      this->process(c);
    }

    cout << "bake: template used " << tree->count() << " nodes"<< endl;
  } else {
    cout << "bake: could not open template" << endl;
    success = NULL;
  }

  return success;
}

void Template::process (char c) {
  static const string style_begin = "<style";
  static const string script_begin = "<script";
  static const string script_end = "</script>";
  static const string style_end = "</style>";

  prev += c;

  if (comment == SCRIPT) {
    tree->push_text(c);

    if (prev.size() >= script_end.size() &&
        (prev.substr(prev.size()-script_end.size(), script_end.size())
        == script_end)) {
      comment = NONE;
    }
    return;
  }
  if (comment == STYLE) {
    tree->push_text(c);

    if (prev.size() >= style_end.size() &&
        (prev.substr(prev.size()-style_end.size(), style_end.size())
        == style_end)) {
      comment = NONE;
    }
    return;
  }

  if (prev.size() >= style_begin.size() &&
      (prev.substr(prev.size()-style_begin.size(), style_begin.size())
      == style_begin)) {
    comment = STYLE;
    tree->push_text(c);
    return;
  }
  if (prev.size() >= script_begin.size() &&
      (prev.substr(prev.size()-script_begin.size(), script_begin.size())
      == script_begin)) {
    comment = SCRIPT;
    tree->push_text(c);
    return;
  }

  // Clean prev
  if (prev.size() > back_buffer_size) {
    prev = prev.substr(prev.size()-back_buffer_size, back_buffer_size);
  }

  while (true) {
    switch (state) {
      case 0:
        if (c == '{') {
          entry.push_back(0);
          next_state(1);
        } else {
          tree->push_text(c);
          next_state(0);
        }
      break;
      case 1:
        if (c == '{') {
          next_state(entry.back());
          entry.pop_back();
        } else {
          tree->up();
          tree->create();
          tree->down();
          tree->create();
          tree->down();

          move_state(2);
        }
      break;
      case 2:
        if (isalnum(c) || c == '@' || c == ':') {
          tree->push_name(c);
          next_state(2);
        } else {
          move_state(3);
        }
      break;
      case 3:
        if (c == '}') {
          next_state(4);
        } else if (c == '{') {
          entry.push_back(3);
          next_state(1);
        } else {
          tree->push_text(c);
          next_state(3);
        }
      break;
      case 4:
        if (c == '}') {
          next_state(3);
        } else {
          tree->up();
          tree->up();
          tree->create();
          tree->down();
          move_state(entry.back());
          entry.pop_back();
        }
      break;
      default: break;
    }

    if (!zero_cost_state_change) {
      break;
    }
  }
}

void Template::move_state (unsigned int state) {
    this->state = state;
    zero_cost_state_change = true;
}

void Template::next_state (unsigned int state) {
    this->state = state;
    zero_cost_state_change = false;
}
