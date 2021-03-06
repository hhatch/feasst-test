
#ifndef FEASST_MATH_RANDOM_H_
#define FEASST_MATH_RANDOM_H_

#include <vector>
#include <deque>
#include <random>
#include <string>
#include <memory>
#include "utils/include/arguments.h"
#include "utils/include/debug.h"
#include "utils/include/cache.h"
#include "utils/include/utils_io.h"
#include "math/include/position.h"
#include "math/include/constants.h"
#include "math/include/matrix.h"

namespace feasst {

/**
  Psuedo random number generator class.
  Initialize the random number seed by global function methods

  1. seed_random_by_date()
  2. seed_random(seed)

  Note that all other Random distributions depend upon the uniform distribution,
  such that reproduction by storage is simplified.
 */
class Random {
 public:
  Random(
    /**
      seed : Provide an integer to seed the random number generator.
             If the string "date" is provided, then use the date to generate
             the seed.
             If no seed is provided, the date will be used.
             If the string "default" is provided, then use the default integer
             included in the following seed_random function definition.
     */
    const argtype& args = argtype());

  /// Initialize random number generator based on date and time.
  void seed_random_by_date();

  /// Initialize random number generator to seed value for reproducibility.
  void seed_random(const int seed = 1346867550);

  /// Return a random real number with a uniform probability distribution
  /// between 0 and 1.
  double uniform();

  /// Return a random integer with a uniform probability distribution
  /// betwee min and max.
  int uniform(const int min, const int max);

  /// Randomly return true or false
  bool coin_flip();

  /// Return random real number with a uniform probability distribution
  /// between min and max.
  double uniform_real(const double min, const double max);

  /// Return a random index of a vector.
  template<class T>
  const int index(const std::vector<T>& vector) {
    ASSERT(vector.size() > 0, "size error");
    const int vec_index = uniform(0, vector.size() - 1);
    return vec_index;
  }

  /// Return a pointer to a random element within a vector.
  template<class T>
  T * element(std::vector<T> * vector,
    /// optionally return the index associated with the element.
    int * return_index = NULL) {
    const int vec_index = index(*vector);
    if (return_index != NULL) {
      *return_index = vec_index;
    }
    return &(*vector)[vec_index];
  }

  /// Return a constant reference to a random element within a vector.
  template<class T>
  const T& const_element(const std::vector<T>& vector,
    /// optionally return the index associated with the element.
    int * return_index = NULL) {
    const int vec_index = index(vector);
    if (return_index != NULL) {
      *return_index = vec_index;
    }
    return vector[vec_index];
  }

  /// Return a random alpha numeric string of given length.
  std::string alpha_numeric(const int length = 5);

  /// Return a random position within a cube of side length with the origin
  /// at the center.
  Position position_in_cube(const int dimension, const double length = 1);

  /// Optimized version of the above, in that an existing position is modified.
  void position_in_cube(const int dimension, const double length, Position * position);

  /// Return a random position within a cuboid of side lengths with the origin
  /// at the center.
  Position position_in_cuboid(const Position& side_length);

  /// Optimized version of the above, in that an existing position is modified.
  void position_in_cuboid(const Position& side_length, Position * position);

  /// Random point on the surface of a unit sphere.
  void unit_sphere_surface(Position * position);

  /// Given a cumulative probability distribution, return a random integer
  /// index from a uniform probability distribution.
  /// The cumulative distribution must be monotonically nondecreasing.
  /// In addition, it must end with the value of unity.
  int index_from_cumulative_probability(const std::vector<double>& cumulative);

  /// Return a random rotation matrix.
  RotationMatrix rotation(
    /// dimensionality of space
    const int dimension,
    /// maximum angle of rotation in degrees.
    const double max_angle = 180);

  /// Return the cache.
  const Cache& cache() const { return cache_; }

  /// Set Cache to load.
  void set_cache_to_load(const bool load) { cache_.set_load(load); }

  /// Set Cache to unload.
  void set_cache_to_unload(const Random& random) {
    cache_.set_unload(random.cache()); }

  /// Serialize.
  std::string class_name() const { return class_name_; }
  virtual void serialize(std::ostream& ostr) const = 0;
  virtual std::shared_ptr<Random> create(std::istream& istr) const = 0;
  std::map<std::string, std::shared_ptr<Random> >& deserialize_map();
  std::shared_ptr<Random> deserialize(std::istream& istr);
  virtual ~Random() {}

 protected:
  std::string class_name_ = "Random";
  void serialize_random_(std::ostream& ostr) const;
  Random(std::istream& istr);

 private:
  Cache cache_;

  virtual void reseed_() = 0;
  virtual double gen_uniform_() = 0;
  virtual int gen_uniform_(const int min, const int max) = 0;
};

}  // namespace feasst

#endif  // FEASST_MATH_RANDOM_H_
