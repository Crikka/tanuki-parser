#include "ref.h"

namespace tanuki {
ref_implement_all_operator(int);
ref_implement_all_operator(float);
ref_implement_all_operator(double);
ref_implement_all_operator(long);
ref_implement_all_operator(long long);

ref<int> operator"" _ref(unsigned long long int in) {
  return ref<int>(new int(in));
}

ref<std::string> operator"" _ref(const char *in) {
  return ref<std::string>(new std::string(in));
}

ref<double> operator"" _ref(long double in) {
  return ref<double>(new double(in));
}

ref<char> operator"" _ref(char in) {
  return ref<char>(new char(in));
}
}
