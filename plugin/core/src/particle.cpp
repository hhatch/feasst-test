#include "core/include/particle.h"
#include "core/include/debug.h"
#include "core/include/utils_io.h"
#include "core/include/utils.h"

namespace feasst {

void Particle::displace(const Position& displacement) {
  add_position(displacement);
  for (Site& site : sites_) {
    site.displace(displacement);
  }
}

void Particle::default_particle() {
  default_position();
  Site site;
  site.default_site();
  site.set_position(position());
  add(site);
  set_type(0);
}

void Particle::check_size() {
  for (Site site : sites_) {
    ASSERT(position().size() == site.position().size(), "size error");
  }
}

void Particle::remove_non_unique_types() {
  std::vector<std::string> names = {"site", "bond"};
  for (std::string name : names) {
    int num = -1;
    if (name == "site") {
      num = num_sites();
    } else if (name == "bond") {
      num = num_bonds();
    } else {
      ERROR("unrecognized");
    }
    std::vector<int> to_remove;
    std::vector<int> types_visited;
    for (int index = 0; index < num; ++index) {
      int type;
      if (name == "site") {
        type = sites_[index].type();
      } else if (name == "bond") {
        type = bonds_[index].type();
      } else {
        ERROR("unrecognized");
      }
      if (!find_in_list(type, types_visited)) {
        types_visited.push_back(type);
      } else {
        to_remove.push_back(index);
      }
    }
    for (int index = num - 1; index >= 0; --index) {
      if (find_in_list(index, to_remove)) {
        if (name == "site") {
          sites_.erase(sites_.begin()  + index);
        } else if (name == "bond") {
          bonds_.erase(bonds_.begin()  + index);
        }
      }
    }
  }
}

void Particle::remove_site(const int index) {
  sites_.erase(sites_.begin() + index);
  if (reference_sites_.size() != 0) {
    reference_sites_.erase(sites_.begin() + index);
  }
}

void Particle::increment_site_types(const int increment) {
  for (Site& site : sites_) {
    site.set_type(site.type() + increment);
  }
}

bool Particle::is_isotropic() {
  if (num_sites() <= 1) { return true; } else { return false; }
}

void Particle::replace_position(const Particle& particle) {
  set_position(particle.position());
  ASSERT(num_sites() == particle.num_sites(), "size error");
  for (int site_index = 0; site_index < num_sites(); ++site_index) {
    sites_[site_index].set_position(particle.site(site_index).position());
  }
}

void Particle::replace_position(const int site_index,
                                const Position& replacement) {
  sites_[site_index].set_position(replacement);
}

void Particle::update_cell(const Domain& domain, const int site_index) {
  for (const Cells& cells : domain.cells()) {
    if (cells.enabled()) {
      if (site_index == -1) {
        for (Site& site : sites_) {
          update_cell_of_site_(cells, domain, &site);
        }
      } else {
        update_cell_of_site_(cells, domain, &sites_[site_index]);
      }
    }
  }
}

void Particle::update_cell_of_site_(const Cells& cells,
                                    const Domain& domain,
                                    Site * site) {
  site->add_or_set_property(cells.label(),
                            domain.cell_id(site->position(), cells));
}

}  // namespace feasst