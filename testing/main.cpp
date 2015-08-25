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
void testGrammarSimple();
void testGrammarMultiple();
void testGrammarFunny();
void testGrammarWithOperator();

int main(int argc, char* argv[]) {
  tanuki_run("Ref", testRef);
  tanuki_run("Lexer", testLexer);
  tanuki_run("Grammar", testGrammar);

  tanuki_summary;
}

void testRef() {
  use_tanuki;

  tanuki_expect(10, ref<int>(new int(5)) + ref<int>(new int(5)), "Add int")
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
  tanuki_match_expect(true, lineTerminator()->match("\r"), "LineTerminator MacOS Style True");
  tanuki_match_expect(true, lineTerminator()->match("\n"), "LineTerminator Linux Style  True");
  tanuki_match_expect(false, blank()->match("n"), "LineTerminator False");
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

  // Regex
  tanuki::ref<tanuki::lexer::RegexToken> cst_regexToken = regex("Hello");
  tanuki::ref<tanuki::lexer::RegexToken> var_regexToken = regex("\\w+");

  tanuki_match_expect(true, cst_regexToken->match("Hello"),
                      "Regex exact Match True test");
  tanuki_expect("Hello", cst_regexToken->match("Hello"),
                "Regex constant expect Hello test");
  tanuki_match_expect(false, cst_regexToken->match(""),
                      "Regex exact Match Empty");
  tanuki_match_expect(true, var_regexToken->match("").isNull(),
                      "Regex exact expect Empty Result");

  tanuki_match_expect(true, var_regexToken->match("Hello"), "Regex \\w");

  tanuki_expect("Hello", var_regexToken->match("Hello"),
                "Regex \\w expect Hello test");
  tanuki_match_expect(false, var_regexToken->match(""), "Regex \\w Empty");
  tanuki_match_expect(true, var_regexToken->match("").isNull(),
                      "Regex \\w expect Empty Result");

  // Integer
  tanuki::ref<tanuki::lexer::IntegerToken> int_ = integer();

  tanuki_match_expect(true, int_->match("52"), "Integer True");
  tanuki_expect(52, int_->match("52"), "Integer Value (true)");
  tanuki_match_expect(false, int_->match("Hello world"), "Integer false");
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
  tanuki_match_expect(
      true, (+(constant("Hello") or regex("\\w\\w")))->match("Hellohello"),
      "Plus Complex (regex) true");
  tanuki_match_expect(
      true, (+(constant("Hello") or regex("\\w\\w")))->match("Hellohelloo"),
      "Plus Complex (regex/hard) true");
  tanuki_match_expect(
      false, (+(constant("Hell") or regex("\\w\\w")))->match("Hellohelloo"),
      "Plus Complex (regex) false");

  // Star
  tanuki_match_expect(true, (*constant("Hello"))->match("HelloHello"),
                      "Star Constant True");
  tanuki_match_expect(true, (*constant("Hello"))->match("Hellohello"),
                      "Star Constant False");

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
                      (constant("Hello") and regex("\\w+"))->match("Hello"),
                      "And Constant + Regexp True");
}

void testGrammar() {
  tanuki_run("Simple", testGrammarSimple);
  tanuki_run("Multiple", testGrammarMultiple);
  tanuki_run("Funny", testGrammarFunny);
  tanuki_run("Grammer with operator", testGrammarWithOperator);
}

void testGrammarSimple() {
  use_tanuki;

  undirect_ref<Fragment<int>> mainFragment = fragment<int>();

  mainFragment->on(integer(), constant('+'), integer())
      ->execute([](ref<int> i, ref<std::string>, ref<int> j)
                    -> ref<int> { return (i + j); })
      ->on(integer(), constant('-'), integer())
      ->execute([](ref<int> i, ref<std::string>, ref<int> j)
                    -> ref<int> { return (i - j); })
      ->on(integer(), constant('*'), integer())
      ->execute([](ref<int> i, ref<std::string>, ref<int> j)
                    -> ref<int> { return (i * j); })
      ->on(integer(), constant('/'), integer())
      ->execute([](ref<int> i, ref<std::string>, ref<int> j)
                    -> ref<int> { return (i / j); });

  tanuki_expect(10, mainFragment->match("5+5"), "Simple add");
  tanuki_expect(0, mainFragment->match("5-5"), "Simple less");
  tanuki_expect(25, mainFragment->match("5*5"), "Simple mult");
  tanuki_expect(1, mainFragment->match("5/5"), "Simple divide");

  tanuki_expect(20, mainFragment->match("15+5"), "Simple great add");
  tanuki_expect(-10, mainFragment->match("5-15"), "Simple great less");
  tanuki_expect(2500, mainFragment->match("50*50"), "Simple great mult");
  tanuki_expect(10, mainFragment->match("500/50"), "Simple greatdivide");

  mainFragment->ignore(blank());

  tanuki_expect(10, mainFragment->match("5 + 5"), "Simple add space");
  tanuki_expect(0, mainFragment->match("5  - 5"), "Simple less tab");
  tanuki_expect(25, mainFragment->match("5 * 5"), "Simple mult mix");
  tanuki_expect(1, mainFragment->match("5      /5"),
                "Simple divide multispace");

  mainFragment->ignore(constant('#'), constant('(') or constant(')'));

  tanuki_expect(10, mainFragment->match("(5 + #5"), "Simple add space hard");
  tanuki_expect(0, mainFragment->match("5  ###- 5"), "Simple less tab hard");
  tanuki_expect(25, mainFragment->match("5 *# #5"), "Simple mult mix hard");
  tanuki_expect(1, mainFragment->match("5   ()##   /5"),
                "Simple divide multispace hard");
}

void testGrammarMultiple() {
  use_tanuki;

  undirect_ref<Fragment<std::string>> mainFragment = fragment<std::string>();
  undirect_ref<Fragment<int>> mainFragment2 = fragment<int>();
  undirect_ref<Fragment<int>> sub = fragment<int>();

  mainFragment->on(constant("hey"), space(), regex("\\w+"))
      ->execute([](ref<std::string>, ref<std::string>, ref<std::string> word) {
        return word;
      })
      ->on(sub, space(), regex("\\w+"))
      ->execute([](ref<int> i, ref<std::string>, ref<std::string> word) {
        return word;
      })

      ->on(regex("\\w+"))
      ->execute([](ref<std::string> word) { return word; });

  mainFragment2->on(sub, space(), integer())
      ->execute([](ref<int> i, ref<std::string>, ref<int> number) {
        return (i + number);
      });

  sub->on(constant("hello"))->execute([](ref<std::string>) { return 25_ref; });

  tanuki_expect(25, sub->match("hello"), "Simple verification");
  tanuki_expect("Everyone", mainFragment->match("Everyone"),
                "Simple verification");
  tanuki_expect("Everyone", mainFragment->match("hey Everyone"),
                "Simple verification");
  tanuki_expect("Everyone", mainFragment->match("hello Everyone"),
                "Simple string");
  tanuki_expect(50, mainFragment2->match("hello 25"), "Simple int");
}

void testGrammarFunny() {
  use_tanuki;

  typedef std::function<ref<int>(ref<int>, ref<int>)> OperatorReturnType;

  class Operator : public tanuki::lexer::Token<OperatorReturnType> {
   public:
    ref<OperatorReturnType> match(const std::string& in) {
      if (in.size() == 1) {
        if (in[0] == '+') {
          return ref<OperatorReturnType>(new OperatorReturnType(
              [](ref<int> x, ref<int> y) -> ref<int> { return x + y; }));
        }
        if (in[0] == '-') {
          return ref<OperatorReturnType>(new OperatorReturnType(
              [](ref<int> x, ref<int> y) -> ref<int> { return x - y; }));
        }
        if (in[0] == '*') {
          return ref<OperatorReturnType>(new OperatorReturnType(
              [](ref<int> x, ref<int> y) -> ref<int> { return x * y; }));
        }
        if (in[0] == '/') {
          return ref<OperatorReturnType>(new OperatorReturnType(
              [](ref<int> x, ref<int> y) -> ref<int> { return x / y; }));
        }
      } else {
        return ref<OperatorReturnType>();
      }
    }
  };

  undirect_ref<Fragment<int>> mainFragment = fragment<int>();

  mainFragment->on(integer(), undirect_ref<Operator>(new Operator), integer())
      ->execute([](ref<int> x, ref<OperatorReturnType> op, ref<int> y) {
        return op->operator()(x, y);
      })
      ->on(constant("("), mainFragment, constant(")"))
      ->execute(
          [](ref<std::string>, ref<int> in, ref<std::string>) { return in; });

  tanuki_expect(10, mainFragment->match("5+5"), "Add");
  tanuki_expect(0, mainFragment->match("5-5"), "Less");
  tanuki_expect(25, mainFragment->match("5*5"), "Mult");
  tanuki_expect(1, mainFragment->match("5/5"), "Divide");
  tanuki_expect(10, mainFragment->match("(5+5)"), "Add parenthesis");
  tanuki_expect(10, mainFragment->match("((((5+5))))"),
                "Add lot of parenthesis");

  mainFragment->ignore(blank());

  tanuki_expect(10, mainFragment->match("((( ( 5 + 5 )) ) ) "),
                "Add lot of parenthesis with blank");
}

void testGrammarWithOperator() {
  use_tanuki;

  undirect_ref<Fragment<int>> mainFragment = fragment<int>();

  mainFragment->on(integer(), constant("++"), constant(';'))
      ->execute([](ref<int> i, ref<std::string>, ref<std::string>)
                    -> ref<int> { return (i + 1); })
      ->on(constant('{'), +mainFragment, constant('}'))
      ->execute([](ref<std::string>, ref<std::vector<ref<int>>> in,
                   ref<std::string>) -> ref<int> { return in->back(); });

  tanuki_expect(6, mainFragment->match("5++;"), "Simple incr");
  tanuki_expect(10, mainFragment->match("{5++;9++;}"), "Double incr");
  tanuki_expect(25, mainFragment->match("{1++;2++;3++;4++;24++;}"),
                "Lot of incr");
}
