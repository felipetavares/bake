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

## Bakefile

The bakefile should be named `bakefile` in lower case without any
extension, just like you see it here (an example bakefile is provided, go
take a look!).

The bakefile is composed of key-value pairs, separated by an `=` symbol. Only one of these key-value pairs is allowrd per line.

All whitespace characters found in the key are removed, what means that all the keys below are equivalent:

    key=
    k ey=
    key =

The same goes for the value:

    key = value
    key=value
    key = v a l u e

are all the same.

If you desire to use whitespace characters in the value, put it between quotation marks `""`:

    name = "Felipe Tavares"

You can put any keys in your bakefile, but the ones that have real functionality are:

* **lang** - used to set the locale used by locale-specific functions (dates)
* **template** - path to the template file
* **output** - path to the output file
* **date** - formatting string in the [strftime](http://man7.org/linux/man-pages/man3/strftime.3.html) format.
* **host** - url to the directory where **output** is located.

## Available {vars}

These are the `vars` available for use in the template. All `vars` must be inside the main `{}` that defines the extension of a single post.

* **{content}** - post content
* **{title}** - post title
* **{date}** - post date, formated as specified by the **date** key in the bakefile.
* **{id}** - post id (used by permalinks)
* **{link}** - permalink to the post, uses the **id** var.
* **{author}** - post author, defined in the first line of each post

> Note about authors: to define an author for a post, begin the first line of a post with a tilde `~` character. The rest of the line until a `\n` is found will be saved to **{author}**.
