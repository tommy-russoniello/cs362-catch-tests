#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

/************************************************************/
// System includes

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/************************************************************/
// Local includes

#include "SearchTree.hpp"

/************************************************************/
// Using declarations

using std::abs;
using std::advance;
using std::cerr;
using std::cout;
using std::equal;
using std::fixed;
using std::mt19937;
using std::ostringstream;
using std::pair;
using std::remove;
using std::set;
using std::setprecision;
using std::string;
using std::to_string;
using std::vector;

/************************************************************/
// Function prototypes

template<typename T> void REQUIRE_TREE (const SearchTree<T>& t, string treeString);
template<typename T> void REQUIRE_TREE (const SearchTree<T>& t, string treeString, int depth);
template<typename T> void REQUIRE_HEAD (const SearchTree<T>& tree, T left, T right, T parent);
template<typename T> void REQUIRE_HEAD_NULL (const SearchTree<T>& tree);
template<typename T> void REQUIRE_LEVEL_ORDER (const SearchTree<T>& tree, string answer);
template<typename T> void tree_insert (SearchTree<T>& tree, const vector<T>& values);
template<typename T> void tree_erase (SearchTree<T>& tree, const vector<T>& values);
template<typename T> void REQUIRE_ALL_EQUAL (const vector<T>& values);
void CHECK_AND_WARN (bool condition, string message);
ostringstream CHECK_AND_WARN (bool condition, string message, ostringstream stream);

/************************************************************/
// Global variables

// Messages from failed tests to be outputted and end.
vector<string> FAILED_TESTS;
// Total points lost from all tests.
double LOST_SCORE;
// Total points scored. Separately calculated as checksum.
double GAINED_SCORE;
// Total point value of all tests that actually ran;
double TESTED_SCORE;
// Total points that these tests are worth.
double POINTS;

/************************************************************/
// Catch listeners

struct ScoringListener : Catch::TestEventListenerBase {
  using TestEventListenerBase::TestEventListenerBase;

  virtual void sectionEnded(Catch::SectionStats const& sectionStats) override {
    string point_value_string = sectionStats.sectionInfo.description;

    if (point_value_string.front() == '[' && point_value_string.back() == ']')
    {
      double point_value_num;

      try
      {
        // Point value is multiplied by total total points for the tests as a precentage.
        point_value_num = (stod (point_value_string.substr (1, 4)) / 100.0) * POINTS;

        if (sectionStats.assertions.total() > 0)
        {
          if (!sectionStats.assertions.allPassed())
          {
            ostringstream stream;
            LOST_SCORE += point_value_num;
            stream << "- " <<  fixed << setprecision (2) << point_value_num << " points:   "
                   << sectionStats.sectionInfo.name;
            FAILED_TESTS.push_back (stream.str ());
          }
          else
          {
            GAINED_SCORE += point_value_num;
          }

          TESTED_SCORE += point_value_num;
        }
      }
      catch (std::invalid_argument& e)
      {
        cerr << e.what ();
        GAINED_SCORE = std::numeric_limits<double>::max();
      }
    }
  }
};

CATCH_REGISTER_LISTENER(ScoringListener)

/************************************************************/

int
main(int argc, char* argv[]) {
  // Set this to total point value of these tests.
  POINTS = 90;
  TESTED_SCORE = GAINED_SCORE = LOST_SCORE = 0.0;

  int result = Catch::Session().run(argc, argv);

  cout << fixed << setprecision (2);

  // Print all failed test error messages.
  for (string str : FAILED_TESTS)
  {
    cout << str << "\n";
  }

  double FINAL_SCORE = TESTED_SCORE - LOST_SCORE;

  // Don't print with pretty colors if they're using Windows.
  #ifdef _WIN32

  cout << "\nSCORE: " << FINAL_SCORE << " / " << POINTS << "   >>>   "
       << (FINAL_SCORE / POINTS) * 100.0 << " %\n\nYour output could have pretty colors if "
       << "you weren't on Windows.\n\n";

   // Check that calculated score hasn't been comprimised.
   if (abs ((POINTS - LOST_SCORE) - GAINED_SCORE) > 0.1)
   {
     cout << "Not all tests were run or an error occurred.\n\n";
   }

  #else

  // Bold
  cout << "\033[1m";
  cout << "\nSCORE: " << FINAL_SCORE << " / " << POINTS << "   >>>   ";
  if (FINAL_SCORE >= 80.0)
  {
    // Green
    cout << "\033[32m";
  }
  else if (FINAL_SCORE >= 60)
  {
    // Yellow
    cout << "\033[33m";
  }
  else
  {
    // Red
    cout << "\033[31m";
  }

  cout << (FINAL_SCORE / POINTS) * 100.0 << " %\n\n";
  // Reset
  cout << "\033[0m";

  // Check that calculated score hasn't been comprimised.
  if (abs ((POINTS - LOST_SCORE) - GAINED_SCORE) > 0.1)
  {
    // Red
    cout << "\033[31m";
    cout << "Not all tests were run or an error occurred.\n\n";
    // Reset
    cout << "\033[0m";
  }

  #endif

  return result;
}

/************************************************************/
// Tests

// not currently part of assignment
TEST_CASE ("default constructor", "0")
{
  SearchTree<int> tree;

  SECTION("default constructor - tree", "0")
  {
    REQUIRE_TREE (tree, "[ ]", -1);
  }

  // Dependencies: size
  SECTION("default constructor - size", "0")
  {
    REQUIRE (tree.size() == 0);
  }

  // Dependencies: end
  SECTION ("default constructor - head", "0")
  {
    REQUIRE_HEAD_NULL (tree);
  }
}

/*----------------------------------------------------------*/

// Dependencies: insert
TEST_CASE ("copy constructor", "7")
{
  SearchTree<int> tree_a;

  tree_insert (tree_a, { 1, 2, 3, 4 });

  SearchTree<int> tree_b (tree_a);

  SECTION ("copy constructor - state of tree after copy construct", "[3.00]")
  {
    REQUIRE_TREE (tree_b, "[ 1 2 3 4 ]", 3);
  }

  // Dependencies: size
  SECTION ("copy constructor - size of tree after copy construct", "[0.20]")
  {
    REQUIRE (tree_b.size() == 4);
  }

  // Dependencies: iterator decrement, end
  SECTION ("copy constructor - head links after copy construct", "[0.80]")
  {
    REQUIRE_HEAD (tree_b, 1, 4, 1);
  }

  // Dependencies: clear
  SECTION ("copy constructor - separate from orginal", "[3.00]")
  {
    tree_b.clear();
    tree_b.insert(5);

    ostringstream stream_a, stream_b;

    tree_a.printInOrder(stream_a);
    tree_b.printInOrder(stream_b);

    REQUIRE (stream_a.str () != stream_b.str ());
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("empty", "1")
{
  SearchTree<int> tree;

  SECTION ("empty - on empty tree", "[0.25]")
  {
    REQUIRE (tree.empty());
  }

  // Dependencies: insert
  SECTION ("empty - on non-empty tree", "[0.75]")
  {
    tree_insert(tree, { 4, 2, 7, 6 });

    REQUIRE_FALSE (tree.empty());
  }
}

/*----------------------------------------------------------*/

// Dependencies: size
TEST_CASE ("size", "1")
{
  SearchTree<int> tree;

  SECTION ("size - on empty tree", "[0.25]")
  {
    REQUIRE (tree.size() == 0);
  }

  // Dependencies: insert
  SECTION ("size - on non-empty tree", "[0.75]")
  {
    tree_insert(tree, { 4, 2, 7, 6 });

    REQUIRE (tree.size() == 4);
  }
}

/*----------------------------------------------------------*/

// Dependencies: insert
TEST_CASE ("begin", "2")
{
  SearchTree<int> tree;
  tree_insert(tree, { 4, 2, 7, 6 });

  SECTION ("begin - non-const", "[1.00]")
  {
    SearchTree<int>::iterator iter = tree.begin();
    REQUIRE (iter.m_nodePtr->data == 2);
  }

  SECTION ("begin - const", "[1.00]")
  {
    const SearchTree<int> const_tree = tree;
    SearchTree<int>::const_iterator iter = const_tree.begin();
    REQUIRE (iter.m_nodePtr->data == 2);
  }
}

/*----------------------------------------------------------*/

// Dependencies: insert, iterator decrement
TEST_CASE ("end", "2")
{
  SearchTree<int> tree;
  tree_insert(tree, { 4, 2, 7, 6 });

  SECTION ("end - non-const", "[1.00]")
  {
    SearchTree<int>::iterator iter = tree.end();
    --iter;
    REQUIRE (iter.m_nodePtr->data == 7);
  }

  SECTION ("end - const", "[1.00]")
  {
    const SearchTree<int> const_tree = tree;
    SearchTree<int>::const_iterator iter = const_tree.end();
    --iter;
    REQUIRE (iter.m_nodePtr->data == 7);
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("clear", "4")
{
  SearchTree<int> tree;

  SECTION ("clear - on empty empty tree", "1")
  {
    SECTION ("clear - on empty tree - state of tree", "[0.75]")
    {
      tree.clear();
      REQUIRE_TREE (tree, "[ ]", -1);
    }

    // Dependencies: size
    SECTION ("clear - on empty tree - size of tree", "[0.05]")
    {
      tree.clear();
      REQUIRE (tree.size() == 0);
    }

    // Dependencies: end
    SECTION ("clear - on empty tree - head links", "[0.20]")
    {
      tree.clear();
      REQUIRE_HEAD_NULL (tree);
    }
  }

  // Dependencies: insert
  SECTION ("clear - on non-empty tree", "3")
  {
    tree_insert(tree, { 4, 2, 7, 6 });

    SECTION ("clear - on non-empty tree - state of tree", "[2.25]")
    {
      tree.clear();
      REQUIRE_TREE (tree, "[ ]", -1);
    }

    // Dependencies: size
    SECTION ("clear - on non-empty tree - size of tree", "[0.15]")
    {
      tree.clear();
      REQUIRE (tree.size() == 0);
    }

    // Dependencies: end
    SECTION ("clear - on non-empty tree - head links", "[0.60]")
    {
      tree.clear();
      REQUIRE_HEAD_NULL (tree);
    }
  }
}

/*----------------------------------------------------------*/

// Dependencies: end
TEST_CASE ("find", "6")
{
  SearchTree<int> tree;

  SECTION ("find - on empty tree", "[2.00]")
  {
    REQUIRE (tree.find(5) == tree.end());
  }

  // Dependencies: insert
  SECTION ("find - on non-empty tree", "4")
  {
    SECTION ("find - on non-empty tree - find invalid value", "[1.00]")
    {
      tree_insert(tree, { 4, 2, 7, 6 });

      REQUIRE (tree.find(1) == tree.end());
    }

    SECTION ("find - on non-empty tree - find root", "[1.00]")
    {
      tree_insert(tree, { 4, 2, 7, 6 });

      REQUIRE (tree.find(4) != tree.end());
      REQUIRE (tree.find(4).m_nodePtr->data == 4);
    }

    SECTION ("find - on non-empty tree - find in skewed tree", "[1.00]")
    {
      tree_insert(tree, { 4, 3, 2, 1 });

      REQUIRE (tree.find(2) != tree.end());
      REQUIRE (tree.find(2).m_nodePtr->data == 2);
    }

    SECTION ("find - on non-empty tree - find in full tree", "[1.00]")
    {
      tree_insert(tree, { 4, 2, 1, 3, 6, 5, 7 });

      REQUIRE (tree.find(3) != tree.end());
      REQUIRE (tree.find(3).m_nodePtr->data == 3);
    }
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("insert", "12")
{
  SearchTree<int> tree;

  SECTION ("insert - root", "2")
  {
    tree.insert(17);

    SECTION ("insert - root - state of tree", "[1.50]")
    {
      REQUIRE_TREE (tree, "[ 17 ]", 0);
    }

    // Dependencies: size
    SECTION ("insert - root - size of tree", "[0.10]")
    {
      REQUIRE (tree.size() == 1);
    }

    // Dependencies: end
    SECTION ("insert - root - head links", "[0.40]")
    {
      REQUIRE_HEAD (tree, 17, 17, 17);
    }
  }

  SECTION ("insert - make simple tree", "2")
  {
    tree_insert (tree, { 4, 2, 7, 6 });

    SECTION ("insert - make simple tree - state of tree", "[1.50]")
    {
      REQUIRE_TREE (tree, "[ 2 4 6 7 ]", 2);
    }

    // Dependencies: size
    SECTION ("insert - make simple tree - size of tree", "[0.10]")
    {
      REQUIRE (tree.size() == 4);
    }

    // Dependencies: end
    SECTION ("insert - make simple tree - head links", "[0.40]")
    {
      REQUIRE_HEAD (tree, 2, 7, 4);
    }
  }

  SECTION ("insert - make skewed tree", "2")
  {
    tree_insert (tree, { 1, 3, 5, 7, 10 });

    SECTION ("insert - make skewed tree - state of tree", "[1.50]")
    {
      REQUIRE_TREE (tree, "[ 1 3 5 7 10 ]", 4);
    }

    // Dependencies: size
    SECTION ("insert - make skewed tree - size of tree", "[0.10]")
    {
      REQUIRE (tree.size() == 5);
    }

    // Dependencies: end
    SECTION ("insert - make skewed tree - head links", "[0.40]")
    {
      REQUIRE_HEAD (tree, 1, 10, 1);
    }
  }

  SECTION ("insert - make full tree", "2")
  {
    tree_insert (tree, { 8, 4, 2, 1, 3, 6, 5, 7, 12, 10, 9, 11, 14, 13, 15 });

    SECTION ("insert - make full tree - state of tree", "[1.50]")
    {
      REQUIRE_TREE (tree, "[ 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 ]", 3);
    }

    // Dependencies: size
    SECTION ("insert - make full tree - size of tree", "[0.10]")
    {
      REQUIRE (tree.size() == 15);
    }

    // Dependencies: end
    SECTION ("insert - make full tree - head links", "[0.40]")
    {
      REQUIRE_HEAD (tree, 1, 15, 8);
    }
  }

  SECTION ("insert - duplicate values", "1")
  {
    tree_insert(tree, { 2, 2, 1, 3, 3, 3});

    SECTION ("insert - duplicate values - state of tree", "[0.75]")
    {
      REQUIRE_TREE (tree, "[ 1 2 3 ]", 1);
    }

    // Dependencies: size
    SECTION ("insert - duplicate values - size of tree", "[0.05]")
    {
      REQUIRE (tree.size() == 3);
    }

    // Dependencies: end
    SECTION ("insert - duplicate values - head links", "[0.20]")
    {
      REQUIRE_HEAD (tree, 1, 3, 2);
    }
  }

  SECTION ("insert - make big tree", "1")
  {
    set<int> big_rand;
    mt19937 gen (0);
    int root = (gen () % 1000) + 19500;
    big_rand.insert (root);
    tree.insert(root);

    for (int i = 1; i < 10000; ++i)
    {
      int temp = gen () % 40000;
      big_rand.insert (temp);
      tree.insert(temp);
    }

    int min = * big_rand.begin();
    int max = * (--big_rand.end());

    string big_rand_string = "[ ";
    for (int num : big_rand)
    {
      big_rand_string += to_string(num) + " ";
    }
    big_rand_string += "]";

    SECTION ("insert - make big tree - state of tree", "[0.75]")
    {
      REQUIRE (tree.depth() == 27);

      // Done without "REQUIRE" to avoid long error messages.
      ostringstream stream;
      stream << "[ ";
      tree.printInOrder (stream);
      stream << "]";

      if (stream.str () != big_rand_string)
      {
        FAIL ("Tree state incorrect. Tree size too large to display.");
      }
    }

    // Dependencies: size
    SECTION ("insert - make big tree - size of tree", "[0.05]")
    {
      REQUIRE (tree.size() == big_rand.size());
    }

// Dependencies: end
    SECTION ("insert - make big tree - head links", "[0.20]")
    {
      REQUIRE_HEAD (tree, min, max, root);
    }
  }

  SECTION ("insert - return value", "2")
  {
    pair<SearchTree<int>::iterator, bool> ret;
    tree.insert(7);

    SECTION ("insert - return value - on unsuccessful insert", "1")
    {
      ret = tree.insert(7);

      SECTION ("insert - return value - on unsuccessful insert - iterator value", "[0.75]")
      {
        REQUIRE (ret.first.m_nodePtr == nullptr);
      }

      SECTION ("insert - return value - on unsuccessful insert - boolean value", "[0.25]")
      {
        REQUIRE_FALSE (ret.second);
      }
    }

    SECTION ("insert - return value - on successful insert", "1")
    {
      ret = tree.insert(4);

      SECTION ("insert - return value - on successful insert - iterator value", "[0.75]")
      {
        REQUIRE (ret.first.m_nodePtr != nullptr);
        REQUIRE (ret.first.m_nodePtr->data == 4);
      }

      SECTION ("insert - return value - on successful insert - boolean value", "[0.25]")
      {
        REQUIRE (ret.second);
      }
    }
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("erase", "18")
{
  SearchTree<int> tree;

  SECTION ("erase - on empty tree", "1")
  {
    tree.erase(5);

    SECTION ("erase - on empty tree - state of tree", "[0.75]")
    {
      REQUIRE_TREE (tree, "[ ]", -1);
    }

    // Dependencies: size
    SECTION ("erase - on empty tree - size of tree", "[0.05]")
    {
      REQUIRE (tree.size() == 0);
    }

    // Dependencies: end
    SECTION ("erase - on empty tree - head links", "[0.20]")
    {
      REQUIRE_HEAD_NULL (tree);
    }
  }

  // Dependencies: insert
  SECTION ("erase - root of tree", "2")
  {
    tree_insert (tree, { 4, 3, 2, 4, 7, 6, 8, 1, 9 });
    tree_erase (tree, { 4, 5, 6 });

    SECTION ("erase - root of tree - state of tree", "[1.50]")
    {
      REQUIRE_TREE (tree, "[ 1 2 3 7 8 9 ]", 3);
    }

    // Dependencies: size
    SECTION ("erase - root of tree - size of tree", "[0.10]")
    {
      REQUIRE (tree.size() == 6);
    }

    // Dependencies: end
    SECTION ("erase - root of tree - head links", "[0.40]")
    {
      REQUIRE_HEAD (tree, 1, 9, 7);
    }
  }

  // Dependencies: insert
  SECTION ("erase - minimum value of tree", "1")
  {
    tree_insert (tree, { 4, 2, 1, 3, 6, 5, 7 });
    tree_erase (tree, { 1, 2, 3 });

    SECTION ("erase - minimum value of tree - state of tree", "[0.75]")
    {
      REQUIRE_TREE (tree, "[ 4 5 6 7 ]", 2);
    }

    // Dependencies: size
    SECTION ("erase - minimum value of tree - size of tree", "[0.05]")
    {
      REQUIRE (tree.size() == 4);
    }

    // Dependencies: end
    SECTION ("erase - minimum value of tree - head links", "[0.20]")
    {
      REQUIRE_HEAD (tree, 4, 7, 4);
    }
  }

  // Dependencies: insert
  SECTION ("erase - maximum value of tree", "1")
  {
    tree_insert (tree, { 4, 2, 1, 3, 6, 5, 7 });
    tree_erase (tree, { 7, 6, 5 });

    SECTION ("erase - maximum value of tree - state of tree", "[0.75]")
    {
      REQUIRE_TREE (tree, "[ 1 2 3 4 ]", 2);
    }

    // Dependencies: size
    SECTION ("erase - maximum value of tree - size of tree", "[0.05]")
    {
      REQUIRE (tree.size() == 4);
    }

    // Dependencies: end
    SECTION ("erase - maximum value of tree - head links", "[0.20]")
    {
      REQUIRE_HEAD (tree, 1, 4, 4);
    }
  }

  // Dependencies: insert
  SECTION ("erase - leaf nodes", "1")
  {
    tree_insert (tree, { 4, 2, 6, 0, 3, 5, 7, 1 });
    tree_erase (tree, { 3, 5, 1 });

    SECTION ("erase - leaf nodes - state of tree", "[0.75]")
    {
      REQUIRE_TREE (tree, "[ 0 2 4 6 7 ]", 2);
    }

    // Dependencies: size
    SECTION ("erase - leaf nodes - size of tree", "[0.05]")
    {
      REQUIRE (tree.size() == 5);
    }

    // Dependencies: end
    SECTION ("erase - leaf nodes - head links", "[0.20]")
    {
      REQUIRE_HEAD (tree, 0, 7, 4);
    }
  }

  // Dependencies: insert
  SECTION ("erase - nodes with only left child", "2")
  {
    tree_insert (tree, { 5, 2, 8, 1, 4, 7, 3, 6 });
    tree_erase (tree, { 4, 6, 7 });

    SECTION ("erase - nodes with only left child - state of tree", "[1.50]")
    {
      REQUIRE_TREE (tree, "[ 1 2 3 5 8 ]", 2);
    }

    // Dependencies: size
    SECTION ("erase - nodes with only left child - size of tree", "[0.10]")
    {
      REQUIRE (tree.size() == 5);
    }

    // Dependencies: end
    SECTION ("erase - nodes with only left child - head links", "[0.40]")
    {
      REQUIRE_HEAD (tree, 1, 8, 5);
    }
  }

  // Dependencies: insert
  SECTION ("erase - nodes with only right child", "2")
  {
    tree_insert (tree, { 5, 2, 8, 1, 3, 6, 4, 7 });
    tree_erase (tree, { 4, 3, 7 });

    SECTION ("erase - nodes with only left child - state of tree", "[1.50]")
    {
      REQUIRE_TREE (tree, "[ 1 2 5 6 8 ]", 2);
    }

    // Dependencies: size
    SECTION ("erase - nodes with only left child - size of tree", "[0.10]")
    {
      REQUIRE (tree.size() == 5);
    }

    // Dependencies: end
    SECTION ("erase - nodes with only left child - head links", "[0.40]")
    {
      REQUIRE_HEAD (tree, 1, 8, 5);
    }
  }

  // Dependencies: insert
  SECTION ("erase - nodes with both children", "2")
  {
    tree_insert (tree, { 7, 3, 5, 4, 6, 2, 9, 8, 10, 1, 11 });
    tree_erase (tree, { 4, 8, 2 });

    SECTION ("erase - nodes with both children - state of tree", "[1.50]")
    {
      REQUIRE_TREE (tree, "[ 1 3 5 6 7 9 10 11 ]", 3);
    }

    // Dependencies: size
    SECTION ("erase - nodes with both children - size of tree", "[0.10]")
    {
      REQUIRE (tree.size() == 8);
    }

    // Dependencies: end
    SECTION ("erase - nodes with both children - head links", "[0.40]")
    {
      REQUIRE_HEAD (tree, 1, 11, 7);
    }
  }

  // Dependencies: insert
  SECTION ("erase - all values in tree", "2")
  {
    tree_insert (tree, { 2, 3, 1});
    tree_erase (tree, { 1, 3, 2 });

    SECTION ("erase - all values in tree - state of tree", "[1.50]")
    {
      REQUIRE_TREE (tree, "[ ]", -1);
    }

    // Dependencies: size
    SECTION ("erase - all values in tree - size of tree", "[0.10]")
    {
      REQUIRE (tree.size() == 0);
    }

    // Dependencies: end
    SECTION ("erase - all values in tree - head links", "[0.40]")
    {
      REQUIRE_HEAD_NULL (tree);
    }
  }

  // Dependencies: insert
  SECTION ("erase - duplicate values", "1")
  {
    tree_insert (tree, { 4, 2, 1, 3, 6, 5, 7 });
    tree_erase (tree, { 3, 3, 3, 1, 2, 2 });

    SECTION ("erase - duplicate values - state of tree", "[0.75]")
    {
      REQUIRE_TREE (tree, "[ 4 5 6 7 ]", 2);
    }

    // Dependencies: size
    SECTION ("erase - duplicate values - size of tree", "[0.05]")
    {
      REQUIRE (tree.size() == 4);
    }

    // Dependencies: end
    SECTION ("erase - duplicate values - head links", "[0.20]")
    {
      REQUIRE_HEAD (tree, 4, 7, 4);
    }
  }

  // Dependencies: insert
  SECTION ("erase - second largest/second smallest values", "1")
  {
    tree_insert (tree, { 4, 2, 1, 5, 7, 8 });
    tree_erase (tree, { 2, 7, 5 });
    SECTION ("erase - second largest/second smallest values - state of tree", "[0.75]")
    {
      REQUIRE_TREE (tree, "[ 1 4 8 ]", 1);
    }

    // Dependencies: size
    SECTION ("erase - second largest/second smallest values - size of tree", "[0.05]")
    {
      REQUIRE (tree.size() == 3);
    }

    // Dependencies: end
    SECTION ("erase - second largest/second smallest values - head links", "[0.20]")
    {
      REQUIRE_HEAD (tree, 1, 8, 4);
    }
  }

  SECTION ("erase - return value", "2")
  {
    SECTION ("erase - return value - on unsuccessful erase", "1")
    {
      SECTION ("erase - return value - on unsuccessful erase - on empty tree", "[0.50]")
      {
        REQUIRE_FALSE (tree.erase(5));
      }

      // Dependencies: insert
      SECTION ("erase - return value - on unsuccessful erase - with value not in tree", "[0.50]")
      {
        tree_insert (tree, { 4, 2, 7, 6 });

        REQUIRE_FALSE (tree.erase(5));
      }
    }

    // Dependencies: insert
    SECTION ("erase - return value - on successful erase", "1")
    {
      SECTION ("erase - return value - on successful erase - on last value in tree", "[0.25]")
      {
        tree.insert(7);

        REQUIRE (tree.erase(7));
      }

      SECTION ("erase - return value - on successful erase - on value other than last in tree", "[0.75]")
      {
        tree_insert (tree, { 2, 3, 1 });

        REQUIRE (tree.erase(3));
      }
    }
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("print level order", "12")
{
  SearchTree<int> tree;

  SECTION ("print level order - on empty tree", "[2.00]")
  {
    REQUIRE_LEVEL_ORDER (tree, "[ ]");
  }

  // Dependencies: insert
  SECTION ("print level order - on tree with just a root", "[2.00]")
  {
    tree.insert(42);

    REQUIRE_LEVEL_ORDER (tree, "[ 42 ]");
  }

  // Dependencies: insert
  SECTION ("print level order - on simple tree", "[2.00]")
  {
    tree_insert (tree, { 4, 2, 7, 6 });

    REQUIRE_LEVEL_ORDER (tree, "[ 4 | 2 7 | - - 6 - ]");
  }

  SECTION ("print level order - on full tree", "[2.00]")
  {
    tree_insert (tree, { 4, 2, 7, 1, 3, 6, 8 });
    REQUIRE_LEVEL_ORDER (tree, "[ 4 | 2 7 | 1 3 6 8 ]");
  }

  SECTION ("print level order - on skewed tree", "[2.00]")
  {
    tree_insert (tree, { 1, 2, 4, 3, 5, 6 });
    REQUIRE_LEVEL_ORDER (
      tree, "[ 1 | - 2 | - - - 4 | - - - - - - 3 5 | - - - - - - - - - - - - - - - 6 ]");
  }

  SECTION ("print level order - on big tree", "[2.00]")
  {
    tree_insert (
      tree,
      {
        49, 14, 12, 17, 65, 223, 4, 7, 6, 11, 14, 21, 15, 1, 2, 3, 8, 10, 44, 45, 46, 39, 81, 100,
        69, 73, 19, 18, 31, 32, 77, 76, 78, 45, 47, 13, 64, 63, 66, 250, 101, 102, 16, 50, 51, 52,
        53, 99, 104, 300, 999, 5
      }
    );

    REQUIRE_LEVEL_ORDER (
      tree,
      "[ 49 | 14 65 | 12 17 64 223 | 4 13 15 21 63 - 81 250 | 1 7 - - - 16 19 44 50 - - - 69 100 "
      "- 300 | - 2 6 11 - - - - - - - - 18 - 39 45 - 51 - - - - - - 66 73 99 101 - - - 999 | - - "
      "- 3 5 - 8 - - - - - - - - - - - - - - - - - - - - - 31 - - 46 - - - 52 - - - - - - - - - -"
      " - - - - - 77 - - - 102 - - - - - - - - | - - - - - - - - - - - - - 10 - - - - - - - - - -"
      " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 32 - - - - - 47 - - - -"
      " - - - 53 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 76 78 - - - - - - - "
      "104 - - - - - - - - - - - - - - - - ]"
    );
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("node default constructor", "3")
{
  SECTION ("node default constructor - with no given value", "[1.25]")
  {
    Node<int> node;

    REQUIRE (node.data == int());
    REQUIRE (node.left == nullptr);
    REQUIRE (node.right == nullptr);
    REQUIRE (node.parent == nullptr);
  }

  SECTION ("node default constructor - with given value", "[1.25]")
  {
    Node<int> node (5);

    REQUIRE (node.data == 5);
    REQUIRE (node.left == nullptr);
    REQUIRE (node.right == nullptr);
    REQUIRE (node.parent == nullptr);
  }

  SECTION ("node default constructor - for string node", "[0.50]")
  {
    Node<string> node ("nobody likes microtransactions");

    REQUIRE (node.data == "nobody likes microtransactions");
    REQUIRE (node.left == nullptr);
    REQUIRE (node.right == nullptr);
    REQUIRE (node.parent == nullptr);
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("node explicit constructor", "3")
{
  SECTION ("node explicit constructor - with nullptr links", "[0.50]")
  {
    Node<int> node (10, nullptr, nullptr, nullptr);

    REQUIRE (node.data == 10);
    REQUIRE (node.left == nullptr);
    REQUIRE (node.right == nullptr);
    REQUIRE (node.parent == nullptr);
  }

  // Dependencies: node default constructor
  SECTION ("node explicit constructor - with links to nodes created with default constructor", "[1.00]")
  {
    Node<int> node_a (1);
    Node<int> node_b;
    Node<int> node_c;
    Node<int> node_d (42, &node_a, &node_b, &node_c);

    REQUIRE (node_d.data == 42);
    REQUIRE (node_d.left == &node_a);
    REQUIRE (node_d.right == &node_b);
    REQUIRE (node_d.parent == &node_c);
  }

  SECTION ("node explicit constructor - with links to nodes created with explicit constructor", "[1.00]")
  {
    Node<int> node_a (1, nullptr, nullptr, nullptr);
    Node<int> node_b (17, nullptr, &node_a, nullptr);
    Node<int> node_c (39, &node_b, nullptr, &node_a);
    Node<int> node_d (14, &node_a, &node_b, &node_c);

    REQUIRE_ALL_EQUAL<Node<int>*> ({ node_a.left, node_a.right, node_a.parent,
        node_b.left, node_b.parent, node_c.right, nullptr });
    REQUIRE_ALL_EQUAL<Node<int>*> ({ &node_a, node_b.right, node_c.parent, node_d.left });
    REQUIRE_ALL_EQUAL<Node<int>*> ({ &node_b, node_c.left, node_d.right });
    REQUIRE (&node_c == node_d.parent);
  }

  SECTION ("node explicit constructor - for string node", "[0.50]")
  {
    Node<string> node ("print level order is fun", nullptr, nullptr, nullptr);

    REQUIRE (node.data == "print level order is fun");
    REQUIRE (node.left == nullptr);
    REQUIRE (node.right == nullptr);
    REQUIRE (node.parent == nullptr);
  }
}

/*----------------------------------------------------------*/

// Dependencies: node default constructor
TEST_CASE ("iterator dereference", "2")
{
  SECTION ("iterator dereference", "[2.00]")
  {
    Node<int> node (33);

    SearchTree<int>::iterator iter (&node);

    REQUIRE (* iter == 33);
  }
}

// Dependencies: insert
TEST_CASE ("iterator post-increment", "2")
{
  SearchTree<int> tree;

  SearchTree<int>::iterator iter = (tree.insert (9)).first;
  tree.insert (26);

  CHECK_AND_WARN (iter.m_nodePtr->data == 9,
    "Your insert needs to be fixed before this test can work.");

  SECTION ("iterator post-increment - value after increment", "[1.00]")
  {
    iter++;

    REQUIRE (iter.m_nodePtr->data == 26);
  }

  SECTION ("iterator post-increment - return value from incrementation", "[1.00]")
  {
    SearchTree<int>::iterator expected = iter, actual = iter;
    actual = iter++;

    REQUIRE (actual == expected);
    REQUIRE (actual != iter);
  }
}

/*----------------------------------------------------------*/

// Dependencies: insert
TEST_CASE ("iterator post-decrement", "2")
{
  SearchTree<int> tree;

  SearchTree<int>::iterator iter = (tree.insert (25)).first;
  tree.insert (9);

  CHECK_AND_WARN (iter.m_nodePtr->data == 25,
    "Your insert needs to be fixed before this test can work.");

  SECTION ("iterator post-decrement - value after decrement", "[1.00]")
  {
    iter--;

    REQUIRE (iter.m_nodePtr->data == 9);
  }

  SECTION ("iterator post-decrement - return value from decrementation", "[1.00]")
  {
    SearchTree<int>::iterator expected = iter, actual = iter;
    actual = iter--;

    REQUIRE (actual == expected);
    REQUIRE (actual != iter);
  }
}

/*----------------------------------------------------------*/

// Dependencies: insert
TEST_CASE ("iterator increment", "10")
{
  SearchTree<int> tree;
  SearchTree<int>::iterator iter;

  SECTION ("iterator increment - on node that has a right child", "2")
  {
    SECTION ("iterator increment - on node that has a right child - with no subtree", "[1.00]")
    {
      tree_insert (tree, { 4, 2, 8 });
      iter = (tree.insert (6)).first;
      tree.insert (7);
      CHECK_AND_WARN (iter.m_nodePtr->data == 6,
        "Your insert needs to be fixed before this test can work.");

      ++iter;

      REQUIRE (iter.m_nodePtr->data == 7);
    }

    SECTION ("iterator increment - on node that has a right child - with a subtree", "[1.00]")
    {
      tree_insert (tree, { 3, 1, 9 });
      iter = (tree.insert (4)).first;
      tree_insert (tree, { 8, 7, 6, 5 });
      CHECK_AND_WARN (iter.m_nodePtr->data == 4,
        "Your insert needs to be fixed before this test can work.");

      ++iter;

      REQUIRE (iter.m_nodePtr->data == 5);
    }
  }

  SECTION ("iterator increment - on node that has both children", "[1.00]")
  {
    tree_insert (tree, { 20, 10 });
    iter = (tree.insert (30)).first;
    tree_insert (tree, { 5, 15, 25, 35, 2, 8, 13, 17, 21, 29, 34, 36 });
    CHECK_AND_WARN (iter.m_nodePtr->data == 30,
      "Your insert needs to be fixed before this test can work.");

    ++iter;

    REQUIRE (iter.m_nodePtr->data == 34);
  }

  SECTION ("iterator increment - on node that is a left child with no children", "[1.75]")
  {
    tree_insert (tree, { 4, 2, 7 });
    iter = (tree.insert (6)).first;
    CHECK_AND_WARN (iter.m_nodePtr->data == 6,
      "Your insert needs to be fixed before this test can work.");

    ++iter;

    REQUIRE (iter.m_nodePtr->data == 7);
  }

  SECTION ("iterator increment - on node that is a right child with no children", "[1.75]")
  {
    tree_insert (tree, { 10, 5, 15, 1, 2, 3 });
    iter = (tree.insert (4)).first;
    CHECK_AND_WARN (iter.m_nodePtr->data == 4,
      "Your insert needs to be fixed before this test can work.");

    ++iter;

    REQUIRE (iter.m_nodePtr->data == 5);
  }

  // Dependencies: end
  SECTION ("iterator increment - on maximum value node in tree", "[1.50]")
  {
    tree_insert (tree, { 4, 2, 3, 7, 6 });
    iter = (tree.insert (8)).first;
    CHECK_AND_WARN (iter.m_nodePtr->data == 8,
      "Your insert needs to be fixed before this test can work.");

    ++iter;

    REQUIRE (iter == tree.end());
  }

  SECTION ("iterator increment - on node that is the left child of the tree root when it is the maximum value node in tree", "[1.00]")
  {
    tree.insert(6);
    iter = (tree.insert (4)).first;
    tree_insert (tree, { 3, 2, 1 });
    CHECK_AND_WARN (iter.m_nodePtr->data == 4,
      "Your insert needs to be fixed before this test can work.");

    ++iter;

    REQUIRE (iter.m_nodePtr->data == 6);
  }

  // Dependencies: end
  SECTION ("iterator increment - on root of tree with only 1 value", "[1.00]")
  {
    iter = (tree.insert (17)).first;
    CHECK_AND_WARN (iter.m_nodePtr->data == 17,
      "Your insert needs to be fixed before this test can work.");

    ++iter;

    REQUIRE (iter == tree.end());
  }
}

/*----------------------------------------------------------*/

// Dependencies: insert
TEST_CASE ("iterator decrement", "10")
{
  SearchTree<int> tree;
  SearchTree<int>::iterator iter;

  SECTION ("iterator decrement - on node that has a left child", "2")
  {
    SECTION ("iterator decrement - on node that has a left child - with no subtree", "[1.00]")
    {
      tree_insert (tree, { 8, 1, 9 });
      iter = (tree.insert (7)).first;
      tree.insert (2);
      CHECK_AND_WARN (iter.m_nodePtr->data == 7,
        "Your insert needs to be fixed before this test can work.");

      --iter;

      REQUIRE (iter.m_nodePtr->data == 2);
    }

    SECTION ("iterator decrement - on node that has a left child - with a subtree", "[1.00]")
    {
      tree_insert (tree, { 8, 1, 9 });
      iter = (tree.insert (7)).first;
      tree_insert (tree, { 2, 3, 4, 5, });
      CHECK_AND_WARN (iter.m_nodePtr->data == 7,
        "Your insert needs to be fixed before this test can work.");

      --iter;

      REQUIRE (iter.m_nodePtr->data == 5);
    }
  }

  SECTION ("iterator decrement - on node that has both children", "[1.00]")
  {
    tree_insert (tree, { 20, 30 });
    iter = (tree.insert (10)).first;
    tree_insert (tree, { 5, 15, 25, 35, 2, 8, 13, 17, 21, 29, 34, 36 });
    CHECK_AND_WARN (iter.m_nodePtr->data == 10,
      "Your insert needs to be fixed before this test can work.");

    --iter;

    REQUIRE (iter.m_nodePtr->data == 8);
  }

  SECTION ("iterator decrement - on node that is a right child with no children", "[1.75]")
  {
    tree_insert (tree, { 4, 2, 7 });
    iter = (tree.insert (3)).first;
    CHECK_AND_WARN (iter.m_nodePtr->data == 3,
      "Your insert needs to be fixed before this test can work.");

    --iter;

    REQUIRE (iter.m_nodePtr->data == 2);
  }

  SECTION ("iterator decrement - on node that is a left child with no children", "[1.75]")
  {
    tree_insert (tree, { 10, 5, 15, 14, 13, 12 });
    iter = (tree.insert (11)).first;
    CHECK_AND_WARN (iter.m_nodePtr->data == 11,
      "Your insert needs to be fixed before this test can work.");

    --iter;

    REQUIRE (iter.m_nodePtr->data == 10);
  }

  // Dependencies: end
  SECTION ("iterator decrement - on minimum value node in tree", "[1.50]")
  {
    tree_insert (tree, { 4, 2, 3, 7, 6 });
    iter = (tree.insert (1)).first;
    CHECK_AND_WARN (iter.m_nodePtr->data == 1,
      "Your insert needs to be fixed before this test can work.");

    --iter;

    REQUIRE (iter == tree.end());
  }

  SECTION ("iterator decrement - on node that is the right child of the tree root when it is the minimum value node in tree", "[1.00]")
  {
    tree.insert(6);
    iter = (tree.insert (8)).first;
    tree_insert (tree, { 11, 14, 19 });
    CHECK_AND_WARN (iter.m_nodePtr->data == 8,
      "Your insert needs to be fixed before this test can work.");

    --iter;

    REQUIRE (iter.m_nodePtr->data == 6);
  }

  // Dependencies: end
  SECTION ("iterator decrement - on root of tree with only 1 value", "[1.00]")
  {
    iter = (tree.insert (13)).first;
    CHECK_AND_WARN (iter.m_nodePtr->data == 13,
      "Your insert needs to be fixed before this test can work.");

    --iter;

    REQUIRE (iter == tree.end());
  }
}

/*----------------------------------------------------------*/

// Dependencies: insert
TEST_CASE ("make string tree", "1")
{
  SearchTree<string> tree;
  tree_insert (tree, { "my", "name", "jeff" });

  SECTION ("make string tree - state of tree", "[0.75]")
  {
    REQUIRE_TREE (tree, "[ jeff my name ]", 1);
  }

  // Dependencies: size
  SECTION ("make string tree - size of tree", "[0.05]")
  {
    REQUIRE (tree.size() == 3);
  }

  // Dependencies: end
  SECTION ("make string tree - head links", "[0.20]")
  {
    const Node<string>* head = tree.end().m_nodePtr;

    REQUIRE (head->right != nullptr);
    REQUIRE (head->left != nullptr);
    REQUIRE (head->parent != nullptr);

    REQUIRE (head->right != head);
    REQUIRE (head->left != head);
    REQUIRE (head->parent != head);

    REQUIRE (head->right->data == "jeff");
    REQUIRE (head->left->data == "name");
    REQUIRE (head->parent->data == "my");
  }
}

/*----------------------------------------------------------*/

// Dependencies: insert, erase, end, clear, iterator dereference, copy constructor
TEST_CASE ("stress test", "2")
{
  SECTION ("stress test", "[2.00]")
  {
    SearchTree<int> tree_a;

    set<int> big_rand;
    mt19937 gen (0);
    int root = (gen () % 1000 ) + 19500;
    big_rand.insert (root);
    tree_a.insert(root);

    for (int i = 1; i < 20000; ++i)
    {
      int temp = gen () % 40000;
      big_rand.insert (temp);
      tree_a.insert(temp);
    }

    int min = * big_rand.begin();
    int max = * (--big_rand.end());

    string big_rand_string = "[ ";
    for (int num : big_rand)
    {
      big_rand_string += to_string(num) + " ";
    }
    big_rand_string += "]";

    // Done without "REQUIRE" to avoid long error messages.
    ostringstream stream;
    stream << "[ ";
    tree_a.printInOrder (stream);
    stream << "]";
    if (stream.str () != big_rand_string)
    {
      FAIL ("Tree state incorrect. Tree size too large to display.");
    }

    REQUIRE (tree_a.depth() == 30);
    REQUIRE (tree_a.size() == big_rand.size());
    REQUIRE_HEAD (tree_a, min, max, root);

    SearchTree<int> tree_b (tree_a);
    tree_b.clear ();

    REQUIRE_TREE (tree_b, "[ ]", -1);
    REQUIRE (tree_b.size() == 0);
    REQUIRE_HEAD_NULL (tree_b);

    while (tree_a.size() > 0)
    {
      tree_a.erase (* (--tree_a.end()));
    }

    REQUIRE_TREE (tree_a, "[ ]", -1);
    REQUIRE (tree_a.size() == 0);
    REQUIRE_HEAD_NULL (tree_a);

    tree_insert (tree_a, { 4, 2, 6, 1, 3, 5, 7 });

    REQUIRE_TREE (tree_a, "[ 1 2 3 4 5 6 7 ]", 2);
    REQUIRE (tree_a.size() == 7);
    REQUIRE_HEAD (tree_a, 1, 7, 4);
  }
}

/************************************************************/
// Helpers

template<typename T>
void
REQUIRE_TREE (const SearchTree<T>& tree, string treeString)
{
  ostringstream output;
  output << "[ ";
  tree.printInOrder(output);
  output << "]";
  REQUIRE (output.str () == treeString);
}

/*----------------------------------------------------------*/

template<typename T>
void
REQUIRE_TREE (const SearchTree<T>& tree, string treeString, int depth)
{
  REQUIRE_TREE (tree, treeString);
  REQUIRE (tree.depth() == depth);
}

/*----------------------------------------------------------*/

template<typename T>
void
REQUIRE_HEAD (const SearchTree<T>& tree, T right, T left, T parent)
{
  const Node<T>* head = tree.end().m_nodePtr;

  REQUIRE (head->right != nullptr);
  REQUIRE (head->left != nullptr);
  REQUIRE (head->parent != nullptr);

  REQUIRE (head->right != head);
  REQUIRE (head->left != head);
  REQUIRE (head->parent != head);

  REQUIRE (head->right->data == right);
  REQUIRE (head->left->data == left);
  REQUIRE (head->parent->data == parent);
}

/*----------------------------------------------------------*/

template<typename T>
void
REQUIRE_HEAD_NULL (const SearchTree<T>& tree)
{
  const Node<T>* head = tree.end().m_nodePtr;

  REQUIRE (head->left == nullptr);
  REQUIRE (head->right == nullptr);
  REQUIRE (head->parent == nullptr);
}

/*----------------------------------------------------------*/

template<typename T>
void
REQUIRE_LEVEL_ORDER (const SearchTree<T>& tree, string answer)
{
  ostringstream os;
  string out, out_no_space, out_no_dash, answer_no_space, answer_no_dash;

  os << tree;
  out = os.str();

  out_no_space = out_no_dash = out;
  answer_no_dash = answer_no_space = answer;

  out_no_space.erase(remove(out_no_space.begin(), out_no_space.end(), ' '), out_no_space.end());
  out_no_dash.erase(remove(out_no_dash.begin(), out_no_dash.end(), '-'), out_no_dash.end());

  answer_no_space.erase(remove(answer_no_space.begin(), answer_no_space.end(), ' '),
    answer_no_space.end());
  answer_no_dash.erase(remove(answer_no_dash.begin(), answer_no_dash.end(), '-'),
    answer_no_dash.end());

  CHECK (out == answer);

  if (out != answer)
  {
    if (out_no_dash == answer_no_dash)
    {
      WARN ("Your output is correct except for dashes\n");
    }
    else if (out_no_space == answer_no_space)
    {
      WARN ("Your output is correct except for spaces\n");
    }
  }
}

/*----------------------------------------------------------*/

template<typename T>
void
REQUIRE_ALL_EQUAL (const vector<T>& values)
{
  T temp = NULL;

  for (T value : values)
  {
    if (temp != NULL)
    {
      REQUIRE (temp == value);
    }
    temp = value;
  }
}

/*----------------------------------------------------------*/

void
CHECK_AND_WARN (bool condition, string message)
{
  CHECK (condition);
  if (!condition)
  {
    cout << message << "\n";
  }
}

/*----------------------------------------------------------*/

ostringstream
CHECK_AND_WARN (bool condition, string message, ostringstream stream)
{
  CHECK (condition);
  if (!condition)
  {
    stream << message << "\n";
  }

  return stream;
}

/*----------------------------------------------------------*/

template<typename T>
void
tree_insert (SearchTree<T>& tree, const vector<T>& values)
{
  for (T value : values)
  {
    tree.insert (value);
  }
}

/*----------------------------------------------------------*/

template<typename T>
void
tree_erase (SearchTree<T>& tree, const vector<T>& values)
{
  for (T value : values)
  {
    tree.erase (value);
  }
}

/*----------------------------------------------------------*/

/************************************************************/
