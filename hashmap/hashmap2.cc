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

template<class KEY, class VALUE>
struct HashMap {
  static const VALUE NONE;
  virtual const VALUE & get(const KEY &) const = 0;
  virtual void insert(KEY &&, VALUE &&) = 0;
  virtual VALUE & operator[](KEY &&) = 0;

protected:
  HashFunction<KEY> hash_{};
  CompareFunction<KEY> compare_{};
};

template<class KEY, class VALUE> const VALUE HashMap<KEY, VALUE>::NONE{};

template<class KEY, class VALUE>
struct BucketMap : public HashMap<KEY, VALUE> {
  using Base = HashMap<KEY, VALUE>;

  struct Node;
  struct Node {
    Node * next = nullptr;
    KEY key;
    VALUE value;
    ~Node() { if (nullptr == next) { delete next; next = nullptr; } }
    template<class K> Node(K && k, VALUE && v) : key(std::forward<K>(k)),  value(std::move(v)) { };
    template<class K> Node(K && k) : key(std::forward<K>(k)) { };
  };

  std::vector<Node *> buckets_;

  ~BucketMap() {
    for (auto & entry : buckets_) {
      if (nullptr != entry) {
        delete entry;
        entry = nullptr;
      }
    }
  }

  BucketMap(const std::size_t size = 7) : buckets_{size} { }

  const VALUE & get(const KEY & key) const override {
    const auto hash = Base::hash_(key);
    const std::size_t index = hash % buckets_.size();
    const Node * node = buckets_[index];
    for (; nullptr != node; node = node->next) {
      if (Base::compare_(node->key, key)) {
        return node->value;
      }
    }
    return Base::NONE;
  }

  void insert(KEY && key, VALUE && value) override {
    const auto hash = Base::hash_(key);
    const std::size_t index = hash % buckets_.size();
    Node * & entry = buckets_[index];
    if (nullptr == entry) {
      entry = new Node(std::move(key), std::move(value));
    } else if (Base::compare_(entry->key, key)) {
        entry->value = std::move(value);
    } else {
      Node * node = entry;
      while (nullptr != node->next) {
        if (Base::compare_(node->key, key)) {
          node->value = std::move(value);
          return;
        }
        node = node->next;
      }
      node->next = new Node(std::move(key), std::move(value));
    }
  }

  VALUE & operator[](KEY && key) override {
    const auto hash = Base::hash_(key);
    const std::size_t index = hash % buckets_.size();
    Node * & entry = buckets_[index];
    if (nullptr == entry) {
      entry = new Node(key);
      return entry->value;
    } else if (Base::compare_(entry->key, key)) {
        return entry->value;
    } else {
      Node * last = nullptr, * node = entry;
      while (nullptr != node) {
        if (Base::compare_(node->key, key)) {
          return node->value;
        }
        last = node;
        node = node->next;
      }
      assert(nullptr != last);
      last->next = node = new Node(key);
      return node->value;
    }
  }
};

namespace {
struct Record {
  std::size_t age;
  std::size_t telephone;
  std::string address;
  std::string name;
  friend std::ostream & operator << (std::ostream &, const Record &);
};

std::ostream & operator << (std::ostream & o, const Record & r) {
  o << "name: " << r.name << ", address: " << r.address << ", age: " << r.age << ", telephone: " << r.telephone;
  return o;
}
} // end of anonymous namespace

int main() {
  using MY_BUCKET_MAP = BucketMap<std::string, Record>;

  MY_BUCKET_MAP contacts;
  contacts["Daniel Augusto"].name = "Daniel Augusto";
  contacts["Daniel Augusto"].age = 28;
  contacts["Daniel Augusto"].telephone = 55'119'683'872'122;
  contacts["Daniel Augusto"].address = "São Paulo";

  contacts["Ana Carolina"].name = "Ana Carolina";
  contacts["Ana Carolina"].age = 32;
  contacts["Ana Carolina"].telephone = 55'359'955'333'132;
  contacts["Ana Carolina"].address = "Belo Horizonte";
  
  contacts["Artur Nogueira"].name = "Artur Nogueira";
  contacts["Artur Nogueira"].age = 44;
  contacts["Artur Nogueira"].telephone = 5'523'944'987'462;
  contacts["Artur Nogueira"].address = "Salvador";

  std::cout << contacts["Daniel Augusto"] << std::endl
    << contacts["Ana Carolina"] << std::endl
    << contacts["Artur Nogueira"] << std::endl;

  return 0;
}
