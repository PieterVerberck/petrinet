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

#include <vector>
#include <boost/test/unit_test.hpp>
#include "sharedtreepointer.h"

using namespace petrinet;

class SimpleToken
{
public:
  virtual int size() const { return children_.size(); }
  virtual SimpleToken* child(int i) const { return children_.at(i).get(); }

  ~SimpleToken(){}

  std::vector<std::shared_ptr<SimpleToken>> children_;
};

BOOST_AUTO_TEST_CASE(testBreadthFirstIteration)
{
  SimpleToken *a, *b, *c, *d, *e, *f;
  SimpleToken* s = new SimpleToken();
  s->children_.push_back(std::shared_ptr<SimpleToken>(a = new SimpleToken()));
  s->children_.back()->children_.push_back(std::shared_ptr<SimpleToken>(d = new SimpleToken()));
  s->children_.back()->children_.push_back(std::shared_ptr<SimpleToken>(e = new SimpleToken()));
  s->children_.push_back(std::shared_ptr<SimpleToken>(b = new SimpleToken()));
  s->children_.push_back(std::shared_ptr<SimpleToken>(c = new SimpleToken()));
  s->children_.back()->children_.push_back(std::shared_ptr<SimpleToken>(f = new SimpleToken()));

  SharedTreePointer<SimpleToken> myToken = SharedTreePointer<SimpleToken>(s);

  SharedTreePointer<SimpleToken>::iterator iter = myToken.begin();
  BOOST_TEST((*iter).get() == s);
  ++iter;
  BOOST_TEST((*iter).get() == a);
  ++iter;
  BOOST_TEST((*iter).get() == b);
  ++iter;
  BOOST_TEST((*iter).get() == c);
  ++iter;
  BOOST_TEST((*iter).get() == d);
  ++iter;
  BOOST_TEST((*iter).get() == e);
  ++iter;
  BOOST_TEST((*iter).get() == f);
  ++iter;

  BOOST_TEST((*myToken.begin(2)).get() == a);
  BOOST_TEST((*myToken.end(2)).get() == d);
  BOOST_VERIFY(myToken.end() == iter);
}