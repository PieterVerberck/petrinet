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

#include "place.h"

#include <cassert>

using namespace petrinet;

Place::Place()
  :capacity_(-1),
  level_(1)
{
}

Place::Place(const Place& p)
  : capacity_(p.capacity_),
  level_(p.level_)
{
}

Place::Place(int capacity, int level)
  : capacity_(capacity),
  level_(level)
{}

Place::~Place()
{
}

int Place::capacity() const
{
  return capacity_;
}

int Place::level() const
{
  return level_;
}

Place& Place::operator=(const Place& p)
{
  capacity_ = p.capacity();
  level_ = p.level();
  return *this;
}

bool Place::hasCapacityLeft(int i) const
{
  assert(capacity_ == -1 || i <= capacity_);
  return capacity_ == -1 || capacity_ >= tokens_.size() + i;
}

void Place::putToken(SharedTreePointer<Token> token)
{
  assert(hasCapacityLeft());

  tokens_.insert(token);
}

void Place::takeToken(SharedTreePointer<Token> token)
{
  auto it = tokens_.find(token);
  assert(it != tokens_.end());
  tokens_.erase(it);
}

