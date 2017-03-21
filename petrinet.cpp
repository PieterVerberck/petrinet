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

#include <algorithm>
#include <iostream>

#include "algorithm.h"
#include "petrinet.h"
#include "token.h"

using namespace petrinet;

std::string countsToString(const std::map<int, int>& placeCounts)
{
  std::string s;
  for(auto it = placeCounts.begin(); it != placeCounts.end();)
  {
    s += std::to_string(it->first) + ":" + std::to_string(it->second);
    if(++it != placeCounts.end())
      s += ", ";
  }
  return s;
}

class LogPetriNetImplQueueFire
{
  public:
    LogPetriNetImplQueueFire(const Fire& toFire, const std::map<int, int>& placeCounts, const std::map<int, int>& transitionCounts, bool canFire)
      : fire_(toFire),
      placeCounts_(placeCounts),
      transitionCounts_(transitionCounts),
      canFire_(canFire){}

    std::string operator()()
    {
      return std::string("PetriNetImpl::queueFire(") + fire_.toString() + ") "
             + "with Places(" + countsToString(placeCounts_) + ") "
             + "and Transitions(" + countsToString(transitionCounts_) + ") "
             + (canFire_ ? "can fire" : "cannot fire");
    }

  private:
    Fire fire_;
    std::map<int, int> placeCounts_;
    std::map<int, int> transitionCounts_;
    bool canFire_;
};

class LogPetriNetImplFire
{
  public:
    LogPetriNetImplFire(const Fire& toFire, const std::map<int, int>& placeCounts, const std::map<int, int>& transitionCounts)
      : fire_(toFire), placeCounts_(placeCounts), transitionCounts_(transitionCounts){}

    std::string operator()()
    {
      return std::string("PetriNetImpl::fire(") + fire_.toString() + ")"
             + "with PetriNet(" + countsToString(placeCounts_) + ") "
             + "and Transitions(" + countsToString(transitionCounts_) + ")";
    }

  private:
    Fire fire_;
    std::map<int, int> placeCounts_;
    std::map<int, int> transitionCounts_;
};

bool basicCompare(int a, int b) { return a == b; }

PetriNetImpl::PetriNetImpl()
  : logger_(new FileLogControl("PetriNetImpl"))
{
}

void PetriNetImpl::queueFire(const Fire& f)
{
  if(!canFire(f))
  {
    logger_.log(LogPetriNetImplQueueFire(f, getPlaceCounts(), getTransitionCounts(), false));
    queuedFires_[f.transitionId()].insert(std::pair<Token*, Fire>(f.token().get(), f));
    return;
  }

  logger_.log(LogPetriNetImplQueueFire(f, getPlaceCounts(), getTransitionCounts(), true));

  

  fire(f);
  Fire oldF = f;
  // Gather all fires that might have become ready
  std::vector<Fire> firesToCheck = searchNextPossibleFires(oldF);
  
  do
  {
    for(auto it = firesToCheck.begin(); it != firesToCheck.end();)
    {
      std::unordered_multimap<Token*, Fire>& queuedFiresForTransition = queuedFires_[it->transitionId()];
      auto fIt = it->token().get() ?
        // Fire to check for specific token
        queuedFiresForTransition.find(it->token().get()) :
        // Fire to check for any token
        queuedFiresForTransition.begin();

      if(fIt == queuedFiresForTransition.end() || !canFire(fIt->second))
        it = firesToCheck.erase(it);
      else
      {
        fire(fIt->second);
        oldF = fIt->second;
        queuedFiresForTransition.erase(fIt);
        it = firesToCheck.erase(it);

        // Gather all fires that might have become ready
        std::vector<Fire> additionalFiresToCheck = searchNextPossibleFires(oldF);
        firesToCheck.insert(firesToCheck.end(), additionalFiresToCheck.begin(), additionalFiresToCheck.end());
        
        break;
      }
    }
  } while(firesToCheck.size());
}

bool PetriNetImpl::canFire(const Fire& f) const
{
  const Transition& tr = transitions_.find(f.transitionId())->second;
  std::list<int> inputPlaces = tr.inputPlaces();
  for(auto it = inputPlaces.begin(); it != inputPlaces.end(); ++it)
  {
    const Place& pl = places_.find(*it)->second;

    // Firing a transition with a token of a deeper level than any of it's linked places
    // is not defined.
    assert(f.token().level() <= pl.level());

    // Performance boost calculation
    if(f.token().level() == pl.level())
    {
      if(pl.tokens().count(f.token()) < tr.requiredTokens(*it))
        return false;
    }
    // Performance boost calculation
    else if(f.token().level()+1 == pl.level())
    {
      for(int i = 0; i < f.token()->size(); ++i)
      {
        if(pl.tokens().count(f.token().child(i)) < tr.requiredTokens(*it))
          return false;
      }
    }
    // Calculates slowly, but can handle all situations
    else
    {
      for(auto childTokenIter = f.token().begin(pl.level()); childTokenIter != f.token().end(pl.level()); ++childTokenIter)
      {
        if(pl.tokens().count(*childTokenIter) < tr.requiredTokens(*it))
          return false;
      }
    }
  }

  std::list<int> outputPlaces = transitions_.find(f.transitionId())->second.outputPlaces();
  for(auto it = outputPlaces.begin(); it != outputPlaces.end(); ++it)
  {
    const Place& pl = places_.find(*it)->second;

    // Firing a transition with a token of a deeper level than any of it's linked places
    // is not defined.
    assert(f.token().level() <= pl.level());

    // Performance boost calculation
    if(f.token().level() == pl.level())
    {
      if(!pl.hasCapacityLeft(tr.requiredCapacity(*it)))
        return false;
    }
    // Performance boost calculation
    else if(f.token().level()+1 == pl.level())
    {
      if(!pl.hasCapacityLeft(f.token()->size() * tr.requiredCapacity(*it)))
        return false;
    }
    // Calculates slowly, but can handle all situations
    else if(!pl.hasCapacityLeft(std::distance(f.token().begin(pl.level()), f.token().end(pl.level()))
                            * tr.requiredCapacity(*it)))
      return false;
  }

  return true;
}

void PetriNetImpl::fire(const Fire& f)
{
  logger_.log(LogPetriNetImplFire(f, getPlaceCounts(), getTransitionCounts()));

  if(transitions_.find(f.transitionId())->second.func())
    transitions_.find(f.transitionId())->second.func()(f.token().get(), f.a(), f.b(), f.c());

  std::list<int> inputPlaces = transitions_.find(f.transitionId())->second.inputPlaces();
  for(auto it = inputPlaces.begin(); it != inputPlaces.end(); ++it)
  {
    const Place& pl = places_.find(*it)->second;

    for(auto childTokenIter = f.token().begin(pl.level()); childTokenIter != f.token().end(pl.level()); ++childTokenIter)
    {
      places_.find(*it)->second.takeToken(*childTokenIter);
    }
  }

  std::list<int> outputPlaces = transitions_.find(f.transitionId())->second.outputPlaces();
  for(auto it = outputPlaces.begin(); it != outputPlaces.end(); ++it)
  {
    const Place& pl = places_.find(*it)->second;

    for(auto childTokenIter = f.token().begin(pl.level()); childTokenIter != f.token().end(pl.level()); ++childTokenIter)
    {
      places_.find(*it)->second.putToken(*childTokenIter);
    }
  }
}

std::vector<Fire> PetriNetImpl::searchNextPossibleFires(const Fire& oldF) const
{
  std::vector<Fire> fires;

  auto oldFoutputPlaces = transitions_.find(oldF.transitionId())->second.outputPlaces();
  auto oldFinputPlaces = transitions_.find(oldF.transitionId())->second.inputPlaces();
  for(auto transitionIt = transitions_.begin(); transitionIt != transitions_.end(); ++transitionIt)
  {
    // Check whether a required place might have received a token
    for(auto placeIt = oldFoutputPlaces.begin(); placeIt != oldFoutputPlaces.end(); ++placeIt)
    {
      if(contains(transitionIt->second.inputPlaces(), *placeIt))
      {
        fires.push_back(Fire(transitionIt->first, oldF.token()));
        break;
      }
    }

    // Check whether a required place might got spare capacity
    for(auto placeIt = oldFinputPlaces.begin(); placeIt != oldFinputPlaces.end(); ++placeIt)
    {
      if(places_.find(*placeIt)->second.capacity() != -1
         && contains(transitionIt->second.outputPlaces(), *placeIt))
      {
        fires.push_back(Fire(transitionIt->first, SharedTreePointer<Token>()));
        break;
      }
    }
  }

  return fires;
}

void PetriNetImpl::reserve(int size)
{
  for(auto it = queuedFires_.begin(); it != queuedFires_.end(); ++it)
    it->second.reserve(size);
}

std::map<int, int> PetriNetImpl::getPlaceCounts() const
{
  std::map<int, int> placeCounts_;
  for(auto it = places_.begin(); it != places_.end(); ++it)
  {
    placeCounts_.insert(std::pair<int, int>(it->first, it->second.tokens().size()));
  }
  return placeCounts_;
}

std::map<int, int> PetriNetImpl::getTransitionCounts() const
{
  std::map<int, int> transitionCounts_;
  for(auto it = queuedFires_.begin(); it != queuedFires_.end(); ++it)
  {
    transitionCounts_.insert(std::pair<int, int>(it->first, it->second.size()));
  }
  return transitionCounts_;
}






PetriNet::PetriNet()
{
}


PetriNet::~PetriNet()
{
}


void PetriNet::createPlace(int placeId, int capacity, int level)
{
  impl_.places_     .insert(std::pair<int, Place>(placeId, Place(capacity, level)));
  impl_.queuedFires_.insert(std::pair<int, std::unordered_multimap<Token*, Fire>>(placeId,std::unordered_multimap<Token*, Fire>()));
}




void PetriNet::queueFire(int transitionId, SharedTreePointer<Token> token, boost::any a, boost::any b, boost::any c)
{
  impl_.queueFire(Fire(transitionId, token, a, b, c));
}

void PetriNet::addToken(int placeId, SharedTreePointer<Token> token)
{
  impl_.places_.find(placeId)->second.putToken(token);
}

void PetriNet::createTransition(int transitionId, const std::list<int>& inputPlaces, const std::list<int>& outputPlaces)
{
  impl_.transitions_.insert(std::pair<int, Transition>(transitionId, Transition(inputPlaces, outputPlaces, std::function<void(Token*, boost::any, boost::any, boost::any)>())));
}

const std::unordered_multiset<SharedTreePointer<Token>>& PetriNet::tokens(int placeId) const
{
  return impl_.places_.find(placeId)->second.tokens();
}

void PetriNet::reserve(int size)
{
  impl_.reserve(size);
}
