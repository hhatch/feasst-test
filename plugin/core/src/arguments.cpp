#include <cmath>
#include "core/include/arguments.h"
#include "core/include/debug.h"
#include "core/include/constants.h"
#include "core/include/utils.h"

namespace feasst {

void Arguments::init(const argtype &args) {
  args_ = args;
}

Arguments& Arguments::key(const std::string &key) {
  key_ = key;
  used_keys_.push_back(key_);
  remove_ = false;
  return *this;
}

Arguments& Arguments::dflt(const std::string &defaultVal) {
  default_value_ = defaultVal;
  return *this;
}

bool Arguments::empty() {
  ASSERT(!key_.empty(), "key must be set before");
  auto pair = args_.find(key_);
  if (pair != args_.end()) {
    return false;
  }
  return true;
}

std::string Arguments::str() {
  ASSERT(!key_.empty(), "key must be set before");
  auto pair = args_.find(key_);
  std::string str;
  if (pair != args_.end()) {
    str = pair->second;

    // remove parsed arg from args_
    if (remove_) {
      args_.erase(pair);
      remove_ = false;
    }
  } else {
    ASSERT(!default_value_.empty(), "key(" << key_ << ") is required for args "
      << "when no default value is set.");
    str = default_value_;
  }
  key_ = "";
  default_value_ = "";
  return str;
}

bool Arguments::check_all_used(){
  used_keys_.push_back("");
  for (const auto &pair : args_) {
    ASSERT(find_in_list(pair.first, used_keys_),
      "Key arg(" << pair.first <<") "
      << ") is not recognized. Check for any typos in your provided keyword "
      << "arguments and check documentation. All keywords provided in args "
      << "must be used, otherwise, a simple typo in keyword arguments would go "
      << "unnoticed. If you are a developer, check that you have processed "
      << "all possible keywords in the appropriate constructor.");
  }
  return true;
}

Arguments& Arguments::remove() {
  remove_ = true;
  return *this;
}

int Arguments::integer() {
  const std::string string_value = str();
  std::stringstream errmsg;
  int intVal = -1;
  errmsg << "Argument({" << used_keys_.back() << ", " << string_value << "}) was "
    << "expected to be an integer";
  try {
    intVal = stoi(string_value);
  } catch (...) {
    ASSERT(0, errmsg.str());
  }
  const double dble = stod(string_value);
  ASSERT(std::abs(dble - static_cast<double>(intVal)) < 10*NEAR_ZERO, errmsg.str());
  return intVal;
}

double Arguments::dble() {
  const std::string string_value = str();
  double double_value = -1;
  try {
    double_value = stod(string_value);
  } catch (...) {
    ASSERT(0, "Argument({" << used_keys_.back() << ", " << string_value << "}) was "
    << "expected to be a double precision floating point number.");
  }
  return double_value;
}

}  // namespace feasst