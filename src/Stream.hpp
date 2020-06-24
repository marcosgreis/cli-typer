#pragma once

#include <boost/process.hpp>
#include <memory>
#include <string>
#include <vector>

#include "Print.hpp"

class StreamListener
{
 public:
  virtual ~StreamListener() = default;
  virtual void streamUpdated(const std::string& line) = 0;
};

class StreamProcessor
{
 public:
  void setInputStream(boost::process::opstream* in);

  void setOutputStream(boost::process::ipstream* out);

  void addOutputListener(std::shared_ptr<StreamListener> listener);

  void typeInput(const std::string& command);

  void startProcessing();

 private:
  boost::process::opstream* _in;
  boost::process::ipstream* _out;
  std::vector<std::shared_ptr<StreamListener>> _listeners;
};
