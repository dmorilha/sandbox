#include <array>
#include <iostream>

#include <cassert>

/*
 * From Chapter 7 of Algorithm Design Manual
 */

template <typename T, int K>
struct Backtracker {
  using Array = std::array<T, K>;
  using size_t = std::size_t;

  virtual auto generate_candidates(const Array & input, const size_t input_size, Array & candidates, size_t & number_of_candidates) -> void = 0;
  virtual auto process(const Array & input, const size_t index) -> void = 0;
  virtual auto terminates(const Array & input, const size_t index) -> bool = 0;

  void operator () (Array & input, const int index = 0) {
    if (terminates(input, index)) {
      process(input, index);
    }

    if (finished_) {
      return;
    }

    if (K >= index) {
      Array candidates;
      size_t number_of_candidates = 0;
      generate_candidates(input, index, candidates, number_of_candidates);
      if (0 < number_of_candidates) {
        for (size_t k = 0; k < number_of_candidates; ++k) {
          // open for some hook here
          input[index] = candidates[k];
          operator () (input, 1 + index);
          if (finished_) {
            return;
          }
          // open for some other hook here
        }
      }
    }
  }

  void operator () () {
    Array input;
    operator()(input);
  }

protected:
  void finish() {
    finished_ = true;
  }

private:
  bool finished_ = false;
};

/*
 * Generates a factorial : K!
 */
template<int K, int START = 1>
struct NumericPermutations : Backtracker<int, K> {
  using Array = typename Backtracker<int, K>::Array;

  auto generate_candidates(const Array & input, const size_t input_size, Array & candidates, size_t & number_of_candidates) -> void override {
    assert(0 == number_of_candidates);
    if (K == input_size) { return; }
    Array possible_candidates{ 0, };
    for (size_t i = 0; input_size > i; ++i) {
      possible_candidates[input[i] - START] = 1;
    }
    for (size_t i = 0; K > i; ++i) {
      if (1 > possible_candidates[i]) {
        candidates[number_of_candidates++] = START + i;
      }
    }
    assert(K >= number_of_candidates);
  }

  auto process(const Array & input, const std::size_t index) -> void override {
    std::cout << "{";
    for (const auto & item : input) {
      std::cout << " " << item;
    }
    std::cout << " }" << std::endl;
  }

  auto terminates(const Array & input, const std::size_t index) -> bool override {
    return K == index;
  }
};

/*
 * Subsets
 */
template<int K, int START = 1>
struct NumericSubsets : Backtracker<int, K> {
  using Array = typename Backtracker<int, K>::Array;

  auto generate_candidates(const Array & input, const size_t input_size, Array & candidates, size_t & number_of_candidates) -> void override {
    assert(0 == number_of_candidates);
    if (K > input_size) {
      candidates = { 1, 0, };
      number_of_candidates = 2;
    }
  }

  auto process(const Array & input, const std::size_t index) -> void override {
    std::cout << "(";
    for (int i = 0; K > i; ++i) {
      if (0 < input[i]) {
        std::cout << " " << START + i;
      }
    }
    std::cout << " )" << std::endl;
  }

  auto terminates(const Array & input, const std::size_t index) -> bool override {
    return K == index;
  }
};

int main() {
  std::cout << "Numeric Permutations 14-16" << std::endl;
  NumericPermutations<3, 14> permutations;
  permutations();
  std::cout << "-------" << std::endl << std::endl;

  std::cout << "Numeric Subsets 9-12" << std::endl;
  NumericSubsets<4, 9> subsets;
  subsets();
  std::cout << "-------" << std::endl << std::endl;

  return 0;
}
