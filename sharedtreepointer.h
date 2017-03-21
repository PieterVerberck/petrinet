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

#ifndef PETRINET_SHAREDTREEPOINTER_H
#define PETRINET_SHAREDTREEPOINTER_H

#include <iterator>
#include <list>
#include <memory>

template<class T>
class SharedTreePointer
{
public:
  SharedTreePointer();
  SharedTreePointer(T* root);
  SharedTreePointer(T* ptr, T* root, int level);
  SharedTreePointer(const SharedTreePointer<T>& other);
  ~SharedTreePointer();

  SharedTreePointer<T>& operator=(const SharedTreePointer<T>& other);
  bool operator==(const SharedTreePointer<T>& other) const;
  T& operator*() const;
  T* operator->() const;
  bool unique() const;
  long use_count() const;

  T* get() const;

  template<typename S>
  S* get() const;

  SharedTreePointer<T> child(int i) const;
  int level() const;

  // Provides level-order-traversal through tree (breadth-first)
  class iterator : public std::iterator<std::forward_iterator_tag, SharedTreePointer<T>>
  {
    friend class SharedTreePointer<T>;
    public:
      iterator(const iterator& other);
      ~iterator();
      iterator& operator=(const iterator& other);
      iterator& operator++();
      SharedTreePointer<T>& operator*();
      bool operator==(const iterator& other) const;
      bool operator!=(const iterator& other) const;

    private:
      iterator(SharedTreePointer<T> root);
      SharedTreePointer<T> root_;
      SharedTreePointer<T> current_;
      std::list<SharedTreePointer<T>> queue_;
  };

  iterator begin() const;
  iterator begin(int level) const;
  iterator end() const;
  iterator end(int level) const;

private:
  SharedTreePointer(T* ptr, std::shared_ptr<T> root, int level);

  std::shared_ptr<T> root_;
  T* ptr_;
  int level_;
};

template<class T>
SharedTreePointer<T>::SharedTreePointer()
  : ptr_(0),
  level_(-1)
{
}

template<class T>
SharedTreePointer<T>::SharedTreePointer(T* root)
  : ptr_(root),
  root_(root),
  level_(1)
{
}

template<class T>
SharedTreePointer<T>::SharedTreePointer(T* ptr, T* root, int level)
  : ptr_(ptr),
  root_(root),
  level_(level)
{
}

template<class T>
SharedTreePointer<T>::SharedTreePointer(T* ptr, std::shared_ptr<T> root, int level)
  : ptr_(ptr),
  root_(root),
  level_(level)
{
}

template<class T>
SharedTreePointer<T>::SharedTreePointer(const SharedTreePointer& other)
  : ptr_(other.ptr_),
  root_(other.root_),
  level_(other.level_)
{
}

template<class T>
SharedTreePointer<T>::~SharedTreePointer()
{
}

template<class T>
SharedTreePointer<T>& SharedTreePointer<T>::operator=(const SharedTreePointer<T>& other)
{
  ptr_ = other.ptr_;
  root_ = other.root_;
  level_ = other.level_;
  return *this;
}

template<class T>
bool SharedTreePointer<T>::operator==(const SharedTreePointer<T>& other) const
{
  return ptr_ == other.ptr_;
}

template<class T>
T& SharedTreePointer<T>::operator*() const
{
  return *ptr_;
}

template<class T>
T* SharedTreePointer<T>::operator->() const
{
  return ptr_;
}

template<class T>
bool SharedTreePointer<T>::unique() const
{
  return root_.unique();
}

template<class T>
long SharedTreePointer<T>::use_count() const
{
}

template<class T>
T* SharedTreePointer<T>::get() const
{
  return ptr_;
}

template<class T>
template<typename S>
S* SharedTreePointer<T>::get() const
{
  return dynamic_cast<S*>(ptr_);
}

template<class T>
SharedTreePointer<T> SharedTreePointer<T>::child(int i) const
{
  return SharedTreePointer<T>(dynamic_cast<T*>(ptr_->child(i)), root_, level() + 1);
}

template<class T>
int SharedTreePointer<T>::level() const
{
  return level_;
}

template<class T>
typename SharedTreePointer<T>::iterator SharedTreePointer<T>::begin() const
{
  return SharedTreePointer<T>::iterator(*this);
}

template<class T>
typename SharedTreePointer<T>::iterator SharedTreePointer<T>::begin(int lvl) const
{
  SharedTreePointer<T> c = *this;
  for(int i = level(); i < lvl; ++i)
    c = c.child(0);

  SharedTreePointer<T>::iterator iter = SharedTreePointer<T>::iterator(*this);
  while((*iter).get() != c.get())
    ++iter;

  return iter;
}

template<class T>
typename SharedTreePointer<T>::iterator SharedTreePointer<T>::end() const
{
  return SharedTreePointer<T>::iterator(0);
}

template<class T>
typename SharedTreePointer<T>::iterator SharedTreePointer<T>::end(int lvl) const
{
  SharedTreePointer<T> c = *this;
  for(int i = level(); i < lvl + 1; ++i)
  {
    if(c->size())
      c = c.child(0);
    else
      return SharedTreePointer<T>::iterator(0);
  }

  SharedTreePointer<T>::iterator iter = SharedTreePointer<T>::iterator(*this);
  while((*iter).get() != c.get())
    ++iter;

  return iter;
}

namespace std
{
  template<typename T>
  struct hash<SharedTreePointer<T>>
  {
    std::size_t operator()(const SharedTreePointer<T>& s) const
    {
      return (std::size_t) s.get();
    }
  };
};


template<class T>
SharedTreePointer<T>::iterator::iterator(SharedTreePointer<T> root)
  : root_(root),
  current_(root)
{
  if(root_.get())
  {
    for(int i = 0; i < root_->size(); ++i)
      queue_.push_back(root_.child((i)));
  }
}

template<class T>
SharedTreePointer<T>::iterator::iterator(const iterator& other)
  : root_(other.root_),
  current_(other.current_),
  queue_(other.queue_)
{
}

template<class T>
SharedTreePointer<T>::iterator::~iterator()
{
}

template<class T>
typename SharedTreePointer<T>::iterator& SharedTreePointer<T>::iterator::operator=(const iterator& other)
{
  root_ = other.root_;
  current_ = other.current_;
  queue_ = other.queue_;
  return *this;
}

template<class T>
bool SharedTreePointer<T>::iterator::operator==(const iterator& other) const
{
  return current_ == other.current_;
}

template<class T>
bool SharedTreePointer<T>::iterator::operator!=(const iterator& other) const
{
  return !(*this == other);
}

template<class T>
typename SharedTreePointer<T>::iterator& SharedTreePointer<T>::iterator::operator++()
{
  // Level order traversal algorithm:
  // https://en.wikipedia.org/wiki/Breadth-first_search
  if(queue_.size())
  {
    current_ = queue_.front();
    queue_.pop_front();

    for(int i = 0; i < current_->size(); ++i)
      queue_.push_back(current_.child(i));
  }
  else
    current_ = 0;

  return *this;
}

template<class T>
SharedTreePointer<T>& SharedTreePointer<T>::iterator::operator*()
{
  return current_;
}

#endif