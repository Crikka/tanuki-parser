#pragma once


namespace tanuki {
template <bool, typename TrueResult, typename FalseResult>
class if_;

template <typename TrueResult, typename FalseResult>
struct if_<true, TrueResult, FalseResult> {
  typedef TrueResult result;
};

template <typename TrueResult, typename FalseResult>
struct if_<false, TrueResult, FalseResult> {
  typedef FalseResult result;
};
}
