#pragma once

int tanuki_number_of_tests = 0;
int tanuki_number_of_succeed_tests = 0;

#define tanuki_expect(result, test, message)                                 \
  tanuki_number_of_tests++;                                                  \
  if (result == test) {                                                      \
    std::cout << "\x1B[34mTest[" message "]\x1B[0m : \x1B[32mSuccess\x1B[0m" \
              << std::endl;                                                  \
    tanuki_number_of_succeed_tests++;                                        \
  } else {                                                                   \
    std::cout << "\x1B[34mTestTest[" message                                 \
                 "]\x1B[0m : \x1B[31mFailed\x1B[0m" << std::endl;            \
  }

#define tanuki_not_expect(result, test, message)                             \
  tanuki_number_of_tests++;                                                  \
  if (result != test) {                                                      \
    std::cout << "\x1B[34mTest[" message "]\x1B[0m : \x1B[32mSuccess\x1B[0m" \
              << std::endl;                                                  \
    tanuki_number_of_succeed_tests++;                                        \
  } else {                                                                   \
    std::cout << "\x1B[34mTestTest[" message                                 \
                 "]\x1B[0m : \x1B[31mFailed\x1B[0m" << std::endl;            \
  }

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
