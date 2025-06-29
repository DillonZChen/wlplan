#ifndef DATA_DATASET_HPP
#define DATA_DATASET_HPP

#include "../graph_generator/graph.hpp"
#include "../planning/action.hpp"
#include "../planning/problem.hpp"
#include "../planning/state.hpp"

#include <memory>
#include <string>
#include <vector>

namespace data {
  class ProblemDataset {
   public:
    const planning::Problem problem;
    const std::vector<planning::State> states;
    const std::vector<std::vector<planning::Action>> actions;

    ProblemDataset(const planning::Problem &problem, const std::vector<planning::State> &states);
    ProblemDataset(const planning::Problem &problem,
                  const std::vector<planning::State> &states,
                  const std::vector<std::vector<planning::Action>> &actions);
  };

  class DomainDataset {
   public:
    const planning::Domain &domain;
    const std::vector<ProblemDataset> data;

    DomainDataset(const planning::Domain &domain, const std::vector<ProblemDataset> &data);

    size_t get_size() const;

   private:
    std::unordered_map<std::string, int> predicate_to_arity;

    void check_good_atom(const planning::Atom &atom,
                         const std::unordered_set<planning::Object> &objects) const;
  };
}  // namespace data

#endif  // DATA_DATASET_HPP
