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

#ifndef PETRINET_FIRE_H
#define PETRINET_FIRE_H

#include <boost/any.hpp>
#include "sharedtreepointer.h"

class Token;

struct FireData
{
  boost::any a_;
  boost::any b_;
  boost::any c_;
  SharedTreePointer<Token> token_;
  int transitionId_;
};

class Fire
{
public:
  Fire();
  Fire(int transitionId, SharedTreePointer<Token> token, boost::any a = boost::any(), boost::any b = boost::any(), boost::any c = boost::any());
  virtual ~Fire();

  bool operator==(const Fire& f) const;

  SharedTreePointer<Token> token() const;
  int transitionId() const;

  boost::any a() const { return data_->a_; }
  boost::any b() const { return data_->b_; }
  boost::any c() const { return data_->c_; }

  std::string toString() const;

private:
  std::shared_ptr<FireData> data_;

  
};

namespace std
{
  template<>
  struct hash<Fire>
  {
    std::size_t operator()(const Fire& s) const
    {
      intmax_t h = (intmax_t)s.token().get();
      h = h << 8;
      h += s.transitionId();
      return h * 2654435761 % (long)std::pow(2.0, 32.0);
    }
  };
};

#endif