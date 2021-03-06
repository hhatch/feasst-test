
#include "monte_carlo/include/modify_factory.h"

namespace feasst {

ModifyFactory::ModifyFactory(std::istream& istr) : Modify(istr) {
  feasst_deserialize_version(istr);
  // feasst_deserialize_fstdr(&modifiers_, istr);
  // HWH for unknown reasons, function template doesn't work
  int dim1;
  istr >> dim1;
  modifiers_.resize(dim1);
  for (int index = 0; index < dim1; ++index) {
    int existing;
    istr >> existing;
    if (existing != 0) {
      modifiers_[index] = modifiers_[index]->deserialize(istr);
    }
  }
}

void ModifyFactory::serialize(std::ostream& ostr) const {
  Stepper::serialize(ostr);
  feasst_serialize_version(1, ostr);
  feasst_serialize_fstdr(modifiers_, ostr);
}

}  // namespace feasst
