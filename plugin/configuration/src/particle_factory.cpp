#include "configuration/include/particle_factory.h"
#include "utils/include/debug.h"
#include "math/include/histogram.h"
#include "configuration/include/file_lmp.h"
#include "utils/include/utils.h"

namespace feasst {

void ParticleFactory::add(const Particle& particle) {
  // compute types and also check none are skipped.
  const int num_site_type = num_site_types();
  const int num_particle_type = num_particle_types();
  Particle particle_copy = particle;
  if (unique_types_) {
    particle_copy.remove_non_unique_types();
  }
  if (unique_particles_) {
    particle_copy.increment_site_types(num_site_type);
    particle_copy.set_type(num_particle_type);
  }
  particles_.push_back(particle_copy);
  check_site_types();
}

void ParticleFactory::check(const int dimension) const {
  if (particles_.size() != 0) {
    int size = dimension;
    if (size == -1) {
      size = particles_.front().position().size();
    }
    for (Particle particle : particles_) {
      ASSERT(particle.position().size() == size, "size error");
      particle.check();
    }
  }
}

void ParticleFactory::check_types(int * num_site_types,
                            int * num_particle_types) const {
  if (particles_.size() == 0) {
    *num_site_types = 0;
    *num_particle_types = 0;
    return;
  }

  Histogram site_type;
  site_type.set_width_center(1, 0);
  Histogram particle_type(site_type);
  for (const Particle particle : particles_) {
    TRACE("particle type " << particle.type());
    particle_type.add(particle.type());
    for (const Site site : particle.sites()) {
      site_type.add(site.type());
    }
  }
  if (unique_particles_) {
    for (double value : site_type.histogram()) {
      ASSERT(value > 0, "particle skipped a site type");
    }
    for (double value : particle_type.histogram()) {
      ASSERT(value > 0, "skipped a particle type");
    }
  }
  *num_site_types = round(site_type.center_of_last_bin()) + 1;
  *num_particle_types = round(particle_type.center_of_last_bin()) + 1;
}

void ParticleFactory::add(const std::string file_name) {
  ASSERT(unique_particles_,
    "only add particles by file for defining allowed types");
  Particle particle = FileLMP().read(file_name);

  // Assign per-site properties from the data file.
  if (unique_types_) {
    FileLMP().read_properties(file_name, &particle);
  }

  add(particle);

  // Update mole parameters only after the particle has been filtered.
  if (unique_types_) {
    model_params_.add(particles_.back());
  }
}

ParticleFactory& ParticleFactory::unique_particles() {
  unique_particles_ = true;
  check_site_types();
  ASSERT(num_site_types() == 0, "set before adding particles");
  return *this;
}

ParticleFactory& ParticleFactory::unique_types() {
  unique_particles();  // one of each requires unique site types.
  unique_types_ = true;
  ASSERT(num_site_types() == 0, "set before adding particles");
  return *this;
}

const Particle& ParticleFactory::particle(const int particle_index) const {
  ASSERT(particle_index < num(), "size error");
  return particles_[particle_index];
}

int ParticleFactory::num_sites() const {
  int num = 0;
  for (const Particle& particle : particles_) {
    num += particle.num_sites();
  }
  return num;
}

void ParticleFactory::check_types() const {
  int num_site_types, num_particle_types;
  check_types(&num_site_types, &num_particle_types);
}

int ParticleFactory::check_site_types() const {
  int num_site_types, num_particle_types;
  check_types(&num_site_types, &num_particle_types);
  return num_site_types;
}

int ParticleFactory::check_particle_types() const {
  int num_site_types, num_particle_types;
  check_types(&num_site_types, &num_particle_types);
  return num_particle_types;
}

void ParticleFactory::remove(const Group group) {
  for (int index = num() -1;
       index >= 0;
       --index) {
    Particle * part = &particles_[index];
    if (group.is_in(*part)) {
      group.remove_sites(part);
      //*part = group.remove_sites(*part);
    } else {
      particles_.erase(particles_.begin() + index);
    }
  }
}

void ParticleFactory::replace_position(const int particle_index,
                                 const Particle& replacement) {
  particles_[particle_index].replace_position(replacement);
}

void ParticleFactory::replace_position(const int particle_index,
                                 const int site_index,
                                 const Position& replacement) {
  particles_[particle_index].replace_position(site_index, replacement);
}

void ParticleFactory::replace_position(const int particle_index,
                                 const Position& replacement) {
  particles_[particle_index].set_position(replacement);
}

void ParticleFactory::remove(const int particle_index) {
  ASSERT(particle_index < num(), "size error");
  particles_.erase(particles_.begin() + particle_index);
}

void ParticleFactory::serialize(std::ostream& ostr) const {
  feasst_serialize_version(1, ostr);
  feasst_serialize_fstobj(particles_, ostr);
  feasst_serialize(unique_particles_, ostr);
  feasst_serialize(unique_types_, ostr);
  model_params_.serialize(ostr);
}

ParticleFactory::ParticleFactory(std::istream& istr) {
  feasst_deserialize_version(istr);
  feasst_deserialize_fstobj(&particles_, istr);
  feasst_deserialize(&unique_particles_, istr);
  feasst_deserialize(&unique_types_, istr);
  model_params_ = ModelParams(istr);
}

}  // namespace feasst
