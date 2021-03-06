#include "feasst.h"

int main() {
  feasst::MonteCarlo mc;
  mc.add(feasst::Configuration({
    {"cubic_box_length", "8"},
    {"particle_type", feasst::append_install_dir("forcefield/data.lj")},
  }));
  mc.add(feasst::Potential(feasst::MakeModelLJ()));
  mc.add(feasst::Potential(feasst::MakeLongRangeCorrections()));
  mc.add(feasst::MakeCriteriaMetropolis(
    {{"beta", "1.2"}, {"chemical_potential", "1."}}));
  mc.add(feasst::MakeTrialTranslate(
    {{"tunable_param", "2."}, {"tunable_target_acceptance", "0,2"}));
  mc.add(feasst::MakeTuner({{"steps_per", feasst::str(steps_per)}}));
  mc.seek_num_particles(50);
  const int steps_per = 1e4;
  mc.add(feasst::MakeLog({{"steps_per", feasst::str(steps_per)}}));
  mc.add(feasst::MakeMovie(
   {{"steps_per", feasst::str(steps_per)}, {"file_name", "movie.xyz"}}));
  mc.add(feasst::MakeCheckEnergy(
   {{"steps_per", feasst::str(steps_per)}, {"tolerance", "1e-8"}}));
  mc.set(feasst::MakeRandomMT19937({{"seed", "date"}}));
  mc.attempt(1e6);
}
