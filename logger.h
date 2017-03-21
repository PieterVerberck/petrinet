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

#ifndef PETRINET_LOGGER_H
#define PETRINET_LOGGER_H

#include "eventloop.h"

#include <chrono>
#include <fstream>
#include <mutex>

#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/thread.hpp>

struct LogItem
{
    std::string getTimeString() const;
    std::string getMessage() const;

    std::chrono::system_clock::time_point time_;
    std::function<std::string(void)> message_;
};

class LogControl
{
  public:
    virtual ~LogControl(){}
    virtual void openOutput() = 0;
    virtual void writeToOutput(std::string msg) = 0;
    virtual void closeOutput() = 0;
};

class FileLogControl : public LogControl
{
  public:
    FileLogControl(std::string name);

    static void setFolder(std::string folder);
    virtual void openOutput();
    virtual void writeToOutput(std::string msg);
    virtual void closeOutput();

  private:
    static std::string folder_;
    std::fstream file_;
    std::string name_;
};

struct LoggerData
{
  LoggerData() : queue_(200){}
  ~LoggerData() { delete logControl_; }
  boost::lockfree::queue<LogItem*> queue_;
  LogControl* logControl_;
  bool postActive_;
};

class Logger
{
  public:
    Logger(LogControl* logControl = new FileLogControl("mylog"));
    virtual ~Logger();
    
    void log(const char* message);
    void log(std::string message);
    void log(std::function<std::string(void)> message);

    static EventLoop& ioService();

  private:
    static void handleLog(std::shared_ptr<LoggerData> loggerData);

    std::shared_ptr<LoggerData> loggerData_;
    static EventLoop ioService_;
};


#endif
