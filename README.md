# Tanuki
## Overview

Tanuki is a cpp14 library that offer a parser generator in code. Tanuki doesn't require other library to be compiled, internally it include <a href="http://utfcpp.sourceforge.net/">utf-cpp</a>.

The design of Tanuki is to be simple to use for reify piece of parser. Tanuki was inspired by PetitParser, but Tanuki offers a different approach than PetitParser.

## How to use

### Linux

Where you want to download Tanuki :

<pre><code>
git clone https://github.com/Crikka/tanuki-parser.git
cd tanuki-parser
mkdir build
cd build
cmake ..
make
sudo make install
</code></pre>

## Concepts

Tanuki uses two things of entities, token & fragment.

A token is an atomic piece of parser, for example DigitToken represents a digit (0-9).

A fragment is a constructed piece of parser.

Each of this entities can match or consume an input string. 

----- TO COMPLETE -----
