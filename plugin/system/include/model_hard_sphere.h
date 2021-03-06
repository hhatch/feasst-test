
#ifndef FEASST_SYSTEM_MODEL_HARD_SPHERE_H_
#define FEASST_SYSTEM_MODEL_HARD_SPHERE_H_

#include "system/include/model_two_body.h"
#include "math/include/constants.h"

namespace feasst {

/**
  The potential energy for a hard sphere model of radius \f$\sigma\f$ and
  separation distance \f$r\f$ is as follows

  \f$ U_{HS} = \left\{
    \begin{array}{lr}
      \infty & r \le \sigma \\
      0 & r > \sigma
    \end{array}
  \right. \f$
 */
class ModelHardSphere : public ModelTwoBody {
 public:
  ModelHardSphere() {}

  double energy(
    const double squared_distance,
    const int type1,
    const int type2,
    const ModelParams& model_params) const override {
    const double& sigma = model_params.mixed_sigma()[type1][type2];
    // INFO("r2 " << squared_distance);
    if (squared_distance <= sigma*sigma) {
      return NEAR_INFINITY;
    }
    return 0.;
  }

  std::shared_ptr<Model> create(std::istream& istr) const override {
    return std::make_shared<ModelHardSphere>(istr); }

  void serialize(std::ostream& ostr) const override {
    ostr << class_name_ << " ";
    feasst_serialize_version(607, ostr);
  }

  ModelHardSphere(std::istream& istr) {
    const int version = feasst_deserialize_version(istr);
    ASSERT(607 == version, version);
  }

  virtual ~ModelHardSphere() {}

 private:
  const std::string class_name_ = "ModelHardSphere";
};

inline std::shared_ptr<ModelHardSphere> MakeModelHardSphere() {
  return std::make_shared<ModelHardSphere>();
}

}  // namespace feasst

#endif  // FEASST_SYSTEM_MODEL_HARD_SPHERE_H_
