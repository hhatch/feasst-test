#include <memory>
#include "system/include/potential.h"
#include "system/include/model_empty.h"

namespace feasst {

Potential::Potential() {
  model_ = std::make_shared<ModelEmpty>();
  visit_model_ = std::make_shared<VisitModel>();
  set_group_index();
}

Potential::Potential(std::shared_ptr<Model> model) : Potential() {
  set_model(model);
}

Potential::Potential(std::shared_ptr<VisitModel> visit_model) : Potential() {
  set_visit_model(visit_model);
}

Potential::Potential(
    std::shared_ptr<Model> model,
    std::shared_ptr<VisitModel> visit_model) : Potential() {
  set_model(model);
  set_visit_model(visit_model);
}

void Potential::set_model_param(const char* name,
    const int site_type,
    const double value) {
  ASSERT(model_params_override_, "you must first initialize model params");
  model_params_.set(name, site_type, value);
}

const ModelParams& Potential::model_params() const {
  ASSERT(model_params_override_, "you must first initialize model params");
  return model_params_;
}

double Potential::energy(Configuration * config) {
  ASSERT(visit_model_, "visitor must be set.");
  if (!cache_.is_unloading(&stored_energy_)) {
    if (model_params_override_) {
      stored_energy_ = model_->compute(model_params_, group_index_, config,
                                       visit_model_.get());
    } else {
      stored_energy_ = model_->compute(group_index_, config, visit_model_.get());
    }
    cache_.load(stored_energy_);
  }
  return stored_energy_;
}

double Potential::energy(const Select& select, Configuration * config) {
  ASSERT(visit_model_, "visitor must be set.");
  if (!cache_.is_unloading(&stored_energy_)) {
    if (model_params_override_) {
      stored_energy_ = model_->compute(model_params_, select, group_index_,
                                       config, visit_model_.get());
    } else {
      stored_energy_ = model_->compute(select, group_index_, config,
                                       visit_model_.get());
    }
    cache_.load(stored_energy_);
  }
  return stored_energy_;
}

void Potential::serialize(std::ostream& ostr) const {
  feasst_serialize_version(432, ostr);
  feasst_serialize(group_index_, ostr);
  feasst_serialize_fstdr(visit_model_, ostr);
  feasst_serialize_fstdr(model_, ostr);
  feasst_serialize(stored_energy_, ostr);
  feasst_serialize(model_params_override_, ostr);
  if (model_params_override_) {
    feasst_serialize_fstobj(model_params_, ostr);
  }
  feasst_serialize_fstobj(cache_, ostr);
}

Potential::Potential(std::istream& istr) {
  const int version = feasst_deserialize_version(istr);
  ASSERT(432 == version, "version mismatch: " << version);
  feasst_deserialize(&group_index_, istr);
  // feasst_deserialize_fstdr(visit_model_, istr);
  { // HWH for unknown reasons the above template function does not work
    int existing;
    istr >> existing;
    if (existing != 0) {
      visit_model_ = visit_model_->deserialize(istr);
    }
  }
  // feasst_deserialize_fstdr(model_, istr);
  { // HWH for unknown reasons the above template function does not work
    int existing;
    istr >> existing;
    if (existing != 0) {
      model_ = model_->deserialize(istr);
    }
  }
  feasst_deserialize(&stored_energy_, istr);
  feasst_deserialize(&model_params_override_, istr);
  if (model_params_override_) {
    feasst_deserialize_fstobj(&model_params_, istr);
  }
  feasst_deserialize_fstobj(&cache_, istr);
}

}  // namespace feasst
