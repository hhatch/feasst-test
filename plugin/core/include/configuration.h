
#ifndef FEASST_CORE_CONFIGURATION_H_
#define FEASST_CORE_CONFIGURATION_H_

#include <string>
#include <vector>
#include "core/include/domain_cuboid.h"
#include "core/include/particles.h"
#include "core/include/selection.h"

namespace feasst {

/**
  A configuration contains both the particles and the spatial domain/boundaries.

  For the particles, this includes both the pool of particles which may
  exist (referred to as particle types) in addition to the particles which
  physically exist (referred to as particles).

  Before particles are physically added to the system, all particle types which
  may exist must be defined.
  Once particles are added to the system, new particle types cannot be added.
  Similarly, site types are also defined at the same time as particle types.

  Groups of different particle/site types and other metrics may be defined.
  These groups then define a selection to be used to modify the system.

  Perturbations (e.g., removal and displacement) of a selection of
  particles/sites are performed in two stages:

   1. selection of particles/sites.
   2. implementation of the perturbation on the selection.

  Only one selection is stored for a configuration at a given time, so it is
  imperative that the intended selection is not erroneously changed before the
  intended perturbation is executed.
 */
class Configuration {
 public:
  Configuration();

  /// Add a particle type that may exist.
  void add_particle_type(const char* file_name);

  /// Return the number of particle types.
  int num_particle_types() const { return particle_types_.num(); }

  /// Return the number of site types.
  int num_site_types() const { return unique_types_.num_sites(); }

  /// Return the particle types.
  const Particles& particle_types() const { return particle_types_; }

  /// Return the particle associated with the type.
  const Particle& particle_type(const int type) const {
    return particle_types_.particle(type);
  }

  /// Return the unique types. Only unique sites and bonds are included.
  /// Thus, the site index is the same as the numeric value for the site type.
  /// And the same for bonds.
  /// This serves as a container for properties based on site or bond type.
  const Particles& unique_types() const { return unique_types_; }

  /// Return the unique type by individual particle.
  const Particle& unique_type(const int type) const {
    return unique_types_.particle(type);
  }

  /// Return the particles.
  const Particles& particles() const { return particles_; }

  /// Add particles of a given type.
  void add_particle(const int type);

  /// Return particle by index.
  const Particle& particle(const int index) const {
    return particles_.particle(index);
  }

  /// Return the number of sites.
  int num_sites() const { return particles_.num_sites(); }

  /// Return the number of particles.
  int num_particles() const { return particles_.num(); }

  /* Create a selection by group */

  /// Add a group (after types are defined but before particles are added).
  void add(const Group group);

  /// Return the number of group selections.
  int num_groups() const { return static_cast<int>(group_selects_.size()); }

  /// Load coordinates by per-site vector containing per-dimension vector.
  void update_positions(const std::vector<std::vector<double> > coords);

  /* Selections are for modifications of a subset of the configuration.
     Note that the requested selection may not be possible.
     In this case the selection remains empty. */

  /// Selection by group.
  void select(const Group& group);

  /// Select random particle in group.
  /// Note: this method is relatively unoptimized compared with
  /// select_random_particle_of_type or using a particle configuration.
  void select_random_particle(const Group& group);

  /// Select a random particle.
  void select_random_particle();

  /// Select a random particle of a given type.
  void select_random_particle_of_type(const int type);

  /// Select a random particle from group index.
  void select_random_particle_of_group(
    /// Group indices are determined by the order of add(Group).
    /// A group index of -1 indicates all particles.
    const int group_index);

  /// Select a particle by index.
  /// HWH: selection by type and index?
  void select_particle(const int particle_index);

  /// Select a site.
  void select_site(const int particle_index, const int site_index);

  /// Select sites.
  PositionSelection select_sites(const int particle_index,
                                 const std::vector<int> site_indices) {
    selection_.clear();
    selection_.add_sites(particle_index, site_indices);
    return position_selection();
  }

  /// Select the most recently added particle.
  void select_last_particle();

  /// Select all particles in the configuration
  void select_all();

  /// Return selection of all particles and sites in the configuration.
  const Selection& get_selection_of_all() const { return select_all_; }

  /// Return selection.
  const Selection& selection() const { return selection_; }

  /// Return selected particle.
  const Particle& selected_particle() const;

  /// Set the selection of particles.
  void set_selection(const Selection selection);

  /// Return the selection and their positions.
  PositionSelection position_selection() const {
    return PositionSelection(selection(), particles());
  }

  /// Update the positions from a selection.
  void update_positions(const PositionSelection& select);

  /* The following are modifications of a subset of the configuration. */

  /// Remove particle(s) by selection. Clears selection because no longer valid.
  void remove_selected_particles();

  /// Same as above except for only one particle that is selected.
  void remove_selected_particle();

  /// Displace particle(s) by selection.
  void displace_selected_particles(const Position &displacement);

  /// Same as above except for only one particle that is selected.
  void displace_selected_particle(const Position &displacement);

  /// Displace the selection. No periodic boundary conditions applied.
  void displace_selection(const Position &displacement);

  /// Replace positions of particle by selection.
  void replace_selected_particle_position(const Particle& replacement);

  /// Replace positions of the last particle.
  void replace_position_of_last(const Particle& replacement);

  /* Interface with ModelParams */

  /// Modify model parameter of a given site type and name to value.
  void set_model_param(const char* name,
                       const int site_type,
                       const double value) {
    unique_types_.set_model_param(name, site_type, value);
  }

  /* Interface with Cells */

  /// Initialize the cells according to the minimum side length.
  void init_cells(const double min_length);

  /* Checks and hacky additions */

  // Used to quickly create a configuration for testing purposes only.
  void default_configuration();

  // HWH consider moving domain from configuration to system.
  // HWH update cells when changing domain.
  // HWH: account for DomainTriclinic
  // HWH: functions which would take domain as argument:
  // add_(particle
  // displace_particle_
  // position_tracker_->replace_position_
  // init_cells
  DomainCuboid domain() const { return domain_; }
  void set_domain(const DomainCuboid domain2) { domain_ = domain2; }
  int dimension() const { return domain().dimension(); }

  /// Check consistency of dimensions and lists.
  void check_size() const;

  /// Return the group-based selections.
  const std::vector<GroupSelection>& group_selects() const {
    return group_selects_;
  }

  /// Return the group-based selections by index.
  const GroupSelection& group_select(const int index) const {
    return group_selects_[index];
  }

 private:
  Particles particle_types_;
  Particles unique_types_;
  Particles particles_;
  DomainCuboid domain_;

  /// Current selection of particle(s) and site(s)
  // HWH rename to select_current_
  Selection selection_;

  /// A list of all particle and site indices.
  Selection select_all_;

  /// Selections based on groups that are continuously updated.
  // HWH currently only updated when adding and removing particles
  // HWH but at some point it should check for positional changes
  // HWH if groups are defined based on positions.
  std::vector<GroupSelection> group_selects_;

  /// Unique identifier for the collection of particle indices.
  std::string unique_indices_;
  void reset_unique_indices_();
  Random random_;

  /// Add particle.
  void add_(const Particle particle);

  /// Remove particle by index.
  void remove_particle_(const int index);

  /// Displace particle by index.
  void displace_particle_(const int index,
                          const Position &displacement);

  /// Displace site by index.
  void displace_site_(const int particle_index,
                      const int site_index,
                      const Position &displacement);

  /// Replace positions of particle by index.
  void replace_position_(const int index,
                         const Particle& replacement);

  /// Replace position of site in particle.
  void replace_position_(const int particle_index,
                         const int site_index,
                         const Position& replacement);

  /// Replace position of particle but not site.
  void replace_position_(const int particle_index,
                         const Position& replacement);

  /// Update position trackers of a particle (e.g., cell, neighbor, etc).
  /// Update all sites if site_index == -1.
  void position_tracker_(const int particle_index, const int site_index = -1);

  /// Update position trackers of all particles.
  void position_tracker_();

  /// Add particle to selection.
  void add_to_selection_(const int particle_index,
                         GroupSelection * select) const;

  /// Initialize selection based on groups
  void init_selection_(GroupSelection * group_select) const;

  /// Remember groups based on types.
  std::vector<int> group_store_particle_type_,
                   group_store_group_index_;

  /// Return the index of the group based on particle types.
  int particle_type_to_group_(const int particle_type);

  void check_id_(const Selection& select) const;
};

}  // namespace feasst

#endif  // FEASST_CORE_CONFIGURATION_H_
