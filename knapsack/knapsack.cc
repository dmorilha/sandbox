/* knapsack from Design Algorithms */

#include <algorithm>
#include <iostream>
#include <map>
#include <tuple>
#include <vector>

#include <cassert>

struct Item {
  std::size_t cost;
  std::size_t value;
  bool operator < (const Item & other) const { return cost < other.cost; }
};

typedef std::vector<Item> Items;

Items knapsack(Items input, const int max_cost) {
  std::map<int, std::map<int, std::vector<std::pair</* index */ int, /* total value */ int>>>> map;
  std::sort(input.begin(), input.end());

  struct {
    int value = 0;
    int round = 0;
    int cost = 0;
  } greatest;

  // first iteration, sets of only one element.
  for (int i = 0; input.size() > i; ++i) {
    const int total_cost = input[i].cost;
    if (max_cost >= total_cost) {
      map[total_cost][1].push_back({i, input[i].value});
      if (greatest.value < input[i].value) {
        greatest.value = input[i].value;
        greatest.cost = total_cost;
        greatest.round = 1;
      }
    }
  }

  // rounds are subsets of two or more elements.
  for (int round = 1; input.size() > round; ++round) {
    for (auto & item : map) {
      const int total_cost = item.first;
      auto iterator = item.second.find(round);
      if (item.second.end() != iterator) {
        for (int i = 0; i < iterator->second.size(); ++i) {
          const int last_index = iterator->second[i].first;
          for (int j = last_index + 1; j < input.size(); ++j) {
            const int new_total_cost = total_cost + input[j].cost;
            if (max_cost >= new_total_cost) {
              const int new_total_value = iterator->second[i].second + input[j].value;
              map[new_total_cost][round + 1].push_back({j, new_total_value});
              if (greatest.value < new_total_value) {
                greatest.value = new_total_value;
                greatest.cost = new_total_cost;
                greatest.round = round + 1;
              }
            }
          }
        }
      }
    }
  }

#if 0
  /* dumps the built map */
  for (const auto & item : map) {
    for (const auto & item2 : item.second) {
      for (const auto & item3 : item2.second) {
        std::cout << item.first << ", " << item2.first << ", "
          << item3.first << ", " << item3.second << std::endl;
      }
    }
  }
#endif

  std::vector<int> indexes;
  indexes.reserve(greatest.round);
  for (; 0 < greatest.round; --greatest.round) {
    int index = -1, value = 0;
    const auto & items = map[greatest.cost][greatest.round];
    assert(!items.empty());
    for (int i = items.size() - 1; 0 <= i; --i) {
      if (indexes.empty() || items[i].first < indexes.back()) {
        if (value <= items[i].second) {
          value = items[i].second;
          index = items[i].first;
        }
      }
    }
    assert(0 <= index);
    indexes.push_back(index);
    greatest.cost -= input[index].cost;
  }

  Items result;
  result.reserve(indexes.size());
  for (const int index : indexes) {
    result.push_back(input[index]);
  }

  return result;
}

int main(int argc, char * * argv) {
  if (1 < argc) {
    Items items {{1, 1}, {2, 100}, {3, 9}, {4, 16}, {5, 25},};

    {
      std::cout << "Input: ";
      for (const auto & item : items) {
        std::cout << "(" << item.cost << ", " << item.value << "), ";
      }
      std::cout << std::endl;
    }

    const int max_cost = std::atoi(argv[1]);
    if (0 < max_cost) {
      Items result = knapsack(items, max_cost);
      {
        std::cout << "Kanpsack with max cost " << max_cost << ": ";
        for (const auto & item : result) {
          std::cout << "(" << item.cost << ", " << item.value << "), ";
        }
        std::cout << std::endl;
      }
    } else {
      std::cerr << "ERROR. usage: \"" << argv[0] << "\" followed by a non-negative integer representing max cost." << std::endl;
    }
  }
  return 0;
}
