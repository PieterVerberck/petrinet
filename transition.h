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

#ifndef PETRINET_TRANSITION_H
#define PETRINET_TRANSITION_H

#include <boost/any.hpp>
#include <functional>
#include <list>
#include <unordered_map>

#include "sharedtreepointer.h"

class Token;

class Transition
{
public:
  Transition(const std::list<int>& inputPlaces, const std::list<int>& outputPlaces, std::function<void(Token*, boost::any, boost::any, boost::any)> func);
  virtual ~Transition();

  std::list<int>              inputPlaces()    const { return inputPlaces_; }
  std::list<int>              outputPlaces()   const { return outputPlaces_; }
  std::function<void(Token*, boost::any, boost::any, boost::any)> func() const { return func_; }

  int requiredTokens  (int placeId) const;
  int requiredCapacity(int placeId) const;

private:
  std::list<int> inputPlaces_;
  std::list<int> outputPlaces_;
  std::function<void(Token*, boost::any, boost::any, boost::any)> func_;

  std::unordered_map<int, int> inputPlacesCount_;
  std::unordered_map<int, int> outputPlacesCount_;
};

#endif