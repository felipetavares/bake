/*
        Copyright © 2015 Felipe Tavares <felipe.oltavares@gmail.com>

        This work is free. You can redistribute it and/or modify it under the
        terms of the Do What The Fuck You Want To Public License, Version 2,
        as published by Sam Hocevar. See the LICENSE file for more details.
 */

#ifndef BAKE_TEMPLATE_H
#define BAKE_TEMPLATE_H

#include <vector>
#include <string>
#include "bakefile.hpp"
#include "post.hpp"
using namespace std;

namespace bake {
  class Tree {
    class Node {
public:

      string name;
      string text;

      Node *parent;
      vector<Node *> children;

      Node(Node *);
      ~Node();

      void         add(Node *);
      Node*        child();

      unsigned int count();

      string       get_post(Post *, string = "");
      string       get_begin();
      string       get_end();
      Node*        get_post_template();

private:

      bool is_extension(string &, string);
      string to_html(string&);
    };

    Node *root;
    Node *ptr;

public:

    Tree();
    ~Tree();

    Tree(Node *);

    void         create();

    void         up();
    void         down();

    void         push_name(char);
    void         push_text(char);

    unsigned int count();

    string       get_post(Post *);
    string       get_begin();
    string       get_end();
    Tree*        get_post_template();
  };

  class Template {
    static const unsigned int back_buffer_size = 32;
    enum CommentType {
      NONE, STYLE, SCRIPT
    };

    string path;
    unsigned int state;
    bool zero_cost_state_change;
    CommentType comment;
    vector<unsigned int> entry;
    string prev;

    Tree *tree;

public:

    Template(string);
    ~Template();

    Tree* read();

private:

    void process(char);
    void move_state(unsigned int);
    void next_state(unsigned int);

    // Avoid copying
    Template(const Template&);
    Template& operator=(const Template&);
  };
}

#endif /* BAKE_TEMPLATE_H */
