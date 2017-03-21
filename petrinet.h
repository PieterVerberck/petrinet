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

#ifndef PETRINET_PETRINET_H
#define PETRINET_PETRINET_H

#include <functional>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/pool/pool_alloc.hpp>

#include "fire.h"
#include "logger.h"
#include "place.h"
#include "sharedtreepointer.h"
#include "transition.h"

class Token;

class PetriNetImpl
{
public:
  PetriNetImpl();
  bool canFire(const Fire& f) const;
  void fire(const Fire& f);
  std::vector<Fire> searchNextPossibleFires(const Fire& f) const;
  void queueFire(const Fire& f);
  void reserve(int size);

private:
  friend class PetriNet;

  std::map<int, int> getPlaceCounts() const;
  std::map<int, int> getTransitionCounts() const;

  std::unordered_map<int, Place> places_;
  std::unordered_map<int, Transition> transitions_;
  std::unordered_map<int, std::unordered_multimap<Token*, Fire>> queuedFires_;
  
  Logger logger_;
};

// TODO logging
// TODO documentation, clean code
// TODO maybe queueFire with different performance strategies?
// TODO maybe parallel work, task based threading?

class PetriNet
{
public:
  PetriNet();
  virtual ~PetriNet();

  void createPlace     (int placeId, int capacity = -1, int level = 1);
  
  void createTransition(int transitionId, const std::list<int>& inputPlaces, const std::list<int>& outputPlaces);
  template<class T>
  void createTransition(int transitionId, const std::list<int>& inputPlaces, const std::list<int>& outputPlaces, void (T::* func)());
  template<class T, typename A>
  void createTransition(int transitionId, const std::list<int>& inputPlaces, const std::list<int>& outputPlaces, void (T::* func)(A));
  template<class T, typename A, typename B>
  void createTransition(int transitionId, const std::list<int>& inputPlaces, const std::list<int>& outputPlaces, void (T::* func)(A, B));
  template<class T, typename A, typename B, typename C>
  void createTransition(int transitionId, const std::list<int>& inputPlaces, const std::list<int>& outputPlaces, void (T::* func)(A, B, C));

  void queueFire       (int transitionId, SharedTreePointer<Token> token, boost::any a = boost::any(), boost::any b = boost::any(), boost::any c = boost::any());
  void addToken        (int placeId,      SharedTreePointer<Token> token);

  const std::unordered_multiset<SharedTreePointer<Token>>& tokens(int placeId) const;

  void reserve(int size);

private:
  PetriNetImpl impl_;
};



template<typename T>
void downCastAndExec(void (T::* func)(), Token* p)
{
  ((*dynamic_cast<T*>(p)).*func)();
}

template<typename T>
void PetriNet::createTransition(int transitionId, const std::list<int>& inputPlaces, const std::list<int>& outputPlaces, void (T::* func)() )
{
  impl_.transitions_.insert(std::pair<int, Transition>(transitionId, Transition(inputPlaces, outputPlaces, std::bind(&downCastAndExec<T>, func, std::placeholders::_1))));
}

template<typename T, typename A>
void downCastAndExec(void (T::* func)(A), Token* p, boost::any a)
{
  ((*dynamic_cast<T*>(p)).*func)(boost::any_cast<A>(a));
}

template<typename T, typename A>
void PetriNet::createTransition(int transitionId, const std::list<int>& inputPlaces, const std::list<int>& outputPlaces, void (T::* func)(A) )
{
  impl_.transitions_.insert(std::pair<int, Transition>(transitionId, Transition(inputPlaces, outputPlaces, std::bind(&downCastAndExec<T, A>, func, std::placeholders::_1, std::placeholders::_2))));
}

template<typename T, typename A, typename B>
void downCastAndExec(void (T::* func)(A, B), Token* p, boost::any a, boost::any b)
{
  ((*dynamic_cast<T*>(p)).*func)(boost::any_cast<A>(a), boost::any_cast<B>(b));
}

template<typename T, typename A, typename B>
void PetriNet::createTransition(int transitionId, const std::list<int>& inputPlaces, const std::list<int>& outputPlaces, void (T::* func)(A, B) )
{
  impl_.transitions_.insert(std::pair<int, Transition>(transitionId, Transition(inputPlaces, outputPlaces, std::bind(&downCastAndExec<T, A, B>, func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))));
}

template<typename T, typename A, typename B, typename C>
void downCastAndExec(void (T::* func)(A, B, C), Token* p, boost::any a, boost::any b, boost::any c)
{
  ((*dynamic_cast<T*>(p)).*func)(boost::any_cast<A>(a), boost::any_cast<B>(b), boost::any_cast<C>(c));
}

template<typename T, typename A, typename B, typename C>
void PetriNet::createTransition(int transitionId, const std::list<int>& inputPlaces, const std::list<int>& outputPlaces, void (T::* func)(A, B, C) )
{
  impl_.transitions_.insert(std::pair<int, Transition>(transitionId, Transition(inputPlaces, outputPlaces, std::bind(&downCastAndExec<T, A, B, C>, func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4))));
}

#endif
