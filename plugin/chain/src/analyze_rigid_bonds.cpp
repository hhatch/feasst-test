#include "chain/include/analyze_rigid_bonds.h"

namespace feasst {

class MapAnalyzeRigidBonds {
 public:
  MapAnalyzeRigidBonds() {
    AnalyzeRigidBonds().deserialize_map()["AnalyzeRigidBonds"] =
      MakeAnalyzeRigidBonds();
  }
};

static MapAnalyzeRigidBonds mapper_ = MapAnalyzeRigidBonds();

AnalyzeRigidBonds::AnalyzeRigidBonds(const argtype &args) : AnalyzeUpdateOnly(args) {}

}  // namespace feasst
