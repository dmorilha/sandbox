#include <cmath>
#include <iostream>

template<typename T>
class Quicksort {
public:
  void operator()(T * t, std::size_t size) {
    /* the first iteration picks pivot as the very first element. */
    quicksort(t, 0, size - 1);
  }

private:
  void quicksort(T * a, int low, int high) {
    if (low >= high) {
      return;
    }
    const int pivot = partition(a, low, high);
    quicksort(a, low, pivot);
    quicksort(a, pivot + 1, high);
  }

  /* hoare's partition scheme */
  int partition(T * a, int low, int high) {
    const T pivot = a[low];
    int i = low - 1;
    int j = high + 1;
    while (true) {
      do {
        i++;
      } while(a[i] < pivot);
      do {
        j--;
      } while(a[j] > pivot);
      if (i >= j) {
        return j;
      }
      std::swap(a[i], a[j]);
    }
  }
};

int main(int argc, char * * argv) {
  if (1 < argc) {
    // get size from command line argument.
    const size_t size = std::atoi(argv[1]);

    // initialize array with random elements.
    auto array = new int[size];
    srand(time(nullptr));
    for (int i = 0; size > i; ++i) {
      array[i] = rand() % 101;
    }
    
    {
      /**
       * instantiate and run quick sort as a temporary
       * instance of the Quicksort Functor class
       */
      Quicksort<int>()(array, size);
    }

    // print the results.
    for (int i = 0; i < size; ++i) {
      std::cout << array[i] << ", ";
    }
    std::cout << std::endl;

    // delete the array.
    delete [] array;
  } else {
    std::cerr << "usage: " << argv[0] << " (size of array)" << std::endl;
  }
  return 0;
}
