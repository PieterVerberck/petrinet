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

#include "logger.h"
#include "eventloop.h"

#include "boost/date_time/posix_time/posix_time.hpp"

#include <chrono>


EventLoop Logger::ioService_ = EventLoop();

std::string FileLogControl::folder_ = "";

std::string LogItem::getTimeString() const
{
  std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(time_.time_since_epoch());
  boost::posix_time::ptime pt = boost::posix_time::from_time_t(ms.count() / 1000000);
  pt += boost::posix_time::microseconds(ms.count() % 1000000);

  return boost::posix_time::to_iso_extended_string(boost::posix_time::microsec_clock::local_time());
}

std::string LogItem::getMessage() const
{
  return getTimeString() + " " + message_();
}


Logger::Logger(LogControl* logControl)
  : loggerData_(new LoggerData())
{
  loggerData_->logControl_ = logControl;
  loggerData_->postActive_ = false;
}

Logger::~Logger()
{
}

void Logger::log(const char* message)
{
  log(std::string(message));
}

void Logger::log(std::string message)
{
  log([=]{return message;});
}

void Logger::log(std::function<std::string(void)> message)
{
  LogItem item;
  item.time_ = std::chrono::system_clock::now();
  item.message_ = message;

  loggerData_->queue_.push(new LogItem(item));
  
  if(!loggerData_->postActive_)
  {
    loggerData_->postActive_ = true;
    ioService_.get().post(std::bind(&Logger::handleLog, loggerData_));
  }
}

void Logger::handleLog(std::shared_ptr<LoggerData> loggerData)
{
  loggerData->logControl_->openOutput();

  loggerData->postActive_ = false;

  LogItem* item;
  while(loggerData->queue_.pop(item))
  {
    loggerData->logControl_->writeToOutput(item->getMessage());
    delete item;
  }

  loggerData->logControl_->closeOutput();
}

EventLoop& Logger::ioService()
{
  return ioService_;
}

FileLogControl::FileLogControl(std::string name)
  : name_(name)
{
}

void FileLogControl::setFolder(std::string folder)
{
  folder_ = folder;
}

void FileLogControl::openOutput()
{
  file_.open(folder_ + "/" + name_ + ".txt", std::ios::out | std::ios::app);
}

void FileLogControl::writeToOutput(std::string msg)
{
  assert(file_.is_open());
  file_ << msg << std::endl;
}

void FileLogControl::closeOutput()
{
  file_.close();
}
