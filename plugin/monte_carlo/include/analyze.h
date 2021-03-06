
#ifndef FEASST_MONTE_CARLO_ANALYZE_H_
#define FEASST_MONTE_CARLO_ANALYZE_H_

#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include "monte_carlo/include/trial_factory.h"
//#include "monte_carlo/include/trial_transfer.h"
#include "configuration/include/bond_visitor.h"
#include "configuration/include/file_xyz.h"
#include "monte_carlo/include/stepper.h"
#include "utils/include/arguments.h"

namespace feasst {

class Analyze : public Stepper {
 public:
  Analyze(const argtype &args = argtype()) : Stepper(args) {}

  // By default, do nothing during initialization.
  virtual void initialize(const Criteria * criteria,
      const System& system,
      const TrialFactory& trial_factory) {}

  virtual void trial(const Criteria * criteria,
      const System& system,
      const TrialFactory& trial_factory);

  virtual void update(const Criteria * criteria,
      const System& system,
      const TrialFactory& trial_factory);

  virtual std::string write(const Criteria * criteria,
      const System& system,
      const TrialFactory& trial_factory);

  virtual const std::vector<std::shared_ptr<Analyze> >& analyzers() const {
    ERROR("not implemented"); }

  virtual const Analyze * analyze(const int index) const {
    ERROR("not implemented"); }

  virtual void serialize(std::ostream& ostr) const;
  virtual std::shared_ptr<Analyze> create(std::istream& istr) const;
  std::map<std::string, std::shared_ptr<Analyze> >& deserialize_map();
  std::shared_ptr<Analyze> deserialize(std::istream& istr);
  Analyze(std::istream& istr) : Stepper(istr) {}
  virtual ~Analyze() {}
  std::string class_name() const override { return std::string("Analyze"); }
};

class AnalyzeWriteOnly : public Analyze {
 public:
  AnalyzeWriteOnly(
    /**
      steps_per : write every this many steps
     */
    const argtype &args = argtype()) : Analyze(args) {
    // disable update
    Stepper::set_steps_per_update(-1);

    // parse
    if (!args_.key("steps_per").empty()) {
      set_steps_per(args_.integer());
    }
  }

  void set_steps_per_update(const int steps) override {
    ERROR("This analyze is write only."); }

  void set_steps_per(const int steps) { set_steps_per_write(steps); }

  AnalyzeWriteOnly(std::istream& istr) : Analyze(istr) {}
};

class AnalyzeUpdateOnly : public Analyze {
 public:
  AnalyzeUpdateOnly(
    /**
      steps_per : update every this many steps
     */
    const argtype &args = argtype()) : Analyze(args) {
    // disable update
    Analyze::set_steps_per_write(-1);

    // parse
    if (!args_.key("steps_per").empty()) {
      set_steps_per(args_.integer());
    }
  }

  void set_steps_per_write(const int steps) override {
    ERROR("This analyze is update only."); }

  void set_steps_per(const int steps) { set_steps_per_update(steps); }

  AnalyzeUpdateOnly(std::istream& istr) : Analyze(istr) {}
};

}  // namespace feasst

#endif  // FEASST_MONTE_CARLO_ANALYZE_H_
