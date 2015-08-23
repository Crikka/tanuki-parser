#include <iostream>

#include "../tanuki/tanuki.h"

#include "framework.h"


void testLexer();
void testLexerConstant();
void testLexerSimple();
void testLexerUnary();
void testLexerBinary();

void testGrammar();
void testGrammarSimple();

int main(int argc, char* argv[]) {
  testLexer();
  testGrammar();

  tanuki_summary;
}

void testLexer() {
  testLexerConstant();
  testLexerSimple();
  testLexerUnary();
  testLexerBinary();
}

void testLexerConstant() {
  use_tanuki;

  // Space
  tanuki_expect(true, space()->match(" "), "Space True");
  tanuki_expect(false, space()->match("b"), "Space False");
  tanuki_expect(false, space()->match("  "), "Space Long False");

  // Tab
  tanuki_expect(true, tab()->match("\t"), "Tab True");
  tanuki_expect(false, tab()->match("b"), "Tab False");
  tanuki_expect(false, tab()->match("\t "), "Tab Long False");

  // Blank
  tanuki_expect(true, blank()->match(" "), "Blank space True");
  tanuki_expect(true, blank()->match("\t"), "Blank tab True");
  tanuki_expect(false, blank()->match("b"), "Blank False");
  tanuki_expect(false, blank()->match("\t "), "Blank Long False");
}

void testLexerSimple() {
  use_tanuki;

  // Char
  tanuki_expect(true, constant('a')->match("a"), "Constant char True");
  tanuki_expect(false, constant('a')->match(""), "Constant char Empty");
  tanuki_expect(false, constant('a')->match("b"), "Constant char False");
  tanuki_expect(false, constant('a')->match("ab"),
                "Constant char begin with char False");
  tanuki_expect(false, constant('a')->match("ba"),
                "Constant char long match False");

  // Constant
  tanuki_expect(true, constant("Hello")->match("Hello"), "Constant True");
  tanuki_expect(false, constant("Hello")->match(""), "Constant Empty");
  tanuki_expect(false, constant("Hello")->match("hello"), "Constant False");

  // Regex
  tanuki::ref<tanuki::lexer::RegexToken> cst_regexToken = regex("Hello");
  tanuki::ref<tanuki::lexer::RegexToken> var_regexToken = regex("\\w+");

  tanuki_expect(true, cst_regexToken->match("Hello"),
                "Regex exact Match True test");
  tanuki_expect("Hello", cst_regexToken->match("Hello"),
                "Regex constant expect Hello test");
  tanuki_expect(false, cst_regexToken->match(""), "Regex exact Match Empty");
  tanuki_expect(true, var_regexToken->match("").isNull(),
                "Regex exact expect Empty Result");

  tanuki_expect(true, var_regexToken->match("Hello"), "Regex \\w");

  tanuki_expect("Hello", var_regexToken->match("Hello"),
                "Regex \\w expect Hello test");
  tanuki_expect(false, var_regexToken->match(""), "Regex \\w Empty");
  tanuki_expect(true, var_regexToken->match("").isNull(),
                "Regex \\w expect Empty Result");

  // Integer
  tanuki::ref<tanuki::lexer::IntegerToken> int_ = integer();

  tanuki_expect(true, int_->match("52"), "Integer True");
  tanuki_expect(52, int_->match("52"), "Integer Value (true)");
  tanuki_expect(false, int_->match("Hello world"), "Integer false");
}

void testLexerUnary() {
  use_tanuki;

  // Not
  tanuki_expect(false, (not constant("Hello"))->match("Hello"),
                "Not Constant True");
  tanuki_expect(true, (not constant("Hello"))->match("hello"),
                "Not Constant False");

  // Optional
  tanuki_expect(true, (~constant("Hello"))->match("Hello"),
                "Optional Constant True");
  tanuki_expect(true, (~constant("Hello"))->match("hello"),
                "Not Constant False");

  // Plus
  tanuki_expect(true, (+constant("Hello"))->match("HelloHello"),
                "Plus Constant True");

  tanuki_expect(false, (+constant("Hello"))->match("Hellohello"),
                "Plus Constant False");
  tanuki_expect(
      true, (+(constant("Hello") or constant("hello")))->match("Hellohello"),
      "Plus Complex (or) true");
  tanuki_expect(false,
                (+(constant("Hello") or constant("hell")))->match("Hellohello"),
                "Plus Complex (or) false");
  tanuki_expect(true,
                (+(constant("Hello") or regex("\\w\\w")))->match("Hellohello"),
                "Plus Complex (regex) true");
  tanuki_expect(true,
                (+(constant("Hello") or regex("\\w\\w")))->match("Hellohelloo"),
                "Plus Complex (regex/hard) true");
  tanuki_expect(false,
                (+(constant("Hell") or regex("\\w\\w")))->match("Hellohelloo"),
                "Plus Complex (regex) false");

  // Star
  tanuki_expect(true, (*constant("Hello"))->match("HelloHello"),
                "Star Constant True");
  tanuki_expect(true, (*constant("Hello"))->match("Hellohello"),
                "Star Constant False");
}

void testLexerBinary() {
  use_tanuki;

  // Or
  tanuki_expect(true, (constant("Hello") or constant("world"))->match("Hello"),
                "Or (1st member) Constant True");
  tanuki_expect(false, (constant("Hello") or constant("world"))->match("hello"),
                "Or Constant False");
  tanuki_expect(true, (constant("Hello") or constant("hello"))->match("hello"),
                "Not (2nd member) Constant True");

  // And
  tanuki_expect(true, (constant("Hello") and constant("Hello"))->match("Hello"),
                "And True");
  tanuki_expect(false,
                (constant("Hello") and constant("world"))->match("hello"),
                "And False");
  tanuki_expect(false,
                (constant("Hello") and constant("hello"))->match("hello"),
                "And one member ok but not other");
  tanuki_expect(true, (constant("Hello") and regex("\\w+"))->match("Hello"),
                "And Constant + Regexp True");

  // Continuous
  tanuki_expect(true,
                (constant("Hello"), constant("Hello"))->match("HelloHello"),
                "Continuous Constant true");
  tanuki_expect(false,
                (constant("Hello"), constant("world"))->match("helloworld"),
                "Continuous Constant false");
  tanuki_expect(true, (constant("Hello"), regex("\\w+"))->match("HelloWorld"),
                "Continuous regex true");
  tanuki_expect(true, (constant("Hello"), space(), regex("\\w+"), constant('!'))
                          ->match("Hello World!"),
                "Continuous regex hard true");
  tanuki_expect(true, (constant("Hello"), +space(), regex("\\w+"),
                       constant('!'))->match("Hello  World!"),
                "Continuous regex harder true");
  tanuki_expect(true, (constant("Hello"), *space(), regex("\\w+"),
                       constant('!'), ~constant(";"))->match("HelloWorld!"),
                "Continuous regex harder false");
}

void testGrammar() { testGrammarSimple(); }

void testGrammarSimple() {
  use_tanuki;

  Fragment<int> fragment;

  fragment.on(integer(), constant('+'), integer())
      ->execute([](int i, std::string, int j) -> int* { return new int(i + j); })
      ->on(integer(), constant('-'), integer())
      ->execute([](int i, std::string, int j) -> int* { return new int(i - j); })
      ->on(integer(), constant('*'), integer())
      ->execute([](int i, std::string, int j) -> int* { return new int(i * j); })
      ->on(integer(), constant('/'), integer())
      ->execute([](int i, std::string, int j) -> int* { return new int(i / j); });

  tanuki_expect(10, fragment.match("5+5"), "Grammar Simple add");
  tanuki_expect(0, fragment.match("5-5"), "Grammar Simple less");
  tanuki_expect(25, fragment.match("5*5"), "Grammar Simple mult");
  tanuki_expect(1, fragment.match("5/5"), "Grammar Simple divide");

  tanuki_expect(20, fragment.match("15+5"), "Grammar Simple great add");
  tanuki_expect(-10, fragment.match("5-15"), "Grammar Simple great less");
  tanuki_expect(2500, fragment.match("50*50"), "Grammar Simple great mult");
  tanuki_expect(10, fragment.match("500/50"), "Grammar Simple greatdivide");

  fragment.ignore(blank());

  tanuki_expect(10, fragment.match("5 + 5"), "Grammar Simple add space");
  tanuki_expect(0, fragment.match("5  - 5"), "Grammar Simple less tab");
  tanuki_expect(25, fragment.match("5 * 5"), "Grammar Simple mult mix");
  tanuki_expect(1, fragment.match("5      /5"), "Grammar Simple divide multispace");

  fragment.ignore(constant('#'), constant('(') or constant(')'));

  tanuki_expect(10, fragment.match("(5 + #5"), "Grammar Simple add space hard");
  tanuki_expect(0, fragment.match("5  ###- 5"), "Grammar Simple less tab hard");
  tanuki_expect(25, fragment.match("5 *# #5"), "Grammar Simple mult mix hard");
  tanuki_expect(1, fragment.match("5   ()##   /5"), "Grammar Simple divide multispace hard");

  /*
   * Work in progress
   * ----------------

  tanuki::ref<Fragment<int>> sub(new Fragment<int>());

  sub->on(constant("hello"))->execute([](std::string) { return new int(25); });

  fragment.on(sub, integer())->execute([](int, int) { return 50; });
  */
}
