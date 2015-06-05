# Bake

Bake is a templated blog generator.

Bake was originally writen in C, for use as blogging tool
at [ctrl-c club](http://ctrl-c.club/). This version is
a new implementation in C++.

You can see an example blog that uses Bake
[here](http://poesiacompilada.com/blog/).
Another in [here](http://ctrl-c.club/~philips). (as of writing it still uses the
original implementation)

Bake is intended to be used in a make-like fashion:

 1. Create a directory, enter it
 2. Write a file named `bakefile` containing some rules
 about how to process the input.
 3. Write a template file anywhere you like (the `bakefile`
 should point to it)
 4. Write blog posts in the format **<filename>**.markdown
 5. Run `bake`
