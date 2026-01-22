#include <iostream>
#include <vector>

template<typename T>
struct PriorityHeap {
  using size = std::size_t;
  std::vector<T> container_;

  template<class ... ARGS>
  void push(ARGS && ... args) {
    container_.emplace_back(std::forward<ARGS>(args)...);
    size i = container_.size() - 1;
    while (0 < i) {
      const size half = i / 2;
      if (container_[half] > container_[i]) {
        std::swap(container_[half], container_[i]);
        i = half;
      } else {
        return;
      }
    }
  }

  T pop() {
    T result{std::move(container_[0])};
    size parent = 1, i = 2;
    while (container_.size() >= i) {
      if (container_.size() > i && container_[i - 1] > container_[i]) {
        i = 1 + i;
      }
      std::swap(container_[parent - 1], container_[i - 1]);
      parent = i;
      i *= 2;
    }
    std::swap(container_[parent - 1], container_[container_.size() - 1]);
    container_.pop_back();
    return result;
  }
};

int main() {
  PriorityHeap<int> priority_heap;
  for (int i = 9; 0 <= i; --i) {
    priority_heap.push(i);
  }
  std::cout << "A pop: " << priority_heap.pop() << std::endl;
  std::cout << "Another pop: " << priority_heap.pop() << std::endl;
  std::cout << "Yet another pop: " << priority_heap.pop() << std::endl;
  std::cout << "I am getting tired, pop it: " << priority_heap.pop() << std::endl;
  std::cout << "What do you have for me? " << priority_heap.pop() << std::endl;
  return 0;
}
