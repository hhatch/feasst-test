
#ifndef FEASST_STEPPERS_CHECK_H_
#define FEASST_STEPPERS_CHECK_H_

#include "monte_carlo/include/modify.h"

namespace feasst {

/**
  Run periodic checks on each class.
 */
class Check : public ModifyUpdateOnly {
 public:
  Check(const argtype &args = argtype()) : ModifyUpdateOnly(args) {}
  void update(Criteria * criteria,
      System * system,
      TrialFactory * trial_factory) override {
    system->configuration().check();
  }

  std::string class_name() const override { return std::string("Check"); }

  void serialize(std::ostream& ostr) const override {
    Stepper::serialize(ostr);
    feasst_serialize_version(1, ostr);
  }

  std::shared_ptr<Modify> create(std::istream& istr) const override {
    return std::make_shared<Check>(istr); }

  Check(std::istream& istr) : ModifyUpdateOnly(istr) {
    feasst_deserialize_version(istr); }
};

inline std::shared_ptr<Check> MakeCheck(const argtype &args = argtype()) {
  return std::make_shared<Check>(args);
}

}  // namespace feasst

#endif  // FEASST_STEPPERS_CHECK_H_
