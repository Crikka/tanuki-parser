#include <iostream>

#include "../tanuki/tanuki.h"

#include "framework.h"

#include <tuple>

void testRef();
void testLexer();
void testLexerConstant();
void testLexerSimple();
void testLexerUnary();
void testLexerBinary();

void testGrammar();
void testGrammarSelect();
void testGrammarSimple();
void testGrammarMultiple();
void testGrammarFunny();
void testGrammarWithOperator();
void testGrammarLeftRecursive();

int main(int argc, char* argv[]) {
  tanuki_run("Ref", testRef);
  tanuki_run("Lexer", testLexer);
  tanuki_run("Grammar", testGrammar);

  tanuki_summary;
}

void testRef() {
  use_tanuki;

  tanuki_result_expect(10, ref<int>(new int(5)) + ref<int>(new int(5)),
                       "Add int")
}

void testLexer() {
  tanuki_run("Constant", testLexerConstant);
  tanuki_run("Simple", testLexerSimple);
  tanuki_run("Unary", testLexerUnary);
  tanuki_run("Binary", testLexerBinary);
}

void testLexerConstant() {
  use_tanuki;

  // Space
  tanuki_match_expect(true, space()->match(" "), "Space True");
  tanuki_match_expect(false, space()->match("b"), "Space False");
  tanuki_match_expect(false, space()->match("  "), "Space Long False");

  // Tab
  tanuki_match_expect(true, tab()->match("\t"), "Tab True");
  tanuki_match_expect(false, tab()->match("b"), "Tab False");
  tanuki_match_expect(false, tab()->match("\t "), "Tab Long False");

  // Blank
  tanuki_match_expect(true, blank()->match(" "), "Blank space True");
  tanuki_match_expect(true, blank()->match("\t"), "Blank tab True");
  tanuki_match_expect(false, blank()->match("b"), "Blank False");
  tanuki_match_expect(false, blank()->match("\t "), "Blank Long False");

  // Line terminator
  tanuki_match_expect(true, lineTerminator()->match("\r"),
                      "LineTerminator MacOS Style True");
  tanuki_match_expect(true, lineTerminator()->match("\n"),
                      "LineTerminator Linux Style  True");
  tanuki_match_expect(false, blank()->match("n"), "LineTerminator False");

  // Digit
  tanuki_match_expect(true, digit()->match("0"), "Digit 0");
  tanuki_match_expect(true, digit()->match("5"), "Digit 5");
  tanuki_match_expect(true, digit()->match("9"), "Digit 9");
  tanuki_match_expect(false, digit()->match("90"), "Digit 90");
  tanuki_match_expect(false, digit()->match("a"), "Digit a");

  // Letter
  tanuki_match_expect(true, letter()->match("a"), "Letter a");
  tanuki_match_expect(true, letter()->match("e"), "Letter e");
  tanuki_match_expect(true, letter()->match("A"), "Letter A");
  tanuki_match_expect(true, letter()->match("Z"), "Letter Z");
  tanuki_match_expect(true, letter()->match("E"), "Letter E");
  tanuki_match_expect(false, letter()->match("Ab"), "Letter Ab");
  tanuki_match_expect(false, letter()->match("9"), "Letter 9");
}

void testLexerSimple() {
  use_tanuki;

  // Char
  tanuki_match_expect(true, constant('a')->match("a"), "Constant char True");
  tanuki_match_expect(false, constant('a')->match(""), "Constant char Empty");
  tanuki_match_expect(false, constant('a')->match("b"), "Constant char False");
  tanuki_match_expect(false, constant('a')->match("ab"),
                      "Constant char begin with char False");
  tanuki_match_expect(false, constant('a')->match("ba"),
                      "Constant char long match False");

  // Constant
  tanuki_match_expect(true, constant("Hello")->match("Hello"), "Constant True");
  tanuki_match_expect(false, constant("Hello")->match(""), "Constant Empty");
  tanuki_match_expect(false, constant("Hello")->match("hello"),
                      "Constant False");

  // AnyOf
  tanuki_match_expect(true, anyOf('a', 'b', 'c')->match("a"), "AnyOf True");
  tanuki_match_expect(true, anyOf('a', 'b', 'c')->match("c"), "AnyOf True");
  tanuki_match_expect(false, anyOf('a', 'b', 'c')->match("z"), "anyOf False");
  tanuki_match_expect(false, anyOf('a', 'b', 'c')->match("ab"), "anyOf False");

  // Consequent
  tanuki_match_expect(true, consequent(constant('+'), integer())->match("+45"),
                      "Consequent True");
  tanuki_match_expect(false, consequent(constant('+'), integer())->match("-45"),
                      "Consequent False");

  // Regex
  auto tWord = word(letter());

  tanuki_match_expect(true, tWord->match("Hello"), "Word True test");
  tanuki_result_expect("Hello", tWord->match("Hello"),
                       "Word expect Hello test");
  tanuki_match_expect(false, tWord->match(""), "Word Match Empty");
  // Integer
  tanuki_match_expect(true, integer()->match("52"), "Integer True");
  tanuki_result_expect(52, integer()->match("52"), "Integer Value (true)");
  tanuki_match_expect(false, integer()->match("Hello world"), "Integer false");
}

void testLexerUnary() {
  use_tanuki;

  // Not
  tanuki_match_expect(false, (not constant("Hello"))->match("Hello"),
                      "Not Constant True");
  tanuki_match_expect(true, (not constant("Hello"))->match("hello"),
                      "Not Constant False");

  // Optional
  tanuki_match_expect(true, (~constant("Hello"))->match("Hello"),
                      "Optional Constant True");
  tanuki_match_expect(true, (~constant("Hello"))->match("hello"),
                      "Not Constant False");

  // Plus
  tanuki_match_expect(true, (+constant("Hello"))->match("HelloHello"),
                      "Plus Constant True");

  tanuki_match_expect(false, (+constant("Hello"))->match("Hellohello"),
                      "Plus Constant False");
  tanuki_match_expect(
      true, (+(constant("Hello") or constant("hello")))->match("Hellohello"),
      "Plus Complex (or) true");
  tanuki_match_expect(
      false, (+(constant("Hello") or constant("hell")))->match("Hellohello"),
      "Plus Complex (or) false");

  // Star
  tanuki_match_expect(true, (*constant("Hello"))->match("HelloHello"),
                      "Star Constant True");
  tanuki_match_expect(true, (*constant("Hello"))->match("Hellohello"),
                      "Star Constant False");

  // StartWith
  tanuki_match_expect(true, startWith(constant("Hello"))->match("HelloHello"),
                      "StartWith Constant True");
  tanuki_match_expect(false, startWith(constant("Hello"))->match("HellaHello"),
                      "StartWith Constant False");
  tanuki_match_expect(true, startWith(constant("Hello"))->match("Hello"),
                      "StartWith exact size true");
  tanuki_match_expect(false, startWith(constant("Hello"))->match("Hella"),
                      "StartWith exact size False");
  tanuki_match_expect(false, startWith(constant("Hello"))->match("Hell"),
                      "StartWith lower size true");

  // EndWith
  tanuki_match_expect(true, endWith(constant("Hello"))->match("HelloHello"),
                      "EndWith Constant True");
  tanuki_match_expect(false, endWith(constant("Hello"))->match("HelloHella"),
                      "EndWith Constant False");
  tanuki_match_expect(true, endWith(constant("Hello"))->match("Hello"),
                      "EndWith exact size true");
  tanuki_match_expect(false, endWith(constant("Hello"))->match("Hella"),
                      "EndWith exact size False");
  tanuki_match_expect(false, endWith(constant("Hello"))->match("Hell"),
                      "EndWith lower size true");

  // Range
  tanuki_match_expect(
      true, range(constant("Hello"), constant("Hello"))->match("HelloHello"),
      "Range Constant True Same");
  tanuki_match_expect(
      true, range(constant("Hello"), constant("Hella"))->match("HelloHella"),
      "Range Constant True Diff");
  tanuki_match_expect(
      true, range(constant("Hello"), constant("Hello"))->match("Hello Hello"),
      "Range Constant True Space Same");
  tanuki_match_expect(
      true, range(constant("Hello"), constant("Hella"))->match("Hello Hella"),
      "Range Constant True Space Diff");
  tanuki_match_expect(false,
                      range(constant("Hello"), constant("666"))->match("Hello"),
                      "Range begin false");
  tanuki_match_expect(
      false, range(constant("Hello"), constant("Hella"))->match("Hella"),
      "Range end false");

  // Repeat
  tanuki_match_expect(true, repeat<3>(constant('6'))->match("666"),
                      "Repeat 666 True");
  tanuki_match_expect(false, repeat<3>(constant('6'))->match("676"),
                      "Repeat 666 False");
  tanuki_match_expect(true, repeat<4>(digit())->match("4525"),
                      "Repeat digicode True");
}

void testLexerBinary() {
  use_tanuki;

  // Or
  tanuki_match_expect(true,
                      (constant("Hello") or constant("world"))->match("Hello"),
                      "Or (1st member) Constant True");
  tanuki_match_expect(false,
                      (constant("Hello") or constant("world"))->match("hello"),
                      "Or Constant False");
  tanuki_match_expect(true,
                      (constant("Hello") or constant("hello"))->match("hello"),
                      "Not (2nd member) Constant True");

  // And
  tanuki_match_expect(true,
                      (constant("Hello") and constant("Hello"))->match("Hello"),
                      "And True");
  tanuki_match_expect(false,
                      (constant("Hello") and constant("world"))->match("hello"),
                      "And False");
  tanuki_match_expect(false,
                      (constant("Hello") and constant("hello"))->match("hello"),
                      "And one member ok but not other");
  tanuki_match_expect(true,
                      (constant("Hello") and word(letter()))->match("Hello"),
                      "And Constant + Regexp True");
}

void testGrammar() {
  tanuki_run("Select", testGrammarSelect);
  tanuki_run("Simple", testGrammarSimple);
  tanuki_run("Multiple", testGrammarMultiple);
  tanuki_run("Funny", testGrammarFunny);
  tanuki_run("Grammer with operator", testGrammarWithOperator);
  tanuki_run("Grammar with left recursive", testGrammarLeftRecursive);
}

void testGrammarSelect() {
  use_tanuki;

  struct A {};
  struct B : public A {};

  ref<Fragment<std::string>> fragment1 = Fragment<std::string>::select(
      constant("Hello"), range(constant("("), constant(")")));
  ref<Fragment<B>> fragmentB = fragment<B>();
  ref<Fragment<A>> fragmentA = fragment<A>();
  ref<Fragment<A>> fragment2 = Fragment<A>::select(fragmentB, fragmentA);
  (void)fragment2;  // Test is in compilation

  tanuki_result_expect("Hello", fragment1->match("Hello"), "Hello");
  tanuki_result_expect("(Hello)", fragment1->match("(Hello)"), "(Hello)");
}

void testGrammarSimple() {
  use_tanuki;

  ref<Fragment<int>> mainFragment = fragment<int>();
  master(mainFragment);

  mainFragment->handle(
      [](ref<int> i, ref<char>, ref<int> j) -> ref<int> { return (i + j); },
      integer(), constant('+'), integer());
  mainFragment->handle(
      [](ref<int> i, ref<char>, ref<int> j) -> ref<int> { return (i - j); },
      integer(), constant('-'), integer());
  mainFragment->handle(
      [](ref<int> i, ref<char>, ref<int> j) -> ref<int> { return (i * j); },
      integer(), constant('*'), integer());
  mainFragment->handle(
      [](ref<int> i, ref<char>, ref<int> j) -> ref<int> { return (i / j); },
      integer(), constant('/'), integer());

  tanuki_result_expect(10, mainFragment->match("5+5"), "Simple add");
  tanuki_result_expect(0, mainFragment->match("5-5"), "Simple less");
  tanuki_result_expect(25, mainFragment->match("5*5"), "Simple mult");
  tanuki_result_expect(1, mainFragment->match("5/5"), "Simple divide");

  tanuki_result_expect(20, mainFragment->match("15+5"), "Simple great add");
  tanuki_result_expect(-10, mainFragment->match("5-15"), "Simple great less");
  tanuki_result_expect(2500, mainFragment->match("50*50"), "Simple great mult");
  tanuki_result_expect(10, mainFragment->match("500/50"),
                       "Simple great divide");

  mainFragment->skip(blank());

  tanuki_result_expect(10, mainFragment->match("5 + 5"), "Simple add space");
  tanuki_result_expect(0, mainFragment->match("5  - 5"), "Simple less tab");
  tanuki_result_expect(25, mainFragment->match("5 * 5"), "Simple mult mix");
  tanuki_result_expect(1, mainFragment->match("5      /5"),
                       "Simple divide multispace");

  mainFragment->skip(constant('#'), constant('(') or constant(')'));

  tanuki_result_expect(10, mainFragment->match("(5 + #5"),
                       "Simple add space hard");
  tanuki_result_expect(0, mainFragment->match("5  ###- 5"),
                       "Simple less tab hard");
  tanuki_result_expect(25, mainFragment->match("5 *# #5"),
                       "Simple mult mix hard");
  tanuki_result_expect(1, mainFragment->match("5   ()##   /5"),
                       "Simple divide multispace hard");
}

void testGrammarMultiple() {
  use_tanuki;

  ref<Fragment<std::string>> mainFragment = fragment<std::string>();
  ref<Fragment<int>> mainFragment2 = fragment<int>();
  ref<Fragment<int>> sub = fragment<int>();

  mainFragment->handle(
      [](ref<std::string>, ref<char>, ref<std::string> word) { return word; },
      constant("hey"), space(), word(letter()));
  mainFragment->handle(
      [](ref<int> i, ref<char>, ref<std::string> word) { return word; }, sub,
      space(), word(letter()));
  mainFragment->handle([](ref<std::string> word) { return word; },
                       word(letter()));

  mainFragment2->handle(
      [](ref<int> i, ref<char>, ref<int> number) { return (i + number); }, sub,
      space(), integer());

  sub->handle([](ref<std::string>) { return 25_ref; }, constant("hello"));

  tanuki_result_expect(25, sub->match("hello"), "Simple verification");
  tanuki_result_expect("Everyone", mainFragment->match("Everyone"),
                       "Simple verification");
  tanuki_result_expect("Everyone", mainFragment->match("hey Everyone"),
                       "Simple verification");
  tanuki_result_expect("Everyone", mainFragment->match("hello Everyone"),
                       "Simple string");
  tanuki_result_expect(50, mainFragment2->match("hello 25"), "Simple int");
}

void testGrammarFunny() {
  use_tanuki;

  typedef std::function<ref<int>(ref<int>, ref<int>)> OperatorReturnType;

  class Operator : public tanuki::Token<OperatorReturnType> {
   public:
    ref<OperatorReturnType> match(const tanuki::String& in) {
      if (in.size() == 1) {
        if (in[0] == '+') {
          return ref<OperatorReturnType>(new OperatorReturnType(
              [](ref<int> x, ref<int> y) -> ref<int> { return x + y; }));
        } else if (in[0] == '-') {
          return ref<OperatorReturnType>(new OperatorReturnType(
              [](ref<int> x, ref<int> y) -> ref<int> { return x - y; }));
        } else if (in[0] == '*') {
          return ref<OperatorReturnType>(new OperatorReturnType(
              [](ref<int> x, ref<int> y) -> ref<int> { return x * y; }));
        } else if (in[0] == '/') {
          return ref<OperatorReturnType>(new OperatorReturnType(
              [](ref<int> x, ref<int> y) -> ref<int> { return x / y; }));
        } else {
          return ref<OperatorReturnType>();
        }
      } else {
        return ref<OperatorReturnType>();
      }
    }

    tanuki::Piece<OperatorReturnType> consume(const tanuki::String& in) {
      if (in.empty()) {
        return tanuki::Piece<OperatorReturnType>{0, ref<OperatorReturnType>()};
      } else {
        if (in[0] == '+') {
          return tanuki::Piece<OperatorReturnType>{
              1,
              ref<OperatorReturnType>(new OperatorReturnType(
                  [](ref<int> x, ref<int> y) -> ref<int> { return x + y; }))};
        } else if (in[0] == '-') {
          return tanuki::Piece<OperatorReturnType>{
              1,
              ref<OperatorReturnType>(new OperatorReturnType(
                  [](ref<int> x, ref<int> y) -> ref<int> { return x - y; }))};
        } else if (in[0] == '*') {
          return tanuki::Piece<OperatorReturnType>{
              1,
              ref<OperatorReturnType>(new OperatorReturnType(
                  [](ref<int> x, ref<int> y) -> ref<int> { return x * y; }))};
        } else if (in[0] == '/') {
          return tanuki::Piece<OperatorReturnType>{
              1,
              ref<OperatorReturnType>(new OperatorReturnType(
                  [](ref<int> x, ref<int> y) -> ref<int> { return x / y; }))};
        } else {
          return tanuki::Piece<OperatorReturnType>{0,
                                                   ref<OperatorReturnType>()};
        }
      }
    }
  };

  ref<Fragment<int>> mainFragment = fragment<int>();
  master(mainFragment);

  mainFragment->handle([](ref<int> x, ref<OperatorReturnType> op,
                          ref<int> y) { return op->operator()(x, y); },
                       integer(), ref<Operator>(new Operator), integer());
  mainFragment->handle(
      [](ref<std::string>, ref<int> in, ref<std::string>) { return in; },
      constant("("), mainFragment, constant(")"));

  tanuki_result_expect(10, mainFragment->match("5+5"), "Add");
  tanuki_result_expect(0, mainFragment->match("5-5"), "Less");
  tanuki_result_expect(25, mainFragment->match("5*5"), "Mult");
  tanuki_result_expect(1, mainFragment->match("5/5"), "Divide");
  tanuki_result_expect(10, mainFragment->match("(5+5)"), "Add parenthesis");
  tanuki_result_expect(10, mainFragment->match("((((5+5))))"),
                       "Add lot of parenthesis");

  mainFragment->skip(space());

  tanuki_result_expect(10, mainFragment->match("((( ( 5 + 5 )) ) )"),
                       "Add lot of parenthesis with blank");
}

void testGrammarWithOperator() {
  use_tanuki;

  ref<Fragment<int>> mainFragment = fragment<int>();
  master(mainFragment);

  mainFragment->handle([](ref<int> i, ref<std::string>,
                          ref<char>) -> ref<int> { return (i + 1); },
                       integer(), constant("++"), constant(';'));
  mainFragment->handle([](ref<char>, ref<std::vector<ref<int>>> in,
                          ref<char>) -> ref<int> { return in->back(); },
                       constant('{'), +mainFragment, constant('}'));
  tanuki_result_expect(6, mainFragment->match("5++;"), "Simple incr");
  tanuki_result_expect(10, mainFragment->match("{5++;9++;}"), "Double incr");
  tanuki_result_expect(25, mainFragment->match("{1++;2++;3++;4++;24++;}"),
                       "Lot of incr");
}

void testGrammarLeftRecursive() {
  use_tanuki;

  ref<Fragment<int>> type = fragment<int>();

  type->handle([](auto) -> ref<int> { return 0_ref; }, constant("int"));
  type->handle([](auto, auto) -> ref<int> { return 0_ref; }, type, constant('%'));
  type->handle([](auto, auto) -> ref<int> { return 0_ref; }, type, constant('!'));
  // type->handle([](auto) -> ref<int> { return 0_ref; }, type); Assert at runtime

  tanuki_match_expect(true, type->match("int"), "Left Recursive without recursive");
  tanuki_match_expect(true, type->match("int%"), "Left Recursive with one recursive");
  tanuki_match_expect(true, type->match("int%%"), "Left Recursive with two recursive");
  tanuki_match_expect(true, type->match("int%%!"), "Left Recursive melt");
  tanuki_match_expect(true, type->match("int%!%%"), "Left Recursive melt of the death");
}
