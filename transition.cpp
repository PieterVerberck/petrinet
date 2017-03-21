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

#include "transition.h"


Transition::Transition(const std::list<int>& inputPlaces, const std::list<int>& outputPlaces, std::function<void(Token*, boost::any, boost::any, boost::any)> func)
  : inputPlaces_(inputPlaces),
  outputPlaces_(outputPlaces),
  func_(func)
{
  for(auto it = inputPlaces_.begin(); it != inputPlaces_.end(); ++it)
  {
    if(inputPlacesCount_.find(*it) != inputPlacesCount_.end())
      ++inputPlacesCount_[*it];
    else
      inputPlacesCount_.insert(std::pair<int, int>(*it, 1));
  }

  for(auto it = outputPlaces_.begin(); it != outputPlaces_.end(); ++it)
  {
    if(outputPlacesCount_.find(*it) != outputPlacesCount_.end())
      ++outputPlacesCount_[*it];
    else
      outputPlacesCount_.insert(std::pair<int, int>(*it, 1));
  }
}


Transition::~Transition()
{
}

int Transition::requiredTokens(int placeId) const
{
  assert(inputPlacesCount_.find(placeId) != inputPlacesCount_.end());
  return inputPlacesCount_.find(placeId)->second;
}

int Transition::requiredCapacity(int placeId) const
{
  assert(outputPlacesCount_.find(placeId) != outputPlacesCount_.end());
  return outputPlacesCount_.find(placeId)->second;
}


