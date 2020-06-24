#include "Stream.hpp"

void StreamProcessor::setInputStream(boost::process::opstream* in) { _in = in; }

void StreamProcessor::setOutputStream(boost::process::ipstream* out) { _out = out; }

void StreamProcessor::addOutputListener(std::shared_ptr<StreamListener> listener) { _listeners.push_back(listener); }

void StreamProcessor::typeInput(const std::string& command)
{
  if (!_in) {
    throw std::runtime_error{"input stream was nullptr"};
  }
  (*_in) << command << std::endl;
}

void StreamProcessor::startProcessing()
{
  // TODO: Capturing 'this' here is risky. This could crash.
  std::thread{[this] {
    std::string line;
    print("Reading output...");
    while (std::getline(*_out, line)) {
      for (const auto& listener : _listeners) {
        listener->streamUpdated(line);
      }
    }
    print("Stopped reading output");
  }}.detach();
}
