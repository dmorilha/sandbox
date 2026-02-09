#include <cmath>
#include <iostream>

template <typename T>
class Mergesort {
public:
  void operator()(T * array, const std::size_t size) {
    aux = new T[size];
    mergesort(array, size);
    delete [] aux;
    aux = nullptr;
  }
private:
  void mergesort(T * array, const std::size_t size) {
    if (2 < size) {
      const auto half = size / 2; // this is always an integer division 5 / 2 = 2.
      mergesort(array, half);
      mergesort(array + half, size - half);
      for (int i = 0, j = 0, k = half; size > i; ++i) {
        if (half > j && (size == k || array[j] < array[k])) {
          aux[i] = array[j];
          ++j;
        } else {
          aux[i] = array[k];
          ++k;
        }
      }
      std::copy(aux, aux + size, array);
    } else if (2 == size) {
      if (array[0] > array[1]) {
        std::swap(array[0], array[1]);
      }
    }
  }
  T * aux = nullptr;
};

int main(int argc, char * * argv) {
  if (1 < argc) {
    const std::size_t size = std::atoi(argv[1]);
    auto array = new int[size];
    srand(time(nullptr));
    for (int i = 0; size > i; ++i) {
      array[i] = rand() % 101;
    }
    Mergesort<int>()(array, size);
    for (int i = 0; size > i; ++i) {
      std::cout << array[i] << ", ";
    }
    std::cout << std::endl;
    delete [] array;
    array = nullptr;
  } else {
    std::cerr << "usage: " << argv[0] << " size of the array.";
  }
  return 0;
}
