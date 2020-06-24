#include <iostream>
#include <unordered_map>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/process.hpp>

#include "Actions.hpp"
#include "Print.hpp"
#include "Stream.hpp"

struct Config {
  std::string command;
  std::chrono::seconds exitTimeout;
  std::chrono::milliseconds actionDelay;
};

class CoutStreamListener : public StreamListener
{
 public:
  void streamUpdated(const std::string& line) override { std::cout << line << std::endl; }
};

std::string toString(const Config& cfg)
{
  return std::string{"command: "} + cfg.command + ", exitTimeout: " + std::to_string(cfg.exitTimeout.count()) +
         "s, actionDelay " + std::to_string(cfg.actionDelay.count()) + "ms";
}

std::tuple<Config, std::vector<std::shared_ptr<Action>>> parseConfig(
  const std::string& configFile, const std::shared_ptr<StreamProcessor>& streamProcessor)
{
  print("Reading config from '%s'...", configFile.c_str());
  auto cfg = Config{"cat", std::chrono::seconds{1}, std::chrono::milliseconds{200}};

  std::vector<std::shared_ptr<Action>> actions;

  std::ifstream inputFile{configFile};
  std::string line;
  while (std::getline(inputFile, line)) {
    boost::trim(line);
    if (line.empty()) {
      continue;
    }

    const auto DELIMITER = std::string{":"};
    auto key = line.substr(0, line.find(DELIMITER));
    auto value = line.substr(line.find(DELIMITER) + 1, line.size() - 1);

    boost::trim(key);
    boost::trim(value);
    print("> key: '%s' | value '%s'", key.c_str(), value.c_str());

    if (key == "cfg.command") {
      cfg.command = value;
    } else if (key == "cfg.exitTimeoutS") {
      cfg.exitTimeout = std::chrono::seconds{std::stoi(value)};
    } else if (key == "cfg.actionDelayMs") {
      cfg.actionDelay = std::chrono::milliseconds{std::stoi(value)};
    } else {
      std::shared_ptr<Action> action;
      if (key == "type") {
        action = createTypeAction(value, streamProcessor);
      } else if (key == "sleep") {
        action = createSleepAction(value);
      } else if (key == "question") {
        action = createQuestionAction(streamProcessor);
      } else if (key == "waitFor") {
        action = createWaitForWordAction(value, streamProcessor);
      }

      if (!action) {
        print("Invalid command read: %s. Abort.", key.c_str());
        throw std::invalid_argument{"Invalid command key: " + key};
      }

      actions.push_back(std::move(action));
    }
  }

  print("Using config {%s}", toString(cfg).c_str());
  return {cfg, actions};
}

int main(int argc, char* argv[])
{
  print("Started!");
  if (argc != 2) {
    print("Incorrect number of parameters (%i).", argc);
    return 1;
  }

  const auto streamProcessor = std::make_shared<StreamProcessor>();
  std::string configFile = std::string{argv[1]};
  auto [cfg, actions] = parseConfig(configFile, streamProcessor);

  print("Doing the work... :)");

  boost::process::opstream in;
  boost::process::ipstream out;
  streamProcessor->setInputStream(&in);
  streamProcessor->setOutputStream(&out);
  streamProcessor->addOutputListener(std::make_shared<CoutStreamListener>());

  streamProcessor->startProcessing();
  boost::process::child process{cfg.command,
                                boost::process::std_out > out,
                                boost::process::std_err > boost::process::null,
                                boost::process::std_in < in};

  for (const auto& action : actions) {
    print("! %s", action->toString().c_str());
    action->act();
    std::this_thread::sleep_for(cfg.actionDelay);
  }

  print("Waiting process for %is", cfg.exitTimeout.count());
  process.wait_for(cfg.exitTimeout);
  process.terminate();

  print("Done!");
  return 0;
}
