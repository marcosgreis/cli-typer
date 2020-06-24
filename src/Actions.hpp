#pragma once

#include <chrono>
#include <string>

class StreamProcessor;

class Action
{
 public:
  virtual ~Action() = default;

  virtual void act() = 0;

  virtual std::string toString() const = 0;
};

std::shared_ptr<Action> createTypeAction(const std::string& description,
                                         std::shared_ptr<StreamProcessor> streamProcessor);
std::shared_ptr<Action> createSleepAction(const std::string& description);
std::shared_ptr<Action> createQuestionAction(std::shared_ptr<StreamProcessor> streamProcessor);
std::shared_ptr<Action> createWaitForWordAction(const std::string& description,
                                                std::shared_ptr<StreamProcessor> streamProcessor);
