#pragma once

#include <unordered_map>
#include <vector>

int tanuki_number_of_tests = 0;
int tanuki_number_of_succeed_tests = 0;
std::unordered_map<std::string, int> tanuki_local_number_of_tests;
std::unordered_map<std::string, int> tanuki_local_number_of_succeed_tests;
std::vector<std::string> tanuki_tests_names_stack;

#define tanuki_match_expect(result, test, message)                           \
  tanuki_number_of_tests++;                                                  \
  for (const std::string &local_name : tanuki_tests_names_stack) {           \
    tanuki_local_number_of_tests[local_name]++;                              \
  }                                                                          \
  if (!tanuki_tests_names_stack.empty()) {                                   \
    std::cout << "|";                                                        \
  }                                                                          \
  for (int i = 0; i < tanuki_tests_names_stack.size(); i++) {                \
    std::cout << "=";                                                        \
  }                                                                          \
  if (!tanuki_tests_names_stack.empty()) {                                   \
    std::cout << "> ";                                                       \
  }                                                                          \
  if (result == (bool)test) {                                                \
    std::cout << "\x1B[34mTest[" message "]\x1B[0m : \x1B[32mSuccess\x1B[0m" \
              << std::endl;                                                  \
    tanuki_number_of_succeed_tests++;                                        \
    for (const std::string &local_name : tanuki_tests_names_stack) {         \
      tanuki_local_number_of_succeed_tests[local_name]++;                    \
    }                                                                        \
  } else {                                                                   \
    std::cout << "\x1B[34mTestTest[" message                                 \
                 "]\x1B[0m : \x1B[31mFailed\x1B[0m" << std::endl;            \
  }

#define tanuki_result_expect(result, test, message)                                 \
  tanuki_number_of_tests++;                                                  \
  for (const std::string &local_name : tanuki_tests_names_stack) {           \
    tanuki_local_number_of_tests[local_name]++;                              \
  }                                                                          \
  if (!tanuki_tests_names_stack.empty()) {                                   \
    std::cout << "|";                                                        \
  }                                                                          \
  for (int i = 0; i < tanuki_tests_names_stack.size(); i++) {                \
    std::cout << "=";                                                        \
  }                                                                          \
  if (!tanuki_tests_names_stack.empty()) {                                   \
    std::cout << "> ";                                                       \
  }                                                                          \
  if (*tanuki::dereference(test) == result) {                                                      \
    std::cout << "\x1B[34mTest[" message "]\x1B[0m : \x1B[32mSuccess\x1B[0m" \
              << std::endl;                                                  \
    tanuki_number_of_succeed_tests++;                                        \
    for (const std::string &local_name : tanuki_tests_names_stack) {         \
      tanuki_local_number_of_succeed_tests[local_name]++;                    \
    }                                                                        \
  } else {                                                                   \
    std::cout << "\x1B[34mTestTest[" message                                 \
                 "]\x1B[0m : \x1B[31mFailed\x1B[0m" << std::endl;            \
  }

#define tanuki_run(name, fct)                                               \
  tanuki_local_number_of_tests[name] = 0;                                   \
  tanuki_local_number_of_succeed_tests[name] = 0;                           \
  tanuki_tests_names_stack.push_back(name);                                 \
  std::cout << "\x1B[34mRunning\x1B[0m ~> \x1B[33m" name "\x1B[0m"          \
            << std::endl;                                                   \
  fct();                                                                    \
  tanuki_tests_names_stack.pop_back();                                      \
  std::cout << "\x1B[34mResult of " name "\x1B[0m ~> \x1B[36m"              \
            << tanuki_local_number_of_succeed_tests[name] << "/"            \
            << tanuki_local_number_of_tests[name] << "("                    \
            << (tanuki_local_number_of_tests[name] == 0                     \
                    ? 0                                                     \
                    : ((double)tanuki_local_number_of_succeed_tests[name] / \
                       (double)tanuki_local_number_of_tests[name])) *       \
                   100 << "%)\x1B[0m!" << std::endl;

#define tanuki_summary                                                    \
  std::cout                                                               \
      << "\x1B[33mLet me do the account... Ok! The result is... \x1B[36m" \
      << tanuki_number_of_succeed_tests << "/" << tanuki_number_of_tests  \
      << "("                                                              \
      << (tanuki_number_of_tests == 0                                     \
              ? 0                                                         \
              : ((double)tanuki_number_of_succeed_tests /                 \
                 (double)tanuki_number_of_tests)) *                       \
             100 << "%)\x1B[33m!\x1B[0m\n"                                \
      << ((tanuki_number_of_succeed_tests == tanuki_number_of_tests)      \
              ? "\x1B[32m/o/ \\o/ \\o\\ ~~ All tests are successful! "    \
                "Congratulation! :)\x1B[0m"                               \
              : "\x1B[31mAt least one test is failed! Sorry :(\x1B[0m")   \
      << std::endl;                                                       \
  return (tanuki_number_of_succeed_tests != tanuki_number_of_tests);
