#ifndef DATA_DATASET_HPP
#define DATA_DATASET_HPP

#include "../graph/graph.hpp"
#include "../graph/ilg_generator.hpp"
#include "../planning/problem.hpp"
#include "../planning/state.hpp"
#include "../planning/state_verbose.hpp"

#include <memory>
#include <string>
#include <vector>

namespace data {
  class ProblemStates {
   public:
    planning::Problem problem;
    std::vector<planning::State> states;

    ProblemStates(const planning::Problem &problem, const std::vector<planning::State> &states)
        : problem(problem), states(states){};

    ProblemStates(const planning::Problem &problem,
                  std::vector<planning::StateVerbose> &states_verbose)
        : problem(problem) {
      planning::Domain domain = problem.get_domain();
      std::unordered_map<std::string, int> predicate_to_id = domain.predicate_to_colour;
      std::unordered_map<std::string, int> object_to_id = problem.get_object_to_id();
      states = std::vector<planning::State>();
      for (const auto &state_verbose : states_verbose) {
        std::vector<planning::Atom> compact_atoms;
        for (const auto &atom_verbose : state_verbose.atoms) {
          // Convert AtomVerbose to Atom
          std::vector<int> object_ids;
          for (const auto &object : atom_verbose->objects) {
            if (object_to_id.find(object) == object_to_id.end()) {
              throw std::runtime_error("Unknown object " + object);
            }
            object_ids.push_back(object_to_id.at(object));
          }
          planning::Atom atom(predicate_to_id.at(atom_verbose->predicate.name), object_ids);
          compact_atoms.push_back(atom);
        }

        planning::State state(compact_atoms, state_verbose.values);
        states.push_back(state);
      }
    }
  };

  class Dataset {
   public:
    const planning::Domain &domain;
    const std::vector<ProblemStates> data;

    Dataset(const planning::Domain &domain, const std::vector<ProblemStates> &data);

    size_t get_size() const;

   private:
    std::unordered_map<std::string, int> predicate_to_arity;

    void check_good_atom(const planning::Atom &atom,
                         const std::unordered_set<int> &objects,
                         const planning::Problem &problem) const;
  };
}  // namespace data

#endif  // DATA_DATASET_HPP
