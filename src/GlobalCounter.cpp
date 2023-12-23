#include "GlobalCounter.h"
#include <iostream>

GlobalCounter* GlobalCounter::instance = nullptr;

std::string GetNext() {
  return "%" + std::to_string(GlobalCounter::GetInstance().GetNext());
}

std::string GetCurrent() {
    return "%" + std::to_string(GlobalCounter::GetInstance().GetCurrent());
}