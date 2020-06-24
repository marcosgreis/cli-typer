#include "Actions.hpp"

#include <future>
#include <iostream>

#include "Stream.hpp"

class TypeAction : public Action
{
 public:
  TypeAction(std::string command, std::shared_ptr<StreamProcessor> streamProcessor)
  : _command{std::move(command)}
  , _streamProcessor{std::move(streamProcessor)}
  {
  }

  void act() override { _streamProcessor->typeInput(_command); }

  std::string toString() const override { return {"type: " + _command}; }

 private:
  std::string _command;
  std::shared_ptr<StreamProcessor> _streamProcessor;
};

std::shared_ptr<Action> createTypeAction(const std::string& description,
                                         std::shared_ptr<StreamProcessor> streamProcessor)
{
  return std::make_shared<TypeAction>(description, std::move(streamProcessor));
}

class SleepAction : public Action
{
 public:
  SleepAction(std::chrono::seconds delay)
  : _delay{std::move(delay)}
  {
  }

  void act() override { std::this_thread::sleep_for(_delay); }

  std::string toString() const override { return "sleepFor: " + std::to_string(_delay.count()) + "s"; }

 private:
  std::chrono::seconds _delay;
};

std::shared_ptr<Action> createSleepAction(const std::string& description)
{
  return std::make_shared<SleepAction>(std::chrono::seconds{std::stoi(description)});
}

class QuestionAction : public Action
{
 public:
  QuestionAction(std::shared_ptr<StreamProcessor> streamProcessor)
  : _streamProcessor{std::move(streamProcessor)}
  {
  }

  void act() override
  {
    std::string str;
    print("? ");
    std::cin >> str;
    _streamProcessor->typeInput(str);
  }

  std::string toString() const override { return {"question: "}; }

 private:
  std::shared_ptr<StreamProcessor> _streamProcessor;
};

std::shared_ptr<Action> createQuestionAction(std::shared_ptr<StreamProcessor> streamProcessor)
{
  return std::make_shared<QuestionAction>(std::move(streamProcessor));
}

class WaitForWordAction : public Action, public StreamListener
{
 public:
  WaitForWordAction(std::string word, std::chrono::seconds timeout)
  : _word{std::move(word)}
  , _timeout{std::move(timeout)}
  {
  }

  void streamUpdated(const std::string& line) override
  {
    std::unique_lock lock{_mutex};
    if (!_acting) {
      return;
    }

    if (line.find(_word) != std::string::npos) {
      _promise.set_value();
      _acting = false;
    }
  }

  void act() override
  {
    std::unique_lock lock{_mutex};
    _acting = true;
    lock.unlock();

    if (_promise.get_future().wait_for(_timeout) == std::future_status::timeout) {
      print("The word '%s' didn't appear for %is. Aborting execution.", _word.c_str(), _timeout.count());
      abort();
    }
  }

  std::string toString() const override
  {
    return {"waitFor: " + _word + ", timeout: " + std::to_string(_timeout.count()) + "s"};
  }

 private:
  std::string _word;
  std::chrono::seconds _timeout;
  std::promise<void> _promise;

  std::mutex _mutex;
  bool _acting{false};
};
std::shared_ptr<Action> createWaitForWordAction(const std::string& description,
                                                std::shared_ptr<StreamProcessor> streamProcessor)
{
  const auto action = std::make_shared<WaitForWordAction>(description, std::chrono::seconds{10});
  streamProcessor->addOutputListener(action);
  return action;
}
