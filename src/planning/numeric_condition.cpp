#include "../../include/planning/numeric_condition.hpp"

#include <algorithm>

namespace planning {
  NumericCondition::NumericCondition(ComparatorType comparator_type,
                                     std::shared_ptr<NumericExpression> expression)
      : comparator_type(comparator_type), expression(expression) {
    switch (comparator_type) {
    case ComparatorType::GreaterThan:
      compare = [](double value) { return value > 0; };
      error = [](double value) { return std::max(value, 0.0); };
      break;
    case ComparatorType::GreaterThanOrEqual:
      compare = [](double value) { return value >= 0; };
      error = [](double value) { return std::max(value, 0.0); };
      break;
    case ComparatorType::Equal:
      compare = [](double value) { return value == 0; };
      error = [](double value) { return std::abs(value); };
      break;
    }
  }

  bool NumericCondition::evaluate_formula(const std::vector<double> &values) const {
    double value = expression->evaluate(values);
    return compare(value);
  }

  double NumericCondition::evaluate_error(const std::vector<double> &values) const {
    double value = expression->evaluate(values);
    return error(value);
  }

  std::pair<bool, double>
  NumericCondition::evaluate_formula_and_error(const std::vector<double> &values) const {
    double value = expression->evaluate(values);
    return {compare(value), error(value)};
  }

  std::string NumericCondition::to_string() const {
    std::string comparator;
    switch (comparator_type) {
    case ComparatorType::GreaterThan:
      comparator = ">";
      break;
    case ComparatorType::GreaterThanOrEqual:
      comparator = ">=";
      break;
    case ComparatorType::Equal:
      comparator = "==";
      break;
    }

    return expression->to_string() + " " + comparator + " 0";
  }
}  // namespace planning
