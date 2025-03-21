#ifndef FEATURE_GENERATION_NEIGHBOUR_CONTAINER_HPP
#define FEATURE_GENERATION_NEIGHBOUR_CONTAINER_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

namespace feature_generation {
  class NeighbourContainer {
   public:
    NeighbourContainer(bool multiset_hash);

    virtual ~NeighbourContainer() = default;

    std::string to_string(const std::vector<int> &colours) const;
    std::string to_string() const;

    virtual void clear() = 0;
    virtual void insert(const int arg0, const int arg1) = 0;
    virtual std::vector<int> to_vector() const = 0;

    virtual std::vector<int> get_neighbour_colours(const std::vector<int> &colours) const = 0;

    virtual std::vector<int> remap(const std::vector<int> &input,
                                   const std::map<int, int> &remap) = 0;

   protected:
    const bool multiset_hash;
  };
}  // namespace feature_generation

#endif  // FEATURE_GENERATION_NEIGHBOUR_CONTAINER_HPP
