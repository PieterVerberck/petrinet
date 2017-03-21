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

#include "fire.h"
#include "token.h"

using namespace petrinet;

Fire::Fire()
{
}

Fire::Fire(int transitionId, SharedTreePointer<Token> token, boost::any a, boost::any b, boost::any c)
  : data_(new FireData())
{
  data_->transitionId_ = transitionId;
  data_->token_ = token;
  data_->a_ = a;
  data_->b_ = b;
  data_->c_ = c;
}


Fire::~Fire()
{
}

bool Fire::operator==(const Fire& f) const
{
  return f.data_->token_ == data_->token_ && f.data_->transitionId_ == data_->transitionId_;
}

SharedTreePointer<Token> Fire::token() const
{
  return data_->token_;
}

int Fire::transitionId() const
{
  return data_->transitionId_;
}

std::string Fire::toString() const
{
  return std::string("Fire(") 
         + std::string("T") + std::to_string(transitionId()) + ", "
         + token().get()->toString()
         + ")";
}

