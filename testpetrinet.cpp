/* The MIT License (MIT)
 * 
 * Copyright (c) 2015 Pieter Verberck

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 **/

#include <cassert>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>

#include <boost/test/unit_test.hpp>

#include "algorithm.h"
#include "petrinet.h"
#include "sharedtreepointer.h"
#include "token.h"

using namespace petrinet;

class MyToken : public Token
{
public:
  MyToken(int* i = 0, int* j = 0, int* k = 0, int* l = 0) 
    : i_(i), j_(j), k_(k), l_(l), target_(10, 0)
  {
  }
  ~MyToken(){}

  void action1() { *i_ = 1; }
  void action2() { *j_ = 2; }
  void action3() { *k_ = 3; }
  void action4() { *l_ = 4; }
  void action5(int i) { *i_ = i; }
  void action6(int* target, int v)   { *target = v; }
  void action7(int* target, double v1, float v2) { *target = v1 + v2; }
  void actionArray(int i) { target_[i] += 1; }

  int* i_;
  int* j_;
  int* k_;
  int* l_;
  std::vector<int> target_;
};

BOOST_AUTO_TEST_CASE(testSingleTransition)
{
  PetriNet p;
  p.createPlace(1);
  p.createPlace(2);
  p.createTransition(1, std::list<int>(1, 1), std::list<int>(1, 2), &MyToken::action1);

  int target = 0;
  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyToken(&target, 0));
  p.addToken(1, token);

  p.queueFire(1, token);

  BOOST_TEST(target == 1);
}

BOOST_AUTO_TEST_CASE(testDoubleTransition)
{
  PetriNet p;
  p.createPlace(1);
  p.createPlace(2);
  p.createPlace(3);
  p.createTransition(1, std::list<int>(1, 1), std::list<int>(1, 2), &MyToken::action1);
  p.createTransition(2, std::list<int>(1, 2), std::list<int>(1, 3), &MyToken::action2);

  int target1 = 0;
  int target2 = 0;
  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyToken(&target1, &target2));
  p.addToken(1, token);

  p.queueFire(2, token);
  BOOST_TEST(target1 == 0);
  p.queueFire(1, token);

  BOOST_TEST(target1 == 1);
  BOOST_TEST(target2 == 2);
}

BOOST_AUTO_TEST_CASE(testTripleTransition)
{
  PetriNet p;
  p.createPlace(1);
  p.createPlace(2);
  p.createPlace(3);
  p.createPlace(4);
  p.createPlace(5);
  p.createTransition(1, std::list<int>(1, 1), std::list<int>(1, 2), &MyToken::action1);
  p.createTransition(2, std::list<int>(1, 2), std::list<int>(1, 3), &MyToken::action2);
  p.createTransition(3, std::list<int>(1, 3), std::list<int>(1, 4), &MyToken::action3);
  p.createTransition(4, std::list<int>(1, 4), std::list<int>(1, 5), &MyToken::action4);

  int target1 = 0;
  int target2 = 0;
  int target3 = 0;
  int target4 = 0;
  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyToken(&target1, &target2, &target3, &target4));
  p.addToken(1, token);

  
  p.queueFire(2, token);
  p.queueFire(4, token);
  BOOST_TEST(target1 == 0);
  p.queueFire(1, token);

  BOOST_TEST(target1 == 1);
  BOOST_TEST(target2 == 2);
  BOOST_TEST(target3 == 0);
  BOOST_TEST(target4 == 0);
}

BOOST_AUTO_TEST_CASE(testTransitions1)
{
  PetriNet p;
  p.createPlace(1);
  p.createPlace(2);
  p.createPlace(3);
  p.createPlace(4);
  p.createPlace(5);
  p.createTransition(1, std::list<int>(1, 1), std::list<int>(1, 2), &MyToken::action1);
  p.createTransition(2, std::list<int>(1, 2), std::list<int>(1, 3), &MyToken::action2);
  p.createTransition(3, std::list<int>(1, 3), std::list<int>(1, 4), &MyToken::action3);
  p.createTransition(4, std::list<int>(1, 4), std::list<int>(1, 5), &MyToken::action4);

  int target1 = 0;
  int target2 = 0;
  int target3 = 0;
  int target4 = 0;
  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyToken(&target1, &target2, &target3, &target4));
  p.addToken(1, token);

  
  p.queueFire(2, token);
  p.queueFire(3, token);
  p.queueFire(4, token);
  BOOST_TEST(target1 == 0);
  p.queueFire(1, token);

  BOOST_TEST(target1 == 1); 
  BOOST_TEST(target2 == 2);
  BOOST_TEST(target3 == 3);
  BOOST_TEST(target4 == 4);
}

BOOST_AUTO_TEST_CASE(testSpareCapacity)
{
  PetriNet p;
  p.createPlace(1);
  p.createPlace(2, 1);
  p.createPlace(3);

  p.createTransition(1, {1}, {2});
  p.createTransition(2, {2}, {3});

  SharedTreePointer<Token> token1 = SharedTreePointer<Token>(new MyToken());
  SharedTreePointer<Token> token2 = SharedTreePointer<Token>(new MyToken());

  p.addToken(1, token1);
  p.addToken(1, token2);
  p.queueFire(1, token1);
  p.queueFire(1, token2);
  BOOST_TEST(p.tokens(2).size() == 1);
  p.queueFire(2, token2);
  p.queueFire(2, token1);
  BOOST_TEST(p.tokens(1).size() == 0);
  BOOST_TEST(p.tokens(2).size() == 0);
  BOOST_TEST(p.tokens(3).size() == 2);
}

BOOST_AUTO_TEST_CASE(testDoubleToken)
{
  PetriNet p;
  p.createPlace(1);
  p.createPlace(2);
  p.createPlace(3);
  p.createTransition(1, {1}, {2});
  p.createTransition(2, {2}, {3});

  int target1 = 0;
  int target2 = 0;
  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyToken());
  p.addToken(1, token);
  p.addToken(1, token);

  p.queueFire(2, token);
  p.queueFire(2, token);
  BOOST_TEST(target1 == 0);
  p.queueFire(1, token);
  p.queueFire(1, token);

  BOOST_TEST(p.tokens(3).size() == 2);
}

BOOST_AUTO_TEST_CASE(testFireParameter0)
{
  PetriNet p;
  p.createPlace(1);
  p.createPlace(2);
  p.createTransition(1, std::list<int>(1, 1), std::list<int>(1, 2));

  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyToken(0, 0));
  p.addToken(1, token);

  p.queueFire(1, token);
}


BOOST_AUTO_TEST_CASE(testFireParameter1)
{
  PetriNet p;
  p.createPlace(1);
  p.createPlace(2);
  p.createTransition(1, std::list<int>(1, 1), std::list<int>(1, 2), &MyToken::action5);

  int target = 0;
  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyToken(&target, 0));
  p.addToken(1, token);

  p.queueFire(1, token, 5);

  BOOST_TEST(target == 5);
}

BOOST_AUTO_TEST_CASE(testFireParameter2)
{
  PetriNet p;
  p.createPlace(1);
  p.createPlace(2);
  p.createTransition(1, std::list<int>(1, 1), std::list<int>(1, 2), &MyToken::action6);

  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyToken(0, 0));
  p.addToken(1, token);

  int target = 0;
  p.queueFire(1, token, &target, 7);

  BOOST_TEST(target == 7);
}


BOOST_AUTO_TEST_CASE(testFireParameter3)
{
  PetriNet p;
  p.createPlace(1);
  p.createPlace(2);
  p.createTransition(1, std::list<int>(1, 1), std::list<int>(1, 2), &MyToken::action7);

  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyToken(0, 0));
  p.addToken(1, token);

  int target = 0;
  p.queueFire(1, token, &target, 3.5, 1.5f);

  BOOST_TEST(target == 5);
}

BOOST_AUTO_TEST_CASE(testFireParameter3Queued)
{
  PetriNet p;
  p.createPlace(1);
  p.createPlace(2);
  p.createPlace(3);
  p.createTransition(1, {1}, {2});
  p.createTransition(2, {2}, {3}, &MyToken::action7);

  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyToken());
  p.addToken(1, token);

  int target = 0;
  p.queueFire(2, token, &target, 3.5, 1.5f);
  p.queueFire(1, token);

  BOOST_TEST(target == 5);
}


BOOST_AUTO_TEST_CASE(testComplexPetriNet)
{

  PetriNet p;
  p.createPlace(1);
  p.createPlace(2);
  p.createPlace(3);
  p.createPlace(4);
  p.createPlace(5);
  p.createPlace(6);
  p.createPlace(7);
  p.createPlace(8);
  p.createPlace(9);

  p.createTransition(1, {},      {1},       &MyToken::actionArray);
  p.createTransition(2, {1,9},   {2,3,4},   &MyToken::actionArray);
  p.createTransition(3, {2},     {5},       &MyToken::actionArray);
  p.createTransition(4, {3},     {6},       &MyToken::actionArray);
  p.createTransition(5, {4},     {7},       &MyToken::actionArray);
  p.createTransition(6, {5,6,7}, {8,9},     &MyToken::actionArray);

  MyToken* m = new MyToken();
  SharedTreePointer<Token> token = SharedTreePointer<Token>(m);
  p.addToken(9, token);
  p.queueFire(2, token, 2);
  p.queueFire(3, token, 3);
  p.queueFire(4, token, 4);
  p.queueFire(6, token, 6);

  BOOST_TEST(m->target_[1] == 0);
  BOOST_TEST(m->target_[2] == 0);

  p.queueFire(1, token, 1);
  BOOST_TEST(m->target_[3] == 1);
  p.queueFire(5, token, 5);
  BOOST_TEST(m->target_[6] == 1);

  BOOST_TEST(p.tokens(1).size() == 0);
  BOOST_TEST(p.tokens(7).size() == 0);
  BOOST_TEST(p.tokens(8).size() == 1);
  BOOST_TEST(p.tokens(9).size() == 1);

  int target = 0;
  //p.queueFire(1, token, &target, 3.5, 1.5f);

  //BOOST_TEST(target == 5);
}

BOOST_AUTO_TEST_CASE(testDoubleTokens)
{
  PetriNet p;
  p.createPlace(1);
  p.createPlace(2);

  p.createTransition(1, {},      {1, 1});
  p.createTransition(2, {1},     {2});

  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyToken());

  p.queueFire(1, token);
  BOOST_TEST(p.tokens(1).size() == 2);
  p.queueFire(2, token);
  BOOST_TEST(p.tokens(1).size() == 1);
  BOOST_TEST(p.tokens(2).size() == 1);
  p.queueFire(2, token);
  BOOST_TEST(p.tokens(1).size() == 0);
  BOOST_TEST(p.tokens(2).size() == 2);
}


class MyTokenLvl2 : public Token
{
public:
  MyTokenLvl2(){}
};

class MyTokenLvl1 : public Token
{
public:
  MyTokenLvl1()
  {
    children_.push_back(std::shared_ptr<Token>(new MyTokenLvl2()));
    children_.push_back(std::shared_ptr<Token>(new MyTokenLvl2()));
    static int idGenerator = 0;
    id_ = idGenerator++;
  }

  virtual int size() const { return children_.size(); }
  virtual Token* child(int i) const { return children_.at(i).get(); }

  void action(int* target, int value) { *target += value; }

  virtual std::string toString() const { return std::string("MyTokenLvl1 ") + std::to_string(id_); }

private:
  std::vector<std::shared_ptr<Token>> children_;
  int id_;
};

BOOST_AUTO_TEST_CASE(testSharedTreeChild)
{
  SharedTreePointer<Token> ptr1(new MyTokenLvl1());
  SharedTreePointer<Token> ptr2 = ptr1.child(0);

  SharedTreePointer<Token> ptr3 = ptr1.child(0);

  BOOST_TEST(ptr3.get<MyTokenLvl2>());
}


BOOST_AUTO_TEST_CASE(testPlaceLevel2)
{
  PetriNet p;
  p.createPlace(1);
  p.createPlace(2, 2, 2);

  p.createTransition(1, {1}, {2}, &MyTokenLvl1::action);
  
  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyTokenLvl1());

  int target = 0;
  p.addToken (1, token);
  p.queueFire(1, token, &target, 3);
  
  BOOST_TEST(p.tokens(1).size() == 0);
  BOOST_TEST(p.tokens(2).size() == 2);
  BOOST_TEST(contains(p.tokens(2), token.child(0)));
  BOOST_TEST(contains(p.tokens(2), token.child(1)));
  BOOST_TEST(target == 3);
}


BOOST_AUTO_TEST_CASE(testLevelUp)
{
  PetriNet p;
  p.createPlace(1, -1, 2);
  p.createPlace(2, 2);

  int target = 0;
  p.createTransition(1, {1}, {2}, &MyTokenLvl1::action);
  
  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyTokenLvl1());

  p.addToken(1, token.child(0));
  p.addToken(1, token.child(1));
  p.queueFire(1, token, &target, 5);
  
  BOOST_TEST(p.tokens(1).size() == 0);
  BOOST_TEST(p.tokens(2).size() == 1);
  BOOST_TEST(contains(p.tokens(2), token));
  BOOST_TEST(target == 5);
}

BOOST_AUTO_TEST_CASE(testPerformance)
{
#ifdef _DEBUG
  const int amountOfTests = 30;
#else
  const int amountOfTests = 30000;
#endif

  PetriNet p;
  p.createPlace(1);
  p.createPlace(2, 3);
  p.reserve(amountOfTests * 2);

  int target = 0;
  p.createTransition(1, {},  {1});
  p.createTransition(2, {1}, {2}, &MyTokenLvl1::action);
  p.createTransition(3, {2}, {});

  std::vector<SharedTreePointer<Token>> tokens;
  for(int i = 0; i < amountOfTests; ++i)
    tokens.push_back(SharedTreePointer<Token>(new MyTokenLvl1()));

  std::clock_t c_start = std::clock();
  auto t_start = std::chrono::high_resolution_clock::now();

  std::for_each(tokens.begin(), tokens.end(), std::bind(&PetriNet::queueFire, &p, 2, std::placeholders::_1, &target, 1, boost::any()));
  std::for_each(tokens.begin(), tokens.end(), std::bind(&PetriNet::queueFire, &p, 3, std::placeholders::_1, boost::any(), boost::any(), boost::any()));

  std::clock_t c_end = std::clock();
  auto t_end = std::chrono::high_resolution_clock::now();

  std::cerr << std::fixed << std::setprecision(2) << "CPU time used: "
              << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms, for setup\n";

  c_start = std::clock();
  t_start = std::chrono::high_resolution_clock::now();

  std::for_each(tokens.begin(), tokens.end(), std::bind(&PetriNet::queueFire, &p, 1, std::placeholders::_1, boost::any(), boost::any(), boost::any()));

  c_end = std::clock();
  t_end = std::chrono::high_resolution_clock::now();

  std::cerr << std::fixed << std::setprecision(2) << "CPU time used: "
              << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms, for "
              << amountOfTests << " signals\n";

  BOOST_TEST(target == amountOfTests);
}


BOOST_AUTO_TEST_CASE(testPerformanceMaxCapacity)
{
  
#ifdef _DEBUG
  const int amountOfTests = 300;
  const int dataFillingSize = 30;
#else
  const int amountOfTests = 30000;
  const int dataFillingSize = 30000;
#endif

  PetriNet p;
  p.createPlace(1);
  p.createPlace(2, 3);
  p.createPlace(9);
  p.reserve(dataFillingSize * 3);

  int target = 0;
  p.createTransition(1, {},  {1});
  p.createTransition(2, {1}, {2});
  p.createTransition(3, {2}, {}, &MyTokenLvl1::action);
  p.createTransition(8, {9}, {});

  std::vector<SharedTreePointer<Token>> tokensDataFilling;
  for(int i = 0; i < dataFillingSize; ++i)
    tokensDataFilling.push_back(SharedTreePointer<Token>(new MyTokenLvl1()));

  std::vector<SharedTreePointer<Token>> tokens;
  for(int i = 0; i < amountOfTests; ++i)
    tokens.push_back(SharedTreePointer<Token>(new MyTokenLvl1()));

  std::clock_t c_start = std::clock();
  auto t_start = std::chrono::high_resolution_clock::now();

  std::for_each(tokensDataFilling.begin(), tokensDataFilling.end(), std::bind(&PetriNet::queueFire, &p, 8, std::placeholders::_1, boost::any(), boost::any(), boost::any()));
  std::for_each(tokens.begin(),            tokens.end(),            std::bind(&PetriNet::queueFire, &p, 1, std::placeholders::_1, boost::any(), boost::any(), boost::any()));
  std::for_each(tokens.begin(),            tokens.end(),            std::bind(&PetriNet::queueFire, &p, 2, std::placeholders::_1, boost::any(), boost::any(), boost::any()));

  std::clock_t c_end = std::clock();
  auto t_end = std::chrono::high_resolution_clock::now();

  std::cerr << std::fixed << std::setprecision(2) << "CPU time used: "
              << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms, for setup\n";

  c_start = std::clock();
  t_start = std::chrono::high_resolution_clock::now();

  std::for_each(tokens.begin(), tokens.end(), std::bind(&PetriNet::queueFire, &p, 3, std::placeholders::_1, &target, 1, boost::any()));

  c_end = std::clock();
  t_end = std::chrono::high_resolution_clock::now();

  std::cerr << std::fixed << std::setprecision(2) << "CPU time used: "
              << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms, for "
              << amountOfTests << " signals\n";

  BOOST_TEST(target == amountOfTests);
}


BOOST_AUTO_TEST_CASE(testLoop)
{
  PetriNet p;
  p.createPlace(1);
  p.createTransition(1, {1}, {1});

  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyToken(0, 0));
  p.queueFire(1, token);
  p.addToken(1, token);
  p.queueFire(1, token);
}

BOOST_AUTO_TEST_CASE(testDoubleArcMaxCap)
{
  PetriNet p;
  p.createPlace(1);
  p.createPlace(2, 3);
  p.createTransition(1, {1}, {2, 2});

  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyToken(0, 0));
  p.addToken(1, token);
  p.addToken(1, token);
  p.queueFire(1, token);
  p.queueFire(1, token);
  BOOST_TEST(p.tokens(1).size() == 1);
  BOOST_TEST(p.tokens(2).size() == 2);
}

BOOST_AUTO_TEST_CASE(testDoubleArcLevelDownMaxCap)
{
  PetriNet p;
  p.createPlace(1);
  p.createPlace(2, 5, 2);
  p.createTransition(1, {1}, {2, 2});

  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyTokenLvl1());
  p.addToken(1, token);
  p.addToken(1, token);
  p.queueFire(1, token);
  p.queueFire(1, token);
  BOOST_TEST(p.tokens(1).size() == 1);
  BOOST_TEST(p.tokens(2).size() == 4);
}

BOOST_AUTO_TEST_CASE(testDoubleArcFewTokens)
{
  PetriNet p;
  p.createPlace(1);
  p.createPlace(2);
  p.createTransition(1, {1, 1}, {2});

  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyToken(0, 0));
  p.addToken(1, token);
  p.queueFire(1, token);
  p.addToken(1, token);
  p.addToken(1, token);
  p.queueFire(1, token);
  
  BOOST_TEST(p.tokens(1).size() == 1);
  BOOST_TEST(p.tokens(2).size() == 1);
}

BOOST_AUTO_TEST_CASE(testDoubleArcLevelUpFewTokens)
{
  PetriNet p;
  p.createPlace(1, -1, 2);
  p.createPlace(2);
  p.createTransition(1, {1, 1}, {2});

  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyTokenLvl1());
  SharedTreePointer<Token> tokenC1 = token.child(0);
  SharedTreePointer<Token> tokenC2 = token.child(1);

  p.addToken(1, tokenC1);
  p.addToken(1, tokenC1);
  p.addToken(1, tokenC2);
  p.queueFire(1, token);
  p.addToken(1, tokenC2);
  p.addToken(1, tokenC1);
  p.addToken(1, tokenC2);
  p.queueFire(1, token);

  BOOST_TEST(p.tokens(1).size() == 2);
  BOOST_TEST(p.tokens(2).size() == 1);
}


class MyTokenMultiLevel : public Token
{
public:
  virtual int size() const { return children_.size(); }
  virtual Token* child(int i) const { return children_.at(i).get(); }
  std::vector<std::shared_ptr<Token>> children_;
};

void createMultiLevel(SharedTreePointer<Token> parent, std::list<int> childCount)
{
  int c = childCount.front();
  childCount.pop_front();

  for(int i = 0; i < c; ++i)
  {
    parent.get<MyTokenMultiLevel>()->children_.push_back(std::shared_ptr<MyTokenMultiLevel>(new MyTokenMultiLevel()));
    if(childCount.size())
      createMultiLevel(parent.child(i), childCount);
  }
}

BOOST_AUTO_TEST_CASE(testSimultaneousUpDown)
{
  SharedTreePointer<Token> token = SharedTreePointer<Token>(new MyTokenMultiLevel());
  createMultiLevel(token, {2, 1, 2, 1, 2});

  PetriNet p;
  p.createPlace(1, -1, 4);
  p.createPlace(2, -1, 2);
  p.createPlace(3, -1, 6);
  p.createTransition(1, {1}, {2, 3});

  for(auto it = token.begin(4); it != token.end(4); ++it)
  {
    p.addToken(1, *it);
  }

  p.queueFire(1, token);

  BOOST_TEST(p.tokens(2).size() == 2);
  BOOST_TEST(p.tokens(3).size() == 8);
}