#include "utils/test/utils.h"
#include "patch/include/visit_model_inner_patch.h"
#include "configuration/include/file_xyz.h"
#include "models/include/model_square_well.h"
#include "monte_carlo/include/perturb_anywhere.h"

namespace feasst {

TEST(VisitModelInnerPatch, patch_one) {
  Configuration config;
  config.add_particle_type("../plugin/patch/forcefield/data.patch_one");
  config.set_model_param("cutoff", 0, 3.);
  config.set_model_param("cutoff", 1, 3.);
  FileXYZ().load("../plugin/patch/test/data/patch5.xyz", &config);
  ModelSquareWell model;
  VisitModel visit;
  auto patch = std::make_shared<VisitModelInnerPatch>();
  visit.set_inner(patch);
  visit.precompute(&config);
  patch->set_patch_angle(1, 90.);
  config.add(Group().add_site_type(0));
  visit.compute(model, &config, 1);
  EXPECT_NEAR(-3., visit.energy(), NEAR_ZERO);

  auto visit2 = test_serialize<VisitModel, VisitModel>(visit);
  visit2->compute(model, &config, 1);
  EXPECT_NEAR(-3., visit2->energy(), NEAR_ZERO);
}

TEST(VisitModelInnerPatch, patch_one_2body) {
  System system;
  { Configuration config;
    config.set_domain(Domain().set_cubic(10.));
    //{ auto angle = std::make_shared<ModelParam>();
    //  angle->set_name("angle");
    //  config.add(angle); }
    config.add_particle_type("../plugin/patch/forcefield/data.patch_one");
    config.add(Group().add_site_type(0));
    config.add_particle_of_type(0);
    config.add_particle_of_type(0);
    system.add(config); }

  { Potential potential;
    potential.set_model(std::make_shared<ModelSquareWell>());
    auto visitor = std::make_shared<VisitModel>();
    auto patch = std::make_shared<VisitModelInnerPatch>();
    visitor->set_inner(patch);
    potential.set_visit_model(visitor);
    potential.set_group_index(1); // optimization: loop through centers only.
    system.add(potential); }
  system.precompute();

  PerturbAnywhere anywhere;
  TrialSelect tsel;
  tsel.set_mobile(SelectList().particle(1, system.configuration()));
  auto traj = Position().set_vector({1.4, 0., 0.});
  anywhere.set_position(traj, &system, &tsel);

  PerturbRotate rotate;
  tsel.set_mobile(SelectList().particle(1, system.configuration()));
  auto axis = Position().set_vector({0., 0., 1.});
  FileXYZ file;
  file.write_for_vmd("tmp/patch.xyz", system.configuration());
  rotate.move(
    traj,
    RotationMatrix().axis_angle(axis, 180.),
    &system,
    &tsel
  );
  file.set_append(1);
  file.write("tmp/patch.xyz", system.configuration());

  EXPECT_NEAR(-1., system.energy(), NEAR_ZERO);

  rotate.move(traj,
    RotationMatrix().axis_angle(axis, -4.999),
    &system,
    &tsel);
  EXPECT_NEAR(-1., system.energy(), NEAR_ZERO);

  rotate.move(traj,
    RotationMatrix().axis_angle(axis, -0.002),
    &system,
    &tsel);
  EXPECT_NEAR(0., system.energy(), NEAR_ZERO);
}

} // namespace feasst

