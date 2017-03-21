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

#ifndef PETRINET_PLACE_H
#define PETRINET_PLACE_H

#include <unordered_set>

#include "sharedtreepointer.h"

class Token;

class Place
{
public:
  Place();
  Place(const Place& p);
  Place(int capacity, int level = 1);
  virtual ~Place();

  Place& operator=(const Place& p);

  int capacity() const;
  int level() const;

  bool hasCapacityLeft(int i = 1) const;

  const std::unordered_multiset<SharedTreePointer<Token>>& tokens() const { return tokens_; }

  void putToken (SharedTreePointer<Token> token);
  void takeToken(SharedTreePointer<Token> token);

private:
  int capacity_;
  int level_;
  std::unordered_multiset<SharedTreePointer<Token>> tokens_;
};

#endif