/* When it comes to collision, a hash map can have two strategies:
 *  - bucketing, when two elements hash to the same bucket, they are linked together.
 *  - open addressing, when two elements share the same hash,
 *    the second element is placed on the next available entry.
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <cassert>
#include <cstdint>

template<class TYPE>
struct HashFunction {
  uint64_t operator() (const TYPE &) const;
};

/*
 * Hashirilha
 * ----------
 * This pseudo hash function xors each character from
 * a string into the result while rotating the previously
 * acquired bytes to the left.
 *
 * The current implementation is weak in terms of distribution.
 */
template<>
struct HashFunction<std::string> {
uint64_t operator() (const std::string & s) const {
  uint64_t hash = 0;
  for (const uint8_t c : s) {
    /* keep rotating */
    hash = (hash << 8) | ((hash & 0xff00000000000000) >> 56);
    hash ^= c;
  }
  return hash;
}
};

template<class TYPE>
struct CompareFunction {
  bool operator() (const TYPE &, const TYPE &) const;
};

template<>
struct CompareFunction<std::string> {
  bool operator() (const std::string & a, const std::string & b) const {
    return a == b;
  }
};

template<class TYPE>
struct HashMap {
  static const TYPE NONE;

  virtual const TYPE & get(const TYPE &) const = 0;
  virtual void insert(TYPE &&) = 0;

protected:
  HashFunction<TYPE> hash_{};
  CompareFunction<TYPE> compare_{};
};

template<class TYPE> const TYPE HashMap<TYPE>::NONE{};

template<class TYPE>
struct BucketMap : public HashMap<TYPE> {
  using Base = HashMap<TYPE>;

  struct Node;
  struct Node {
    Node * next = nullptr;
    TYPE value;
    Node(TYPE && v) : value(std::move(v)) { };
  };

  std::vector<Node *> buckets_;

  BucketMap(const std::size_t size = 7) : buckets_{size} { }

  const TYPE & get(const TYPE & item) const override {
    const auto hash = Base::hash_(item);
    const std::size_t index = hash % buckets_.size();
    std::cerr << __func__ << "(\"" << item << "\") index -> " << index << std::endl;
    const Node * node = buckets_[index];
    for (; nullptr != node; node = node->next) {
      if (Base::compare_(node->value, item)) {
        return node->value;
      }
    }
    return Base::NONE;
  }

  void insert(TYPE && item) override {
    const auto hash = Base::hash_(item);
    const std::size_t index = hash % buckets_.size();
    std::cerr << __func__ << "(\"" << item << "\") index -> " << index << std::endl;
    Node * & entry = buckets_[index];
    if (nullptr == entry) {
      entry = new Node(std::move(item));
    } else {
      Node * node = entry;
      while (nullptr != node->next) node = node->next;
      node->next = new Node(std::move(item));
    }
  }
};

template<class TYPE>
struct OpenAddressMap : public HashMap<TYPE> {
  using Base = HashMap<TYPE>;
  std::vector<std::unique_ptr<TYPE>> data_;

  OpenAddressMap(const std::size_t size = 23) : data_{size} { }

  const TYPE & get(const TYPE & item) const override {
    const auto hash = Base::hash_(item);
    const std::size_t position = hash % data_.size();
    std::cerr << __func__ << "(\"" << item << "\") position -> " << position << std::endl;
    if (static_cast<bool>(data_[position])
        && Base::compare_(*data_[position], item)) {
      return *data_[position];
    } else {
      for (std::size_t index = (1 + position) % data_.size();
          position != index; index = (1 + index) % data_.size()) {
        if (static_cast<bool>(data_[index])
            && Base::compare_(*data_[index], item)) {
          return *data_[index];
        }
      }
    }
    return Base::NONE;
  }

  void insert(TYPE && item) override {
    const auto hash = Base::hash_(item);
    const std::size_t position = hash % data_.size();
    std::cerr << __func__ << "(\"" << item << "\") position -> " << position << std::endl;
    std::size_t index = position;
    if (static_cast<bool>(data_[index])) {
      for (index = (1 + index) % data_.size(); position != index;) {
        if ( ! static_cast<bool>(data_[index])) {
          break;
        }
        index = (1 + index) % data_.size();
      }
      if (position == index) {
        /*
         * Alternatively, the data container should grow, and
         * all existing items should be re-hashed into new positions.
         */
        throw std::runtime_error("map is full");
      }
    }
    data_[index] = std::make_unique<TYPE>(std::move(item));
  }
};

void test(HashMap<std::string> & hash_map) {
  std::cerr << "---------------" << std::endl
    << "Insertion" << std::endl
    << "---------------" << std::endl;
  hash_map.insert(std::string{"When you, when you forget your name."});
  hash_map.insert(std::string{"When old faces all look the same."});
  hash_map.insert(std::string{"Meet me in the morning when you wake up."});
  hash_map.insert(std::string{"Meet me in the morning then you wake up."});
  hash_map.insert(std::string{"If only I don't bend and break,"});
  hash_map.insert(std::string{"I'll meet you on the other side,"});
  hash_map.insert(std::string{"I'll meet you in the light."});
  hash_map.insert(std::string{"If only I don't suffocate,"});
  hash_map.insert(std::string{"I will meet you in the morning when you wake."});

  std::cerr << "---------------" << std::endl
    << "Retrieval" << std::endl
    << "---------------" << std::endl;

  {
    const auto & item =  hash_map.get(std::string{"Meet me in the morning when you wake up."});
    if (HashMap<std::string>::NONE == item) {
      std::cerr << "item was not found!" << std::endl;
    } else {
      std::cout << "item was found: " << item << std::endl;
    }
    std::cerr << std::endl;
  }

  {
    const auto & item =  hash_map.get(std::string{"I'll meet you in the light."});
    if (HashMap<std::string>::NONE == item) {
      std::cerr << "item was not found!" << std::endl;
    } else {
      std::cout << "item was found: " << item << std::endl;
    }
    std::cerr << std::endl;
  }

  {
    const auto & item =  hash_map.get(std::string{"Waiting for life to start."});
    if (HashMap<std::string>::NONE == item) {
      std::cerr << "item was not found!" << std::endl;
    } else {
      std::cout << "item was found: " << item << std::endl;
    }
    std::cerr << std::endl;
  }
}

int main() {
  {
    using MY_BUCKET_MAP = BucketMap<std::string>;
    MY_BUCKET_MAP map;
    std::cerr << "Bucket Hash Map" << std::endl;
    test(map);
  }

  {
    using MY_OPEN_ADDRESS_MAP = OpenAddressMap<std::string>;
    MY_OPEN_ADDRESS_MAP map;
    std::cerr << "Open Addressing Hash Map" << std::endl;
    test(map);
  }

  return 0;
}
