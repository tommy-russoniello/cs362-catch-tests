#define CATCH_CONFIG_MAIN
#include "catch.hpp"

/************************************************************/
// System includes

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

/************************************************************/
// Local includes

#include "List.hpp"

/************************************************************/
// Using declarations

using std::advance;
using std::equal;
using std::ostringstream;
using std::string;
using std::to_string;

/************************************************************/

TEST_CASE ("default constructor [%3]", "[default ctor]")
{
  List<int> list;
  REQUIRE (list.empty ());
  REQUIRE (list.size () == 0);
  REQUIRE (list.begin () == list.end ());

  ostringstream output;
  output << list;
  REQUIRE (output.str () == "[ ]");
}

/*----------------------------------------------------------*/

TEST_CASE ("initializer list constructor [%4]", "[init list ctor]")
{
  List<int> list_a { 3, 10, 44, 6 };
  List<int> list_b { 3, 10, 44, 6 };
  REQUIRE (equal (list_a.begin (), list_a.end (), list_b.begin ()));

  ostringstream output;
  output << list_a;
  REQUIRE (output.str () == "[ 3 10 44 6 ]");
}

/*----------------------------------------------------------*/
/* RANGE CTOR NOT PART OF ASSIGNMENT AS OF FALL 2017
 * UNCOMMENT IF ADDED TO ASSIGNMENT
 * TO GET %4, TAKE %1 FROM INSERT, ERASE, RANGE ERASE, AND OP=
TEST_CASE ("range constructor [%4]", "range ctor")
{
  List<int> list_a { 3, 10, 44, 6, 69 };
  List<int>::iterator start = list_a.begin(), end = list_a.end();
  advance (start, 1);
  advance (end, -1);
  List<int> list_b (start, end);

  ostringstream output;
  output << list_b;
  REQUIRE (output.str () == "[ 10 44 6 ]");
}
*/
/*----------------------------------------------------------*/

TEST_CASE ("size constructor [%4]", "[size ctor]")
{
  List<int> list_a (3, 9);
  ostringstream output;
  output << list_a;
  REQUIRE (output.str () == "[ 9 9 9 ]");

  List<int> list_b (5);
  output.str ("");
  output << list_b;
  REQUIRE (output.str () == "[ 0 0 0 0 0 ]");
}

/*----------------------------------------------------------*/

TEST_CASE ("copy constructor [%4]", "[copy ctor]")
{
  List<int> list_a { 3, 10, 44, 6 };
  List<int> list_b (list_a);

  ostringstream output;
  output << list_b;
  REQUIRE (output.str () == "[ 3 10 44 6 ]");
  *list_a.begin () = 7;
  REQUIRE (*list_b.begin () == 3);
}

/*----------------------------------------------------------*/

TEST_CASE ("assignment operator [%7]", "[op=]")
{
  List<int> list_a { 1, 2, 3 };
  List<int> list_b { 4, 5, 6 };
  List<int> list_c { 7, 8, 9 };

  ostringstream output;

  SECTION ("assignment operator - single [%4]", "operator=")
  {
    list_b = list_a;

    output << list_b;
    REQUIRE (output.str () == "[ 1 2 3 ]");
    *list_a.begin () = 10;
    REQUIRE (*list_b.begin () != *list_a.begin ());
  }

  SECTION ("assignment operator - chained [%2]")
  {
    list_c = list_b = list_a;

    output << list_a;
    REQUIRE (output.str () == "[ 1 2 3 ]");
    REQUIRE (equal(list_a.begin(), list_a.end(), list_b.begin()));
    REQUIRE (equal(list_b.begin(), list_b.end(), list_c.begin()));
  }

  SECTION ("assignment operator - self-assignment [%1]")
  {
    list_a = list_a;

    output << list_a;

    REQUIRE (output.str () == "[ 1 2 3 ]");
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("empty [%1]", "[empty]")
{
  SECTION ("empty - empty [%.5]")
  {
    List<int> list;

    REQUIRE (list.empty() == 1);
  }

  SECTION ("empty - non-empty [%.5]")
  {
    List<int> list { 3, 10, 44, 6 };

    REQUIRE (list.empty() == 0);
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("size [%1]", "[size]")
{
  List<int> list { 3, 10, 44, 6 };

  REQUIRE (list.size() == 4);
}

/*----------------------------------------------------------*/

TEST_CASE ("front [%2]", "[front]")
{
  List<int> list { 3, 10, 44, 6 };

  REQUIRE (list.front() == 3);
}

/*----------------------------------------------------------*/

TEST_CASE ("back [%2]", "[back]")
{
  List<int> list { 3, 10, 44, 6 };

  REQUIRE (list.back() == 6);
}

/*----------------------------------------------------------*/

TEST_CASE ("push back [%2]", "[push_back]")
{
  ostringstream output;

  SECTION ("push back - empty [%1]")
  {
    List<int> empty_list;

    empty_list.push_back(69);

    output << empty_list;
    REQUIRE (output.str () == "[ 69 ]");
  }

  SECTION ("push back - non-empty [%1]")
  {
    List<int> list { 3, 10, 44, 6 };

    list.push_back(69);
    list.push_back(99);
    list.push_back(0);

    output << list;
    REQUIRE (output.str () == "[ 3 10 44 6 69 99 0 ]");
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("pop back [%2]", "[pop_back]")
{
  List<int> list { 3, 10, 44, 6, 69 };

  list.pop_back();
  list.pop_back();
  list.pop_back();

  ostringstream output;
  output << list;
  REQUIRE (output.str () == "[ 3 10 ]");
}

/*----------------------------------------------------------*/

TEST_CASE ("push front [%2]", "[push_front]")
{
  ostringstream output;

  SECTION ("push front - empty [%1]")
  {
    List<int> empty_list;

    empty_list.push_front (69);

    output << empty_list;
    REQUIRE (output.str () == "[ 69 ]");
  }

  SECTION ("push front - non-empty [%1]")
  {
    List<int> list { 3, 10, 44, 6 };

    list.push_front(69);
    list.push_front(99);
    list.push_front(0);

    output << list;
    REQUIRE (output.str () == "[ 0 99 69 3 10 44 6 ]");
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("pop front [%2]", "[pop_front]")
{
  List<int> list { 3, 10, 44, 6, 69 };

  list.pop_front();
  list.pop_front();
  list.pop_front();

  ostringstream output;
  output << list;
  REQUIRE (output.str () == "[ 6 69 ]");
}

/*----------------------------------------------------------*/

TEST_CASE ("begin [%3]", "[begin]")
{
  SECTION ("begin - non-const [%1.5]")
  {
    List<int> list { 3, 10, 44, 6 };
    List<int>::iterator iter = list.begin();
    REQUIRE (* iter == 3);
  }

  SECTION ("begin - const [%1.5]")
  {
    List<int> list { 3, 10, 44, 6 };
    List<int>::const_iterator iter = list.begin();
    REQUIRE (* iter == 3);
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("end [%3]", "[end]")
{
  SECTION ("end - non-const [%1.5]")
  {
    List<int> list { 3, 10, 44, 6 };
    List<int>::iterator iter = list.end();
    advance (iter, -1);
    REQUIRE (* iter == 6);
  }

  SECTION ("end - const [%1.5]")
  {
    List<int> list { 3, 10, 44, 6 };
    List<int>::const_iterator iter = list.end();
    advance (iter, -1);
    REQUIRE (* iter == 6);
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("insert [%13]", "[insert]")
{
  List<int> list { 3, 10, 44, 6 };
  ostringstream output;

  SECTION ("insert - empty [%3]")
  {
    List<int> empty_list;

    for (int i = 0; i < 10; ++i)
    {
      empty_list.insert (empty_list.end(), i);
    }

    output << empty_list;
    REQUIRE (output.str () == "[ 0 1 2 3 4 5 6 7 8 9 ]");
  }

  SECTION ("insert - front [%3]")
  {
    for (int i = 0; i < 10; ++i)
    {
      list.insert (list.begin(), i);
    }

    output << list;
    REQUIRE (output.str () == "[ 9 8 7 6 5 4 3 2 1 0 3 10 44 6 ]");
  }

  SECTION("insert - back [%3]")
  {
    for (int i = 0; i < 10; ++i)
    {
      list.insert (list.end(), i);
    }

    output << list;
    REQUIRE (output.str () == "[ 3 10 44 6 0 1 2 3 4 5 6 7 8 9 ]");
  }

  SECTION ("insert - middle [%3]")
  {
    List<int>::iterator iter = list.begin();
    advance(iter,2);

    for (int i = 0; i < 10; ++i)
    {
      list.insert (iter, i);
    }

    output << list;
    REQUIRE (output.str () == "[ 3 10 0 1 2 3 4 5 6 7 8 9 44 6 ]");
  }

  SECTION ("insert - return value [%1]")
  {
    int insertValue = 5;
    List<int>::iterator iter = list.insert(list.begin(), insertValue);

    REQUIRE (* iter == insertValue);
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("erase [%13]", "[erase]")
{
  List<int> list { 3, 10, 44, 6, 69, 99, 0 };
  List<int>::iterator iter;
  ostringstream output;

  SECTION ("erase - middle [%3]")
  {
    for (int i = 0; i < 3; ++i)
    {
      iter = list.begin();
      advance(iter,2);
      list.erase(iter);
    }

    output << list;
    REQUIRE (output.str () == "[ 3 10 99 0 ]");
  }

  SECTION ("erase - front [%3]")
  {
    for (int i = 0; i < 3; ++i)
    {
      list.erase(list.begin());
    }

    output << list;
    REQUIRE (output.str () == "[ 6 69 99 0 ]");
  }

  SECTION ("erase - back [%3]")
  {
    for (int i = 0; i < 3; ++i)
    {
      iter = list.end();
      advance(iter, -1);
      list.erase(iter);
    }

    output << list;
    REQUIRE (output.str () == "[ 3 10 44 6 ]");
  }

  SECTION ("erase - all [%2]")
  {
    int size = list.size();
    for (int i = 0; i < size; ++i)
    {
      list.erase(list.begin());
    }

    output << list;
    REQUIRE (output.str () == "[ ]");
  }

  SECTION ("erase - return value [%2]")
  {
    iter = list.begin();
    advance (iter, 1);
    iter = list.erase(iter);
    REQUIRE (* iter == 44);
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("range erase [%7]", "[range erase]")
{
  List<int> list { 3, 10, 44, 6, 69, 99, 0 };
  ostringstream output;
  List<int>::iterator first = list.begin(), last = list.end();

  SECTION ("range erase - middle [%1.5]")
  {
    advance(first,2);
    advance(last, -2);
    list.erase(first, last);

    output << list;
    REQUIRE (output.str () == "[ 3 10 99 0 ]");
  }

  SECTION ("range erase - front [%1.5]")
  {
    advance(first, 3);
    list.erase(list.begin(), first);

    output << list;
    REQUIRE (output.str () == "[ 6 69 99 0 ]");
  }

  SECTION ("range erase - back [%1.5]")
  {
    advance(last, -3);
    list.erase(last, list.end());

    output << list;
    REQUIRE (output.str () == "[ 3 10 44 6 ]");
  }

  SECTION ("range erase - all [%1]")
  {
    list.erase(list.begin(), list.end());

    output << list;
    REQUIRE (output.str () == "[ ]");
  }

  SECTION ("range erase - return [%1.5]")
  {
    advance(first, 2);
    advance(last, -2);
    List<int>::iterator iter = list.erase(first, last);

    REQUIRE (* iter == 99);
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("large list [%2]", "[general]")
{
  List<int> list;

  for (int i = 0; i <= 15000; ++i)
  {
    list.push_back (i);
  }

  List<int>::iterator iter = list.end();
  advance(iter, -1);

  ostringstream output;
  output << "first: " << *(list.begin ()) << ", last: " << *(iter) << ", size: " << list.size();
  REQUIRE (output.str () == "first: 0, last: 15000, size: 15001");
}

/*----------------------------------------------------------*/

TEST_CASE ("string list [%2]", "[general]")
{
  List<string> list;

  for (int i = 1; i < 4; ++i)
  {
    list.push_back (to_string(i) + "pac,");
  }
  list.push_back ("4");

  ostringstream output;
  output << list;
  REQUIRE (output.str () == "[ 1pac, 2pac, 3pac, 4 ]");
}

/*----------------------------------------------------------*/

TEST_CASE ("iter equality operators [%6]", "[iter]")
{
  List<int> list { 3, 10, 44 };

  List<int>::iterator begin = list.begin(), other_begin = list.begin(), end = list.end();
  List<int>::const_iterator const_begin = list.begin(), other_const_begin = list.begin(),
    const_end = list.end();

  SECTION ("iter equality operators - equals operator [%3]")
  {
    SECTION ("iter equals operator - iter~iter [%1]")
    {
      REQUIRE (begin == other_begin);
      REQUIRE_FALSE (begin == end);
    }

    SECTION ("iter equals operator - iter~const iter [%1]")
    {
      REQUIRE (begin == const_begin);
      REQUIRE_FALSE (begin == const_end);
    }

    SECTION ("iter equals operator - const iter~const iter [%1]")
    {
      REQUIRE (const_begin == other_const_begin);
      REQUIRE_FALSE (const_begin == const_end);
    }
  }

  SECTION ("iter equality operators - not equals operator [%3]")
  {
    SECTION ("iter not equals operator - iter~iter [%1]")
    {
      REQUIRE (begin != end);
      REQUIRE_FALSE (begin != other_begin);
    }

    SECTION ("iter not equals operator - iter~const iter [%1]")
    {
      REQUIRE (begin != const_end);
      REQUIRE_FALSE (begin != const_begin);
    }

    SECTION ("iter not equals operator - const iter~const iter [%1]")
    {
      REQUIRE (const_begin != const_end);
      REQUIRE_FALSE (const_begin != other_const_begin);
    }
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("iter unary operators [%6]", "[iter]")
{
  List<int> list { 3, 10, 44 };

  SECTION ("iter unary operators - increment [%3]")
  {
    SECTION ("iter increment operator - pre [%1.5]")
    {
      SECTION ("iter pre-increment - non-const [%.75]")
      {
        SECTION ("iter non-const pre-increment - value [%.5]")
        {
          List<int>::iterator iter = list.begin();
          ++iter;
          REQUIRE (* iter == 10);
        }

        SECTION ("iter non-const pre-increment - return [%.25]")
        {
          List<int>::iterator iter = list.begin(), actual = list.begin(), expected = list.begin();

          advance(expected, 1);
          actual = ++iter;

          REQUIRE (expected == actual);
        }
      }

      SECTION ("iter pre-increment - const [%.75]")
      {
        SECTION ("iter const pre-increment - value [%.5]")
        {
          List<int>::const_iterator iter = list.begin();
          ++iter;
          REQUIRE (* iter == 10);
        }

        SECTION ("iter const pre-increment - return [%.25]")
        {
          List<int>::const_iterator iter = list.begin(), actual = list.begin(), expected = list.begin();

          advance(expected, 1);
          actual = ++iter;

          REQUIRE (expected == actual);
        }
      }
    }

    SECTION ("iter increment operator - post [%1.5]")
    {
      SECTION ("iter post-increment - non-const [%.75]")
      {
        SECTION ("iter non-const post-increment - value [%.5]")
        {
          List<int>::iterator iter = list.begin();
          iter++;
          REQUIRE (* iter == 10);
        }

        SECTION ("iter non-const post-increment - return [%.25]")
        {
          List<int>::iterator iter = list.begin(), actual = list.begin(), expected = list.begin();

          actual = iter++;

          REQUIRE (expected == actual);
        }
      }

      SECTION ("iter post-increment - const [%.75]")
      {
        SECTION ("iter const post-increment - value [%.5]")
        {
          List<int>::const_iterator iter = list.begin();
          iter++;
          REQUIRE (* iter == 10);
        }

        SECTION ("iter const post-increment - return [%.25]")
        {
          List<int>::const_iterator iter = list.begin(), actual = list.begin(), expected = list.begin();

          actual = iter++;

          REQUIRE (expected == actual);
        }
      }
    }
  }

  SECTION ("iter unary operators - decrement [%3]")
  {
    SECTION ("iter decrement operator - pre [%1.5]")
    {
      SECTION ("iter pre-decrement - non-const [%.75]")
      {
        SECTION ("iter non-const pre-decrement - value [%.5]")
        {
          List<int>::iterator iter = list.end();
          --iter;
          REQUIRE (* iter == 44);
        }

        SECTION ("iter non-const pre-decrement - return [%.25]")
        {
          List<int>::iterator iter = list.end(), actual = list.end(), expected = list.end();

          advance(expected, -1);
          actual = --iter;

          REQUIRE (expected == actual);
        }
      }

      SECTION ("iter pre-decrement - const [%.75]")
      {
        SECTION ("iter const pre-decrement - value [%.5]")
        {
          List<int>::const_iterator iter = list.end();
          --iter;
          REQUIRE (* iter == 44);
        }

        SECTION ("iter const pre-decrement - return [%.25]")
        {
          List<int>::const_iterator iter = list.end(), actual = list.end(), expected = list.end();

          advance(expected, -1);
          actual = --iter;

          REQUIRE (expected == actual);
        }
      }
    }

    SECTION ("iter decrement operator - post [%1.5]")
    {
      SECTION ("iter post-decrement - non-const [%.75]")
      {
        SECTION ("iter non-const post-decrement - value [%.5]")
        {
          List<int>::iterator iter = list.end();
          iter--;
          REQUIRE (* iter == 44);
        }

        SECTION ("iter non-const post-decrement - return [%.25]")
        {
          List<int>::iterator iter = list.end(), actual = list.end(), expected = list.end();

          actual = iter--;

          REQUIRE (expected == actual);
        }
      }

      SECTION ("iter post-decrement - const [%.75]")
      {
        SECTION ("iter const post-decrement - value [%.5]")
        {
          List<int>::const_iterator iter = list.end();
          iter--;
          REQUIRE (* iter == 44);
        }

        SECTION ("iter const post-decrement - return [%.25]")
        {
          List<int>::const_iterator iter = list.end(), actual = list.end(), expected = list.end();

          actual = iter--;

          REQUIRE (expected == actual);
        }
      }
    }
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("iter change value [%3]", "[iter]")
{
  List<int> list { 3, 10, 44 };

  int newValue = 5;
  List<int>::iterator iter = list.begin();
  * iter = newValue;

  REQUIRE (* iter == newValue);

  ostringstream output;
  output << list;
  REQUIRE (output.str () == "[ 5 10 44 ]");
}

/*----------------------------------------------------------*/

TEST_CASE ("iter dereference [%3]", "[iter]")
{
  List<int> list { 3, 10, 44 };

  SECTION ("iter dereference - non-const [%1.5]")
  {
    List<int>::iterator iter = list.begin();
    advance(iter, 1);
    REQUIRE (* iter == 10);
  }

  SECTION ("iter dereference - const [%1.5]")
  {
    List<int>::const_iterator iter = list.begin();
    advance(iter, 1);
    REQUIRE (* iter == 10);
  }
}

/*----------------------------------------------------------*/

TEST_CASE ("iter arrow operator [%3]", "[iter]")
{
  List<int> list_a { 1, 2, 3 };
  List<int> list_b { 4, 5, 6 };
  List<int> list_c { 7, 8, 8 };

  List<List<int>> list_of_lists { list_a, list_b, list_c };

  SECTION ("iter arrow operator - non-const [%1.5]")
  {
    List<List<int>>::iterator iter = list_of_lists.begin();
    advance(iter,1);
    REQUIRE (iter -> back() == 6);
  }

  SECTION ("iter arrow operator - const [%1.5]")
  {
    List<List<int>>::const_iterator iter = list_of_lists.begin();
    advance(iter,1);
    REQUIRE (iter -> back() == 6);
  }
}

/*----------------------------------------------------------*/
/************************************************************/
