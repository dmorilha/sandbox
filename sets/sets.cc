#include <algorithm>
#include <iostream>
#include <vector>

#include <cassert>

template<typename T>
class Set {
  using Self = Set<T>;
  using Size = std::size_t;
  using Vector = std::vector<T>;

  bool is_sorted_ = true;
  Vector vector_{};
public:
  template<typename ... ARGS>
  Self & push(ARGS && ... args) {
    vector_.emplace_back(std::forward<ARGS>(args)...);
    const Size size = vector_.size();
    if (1 < size && is_sorted_) {
      is_sorted_ = vector_[size - 2] <= vector_[size - 1];
    }
    return *this;
  }

  /* Unfortunately `union` is a reserved word in c++ */
  Self Union(const Self & other) {
    Self result;
    if ( ! is_sorted_) {
      std::sort(vector_.begin(), vector_.end());
      is_sorted_ = true;
    }
    Vector copy;
    if ( ! other.is_sorted_) {
      std::copy(other.vector_.begin(), other.vector_.end(), std::back_inserter(copy));
      std::sort(copy.begin(), copy.end());
    }
    const Vector & o = other.is_sorted_ ? other.vector_ : copy;
    for (Size i = 0, j = 0; vector_.size() > i && o.size() > j;) {
      if (vector_[i] < o[j]) {
        ++i;
      } else if (vector_[i] > o[j]) {
        ++j;
      } else {
        result.vector_.push_back(vector_[i]);
        ++i;
        ++j;
      }
    }
    result.is_sorted_ = true;
    return result;
  }

  Self intersection(const Self & other) {
    Self result;
    if ( ! is_sorted_) {
      std::sort(vector_.begin(), vector_.end());
      is_sorted_ = true;
    }
    Vector copy;
    if ( ! other.is_sorted_) {
      std::copy(other.vector_.begin(), other.vector_.end(), std::back_inserter(copy));
      std::sort(copy.begin(), copy.end());
    }
    const Vector & o = other.is_sorted_ ? other.vector_ : copy;
    Size i = 0, j = 0;
    while (vector_.size() > i && o.size() > j) {
      if (vector_[i] < o[j]) {
        result.vector_.push_back(vector_[i]);
        ++i;
      } else if (vector_[i] > o[j]) {
        result.vector_.push_back(o[j]);
        ++j;
      } else {
        ++i;
        ++j;
      }
    }
    for (; vector_.size() > i; ++i) {
      result.vector_.push_back(vector_[i]);
    }
    for (; o.size() > j; ++j) {
      result.vector_.push_back(o[j]);
    }
    result.is_sorted_ = true;
    return result;
  }

  friend std::ostream & operator << (std::ostream &, const Set<T> &);
};

std::ostream & operator << (std::ostream & o, const Set<std::size_t> & set) {
  for (const int item : set.vector_) {
    o << item << ", ";
  }
  return o;
}

int main() {
  using MySet = Set<std::size_t>;
  MySet set_a, set_b;
  std::cout << "set A is " << set_a.push(1).push(2).push(3).push(5).push(4) << std::endl;
  std::cout << "set B is " << set_b.push(2).push(4).push(6) << std::endl;


  {
    MySet union_set = set_a.Union(set_b);
    std::cout << "A U B is " << union_set << std::endl;
  }

  {
    MySet union_set = set_a.intersection(set_b);
    std::cout << "A ∩ B is " << union_set << std::endl;
  }
  return 0;
}
