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

#ifndef PETRINET_EVENTLOOP_H
#define PETRINET_EVENTLOOP_H

#include <condition_variable>

#include <boost/asio.hpp>
#include <boost/thread.hpp>


class EventLoop
{
public:
  EventLoop();

  void start();
  void run();
  void stop();
  void waitForStopped();
  void waitForWorkDone();
  boost::asio::io_service& get();

private:
  boost::thread thread_;
  boost::asio::io_service tcpIoService_;

  // Prevent io_service::run from returning immediately
  // http://www.boost.org/doc/libs/1_52_0/doc/html/boost_asio/reference/io_service.html#boost_asio.reference.io_service.stopping_the_io_service_from_running_out_of_work
  std::auto_ptr<boost::asio::io_service::work> work_;

  std::condition_variable cv_;
  std::mutex mutex_;
  bool running_;
};


#endif