#include <string>
#include <fstream>
#include <sstream>
#include "monte_carlo/include/stepper.h"
#include "utils/include/debug.h"
#include "utils/include/utils_io.h"

namespace feasst {

Stepper::Stepper(const argtype &args) {
  args_.init(args);
  set_steps_per_write(args_.key("steps_per_write").dflt("1").integer());
  set_steps_per_update(args_.key("steps_per_update").dflt("1").integer());
  if (args_.key("file_name").used()) {
    set_file_name(args_.str());
  }

  // append
  if (args_.key("append").dflt("0").boolean()) {
    set_append();
  } else {
    set_no_append();
  }

  set_multistate(args_.key("multistate").dflt("0").boolean());
}

bool Stepper::is_time(const int steps_per, int * steps_since) {
  if (steps_per > 0) {
    ++(*steps_since);
    if (*steps_since >= steps_per) {
      *steps_since = 0;
      return true;
    } else {
      ASSERT(*steps_since < steps_per,
        "skipped an analysis step?");
    }
  }
  return false;
}

void Stepper::printer(const std::string output) {
  DEBUG("filename? " << file_name_);
  if (file_name_.empty()) {
    std::cout << output;
  } else {
    std::ofstream file;
    if (append_) {
      file.open(file_name_, std::ofstream::out | std::ofstream::app);
    } else {
      file.open(file_name_, std::ofstream::out);
    }
    file << output;
    file.close();
  }
}

void Stepper::set_state(const int state) {
  state_ = state;
  if (!file_name_.empty()) {
    std::stringstream ss;
    ss << "state" << state << "_" << file_name_;
    file_name_ = ss.str();
  }
}

void Stepper::serialize(std::ostream& ostr) const {
  ostr << class_name() << " ";
  feasst_serialize_version(497, ostr);
  feasst_serialize(steps_since_update_, ostr);
  feasst_serialize(steps_since_write_, ostr);
  feasst_serialize(steps_per_update_, ostr);
  feasst_serialize(steps_per_write_, ostr);
  feasst_serialize(file_name_, ostr);
  feasst_serialize(append_, ostr);
  feasst_serialize(is_multistate_, ostr);
  feasst_serialize(state_, ostr);
}

Stepper::Stepper(std::istream& istr) {
  std::string name;
  istr >> name;
  const int version = feasst_deserialize_version(istr);
  ASSERT(497 == version, version);
  feasst_deserialize(&steps_since_update_, istr);
  feasst_deserialize(&steps_since_write_, istr);
  feasst_deserialize(&steps_per_update_, istr);
  feasst_deserialize(&steps_per_write_, istr);
  feasst_deserialize(&file_name_, istr);
  feasst_deserialize(&append_, istr);
  feasst_deserialize(&is_multistate_, istr);
  feasst_deserialize(&state_, istr);
}

}  // namespace feasst
