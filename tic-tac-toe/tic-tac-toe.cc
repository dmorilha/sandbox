/* Copyright (c) 2026 Daniel Morilha */

#include <iostream>

#include <array>
#include <memory>
#include <vector>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <cassert>

#include <termios.h>
#include <unistd.h>

enum State : char {
  EMPTY,
  X,
  O,
};

State winner(State * state) {
  // logic to tell which player wins

  // horizontal
  /*
   * X X X
   * ? ? ?
   * ? ? ?
   */
  if (state[1] != EMPTY && state[1] == state[2] && state[2] == state[3])
    return state[1];

  /*
   * ? ? ?
   * X X X
   * ? ? ?
   */
  if (state[4] != EMPTY && state[4] == state[5] && state[5] == state[6])
    return state[4];

  /*
   * ? ? ?
   * ? ? ?
   * X X X
   */
  if (state[7] != EMPTY && state[7] == state[8] && state[8] == state[9])
    return state[7];

  // vertical
  /*
   * X ? ?
   * X ? ?
   * X ? ?
   */
  if (state[1] != EMPTY && state[1] == state[4] && state[4] == state[7])
    return state[1];

  /*
   * ? X ?
   * ? X ?
   * ? X ?
   */
  if (state[2] != EMPTY && state[2] == state[5] && state[5] == state[8])
    return state[2];

  /*
   * ? ? X
   * ? ? X
   * ? ? X
   */
  if (state[3] != EMPTY && state[3] == state[6] && state[6] == state[9])
    return state[3];

  // diagonal
  /*
   * X ? ?
   * ? X ?
   * ? ? X
   */
  if (state[1] != EMPTY && state[1] == state[5] && state[5] == state[9])
    return state[1];

  /*
   * ? ? X
   * ? X ?
   * X ? ?
   */
  if (state[3] != EMPTY && state[3] == state[5] && state[5] == state[7])
    return state[3];

  return EMPTY;
}

void print_table(State * state) {
  for (int i = 1; i < 10; ++i) {
    switch (state[i]) {
      case EMPTY:
        printf("%d ", i);
        break;
      case X:
        printf("X ");
        break;
      case O:
        printf("O ");
        break;
    }
    if (0 == i % 3)
      printf("\n\r");
  }
}

namespace ai {
struct Node {
  // 0 index array to save one pointer per node, ugly but better.
  std::array<Node *, 9> descendants = { nullptr };
  int64_t x = 0, o = 0;
};
Node root;
void build_tree(Node *node = &root) {
  static State state[10] = { EMPTY };
  std::vector<char> empties;
  for (int i = 9; i > 0; --i) {
    if (EMPTY == state[i]) {
      empties.push_back(i);
    }
  }
  const int degree = empties.size();
  const State turn = degree % 2 ? X : O;

  int points = 1;
  while (!empties.empty()) {
    state[empties.back()] = turn;
    switch (winner(state)) {
      case EMPTY:
        {
          Node * descendant = new Node();
          build_tree(descendant);
          node->x += descendant->x;
          node->o += descendant->o;
          node->descendants[empties.back() - 1] = descendant;
        }
        break;

      /* terminals */
      case X:
        node->x += points;
        break;

      case O:
        node->x -= points;
        break;
    }
    state[empties.back()] = EMPTY;
    empties.pop_back();
  }
}


void print_node(Node *node) {
  printf("points: %li\n\r", node->x);
  printf(" ---\n\r");
}

void print_descendants(Node *node) {
  printf("printing descendants...\n\r");
  for (int i = 0; i < node->descendants.size(); ++i)
    if (nullptr != node->descendants[i]) {
      printf("node %d\n\r", i + 1);
      print_node(node->descendants[i]);
    }
}

char next_move(State * state, const std::vector<char> & history, Node * node = &root) {
  State turn = X;
  for (auto & item : history) {
    assert(nullptr != node->descendants[item - 1]);
    node = node->descendants[item - 1];
    if (X == turn) {
      turn = O;
    } else {
      turn = X;
    }
  }
  print_node(node);
  print_descendants(node);
  int best_move = 0;
  int64_t max = 0;

  for (int i = 0; i < 9; ++i) {
    if (state[i + 1] != EMPTY)
      continue;
    state[i + 1] = turn;
    if (turn == winner(state)) {
      return i + 1;
    }
    state[i + 1] = EMPTY;
  }

  for (int i = 0; i < 9; ++i) {
    if (state[i + 1] != EMPTY)
      continue;

    const auto opponent = X == turn ? O : X;
    state[i + 1] = X == turn ? O : X;
    if (opponent == winner(state)) {
      return i + 1;
    }
    state[i + 1] = EMPTY;

    if (nullptr != node->descendants[i]) {
      switch (turn) {
      case X:
        if (max < node->descendants[i]->x) {
          max = node->descendants[i]->x;
          best_move = i + 1;
        } else if (max == node->descendants[i]->x && 0 < rand() % 2) {
          max = node->descendants[i]->x;
          best_move = i + 1;
        }
        break;
      case O:
        if (0 == max || max > node->descendants[i]->x) {
          max = node->descendants[i]->x;
          best_move = i + 1;
        } else if (max == node->descendants[i]->x && 0 < rand() % 2) {
          max = node->descendants[i]->x;
          best_move = i + 1;
        }
        break;
      }
    }
  }
  return best_move;
}
} // end of ai namespace

// plays next round randomly
char play(State * state, State player) {
  char i = (rand() % 9) + 1;
  while (EMPTY != state[i])
    i = (rand() % 9) + 1;
  state[i] = player;
  return i;
}

int main() {
  std::vector<char> history;

  ai::build_tree();

  State state[10] = { EMPTY }; // initialize table
  srand(arc4random()); // seeds the random number generator
  printf("GAME ON!\n");

  // makes UNIX terminal raw, ignores carriage return and new line for input, does not echo input
  struct termios terminal, new_terminal;
  tcgetattr(STDIN_FILENO, &terminal);
  memcpy(&new_terminal, &terminal, sizeof(struct termios));
  cfmakeraw(&new_terminal);
  tcsetattr(STDIN_FILENO, 0, &new_terminal);

  State turn = X;
  print_table(state);
  // the game lasts until all 9 spaces are filled or a player has won
  while (9 > history.size() && winner(state) == EMPTY) {
    bool should_turn = false;
    const int input = getchar();

    if ('q' /* quits */ == input)
      break;

    switch (input) {
    case 'p' /* plays */ :
      history.push_back(play(state, turn));
      should_turn = true;
      break;

    case 'a' /* ai */ :
      {
        const char next_move = ai::next_move(state, history);
        state[next_move] = turn;
        history.push_back(next_move);
        should_turn = true;
      }
      break;

    // player chooses from 1 to 9
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if (EMPTY == state[input - '0']) {
        history.push_back(input - '0');
        should_turn = true;
        state[input - '0'] = turn;
      }
      break;
    default:
      // inshould_turn input
      break;
    }

    print_table(state);

    if (should_turn) {
      if (X == turn)
        turn = O;
      else
        turn = X;
    }
  }

  // resets terminal back to normal
  tcsetattr(STDIN_FILENO, 0, &terminal);

  switch (winner(state)) {
  case EMPTY:
    printf("Drawn\n");
    break;
  case X:
    printf("X won!\n");
    break;
  case O:
    printf("O won!\n");
    break;
  }

  printf("history of moves\n");
  for (auto item : history)
    printf("%d - ", item);
  printf("\n");
  return 0;
}
