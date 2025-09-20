#include "../../include/planning/numeric_expression.hpp"

namespace wlplan {
  namespace planning {
    /* FormulaExpression */
    FormulaExpression::FormulaExpression(OperatorType op_type,
                                         std::shared_ptr<NumericExpression> expr_a,
                                         std::shared_ptr<NumericExpression> expr_b)
        : expr_a(expr_a), expr_b(expr_b) {
      switch (op_type) {
      case OperatorType::Plus:
        op = [](double a, double b) { return a + b; };
        op_symbol = "+";
        break;
      case OperatorType::Minus:
        op = [](double a, double b) { return a - b; };
        op_symbol = "-";
        break;
      case OperatorType::Multiply:
        op = [](double a, double b) { return a * b; };
        op_symbol = "*";
        break;
      case OperatorType::Divide:
        op = [](double a, double b) { return a / b; };
        op_symbol = "/";
        break;
      }
    }

    double FormulaExpression::evaluate(const std::vector<double> &values) const {
      double a = expr_a->evaluate(values);
      double b = expr_b->evaluate(values);
      return op(a, b);
    }

    std::vector<int> FormulaExpression::get_fluent_ids() const {
      std::vector<int> ids_a = expr_a->get_fluent_ids();
      std::vector<int> ids_b = expr_b->get_fluent_ids();
      ids_a.insert(ids_a.end(), ids_b.begin(), ids_b.end());
      return ids_a;
    }

    std::string FormulaExpression::to_string() const {
      return "(" + expr_a->to_string() + " " + op_symbol + " " + expr_b->to_string() + ")";
    }

    /* ConstantExpression */

    ConstantExpression::ConstantExpression(double value) : value(value) {}
    double ConstantExpression::evaluate(const std::vector<double> &values) const {
      (void)values;  // to avoid unused variable warnings
      return value;
    }
    std::vector<int> ConstantExpression::get_fluent_ids() const { return {}; }
    std::string ConstantExpression::to_string() const { return std::to_string(value); }

    /* FluentExpression */

    FluentExpression::FluentExpression(int id, std::string fluent_name)
        : id(id), fluent_name(fluent_name) {}
    double FluentExpression::evaluate(const std::vector<double> &values) const {
      return values[id];
    }
    std::vector<int> FluentExpression::get_fluent_ids() const { return {id}; }
    std::string FluentExpression::to_string() const { return fluent_name; }

  }  // namespace planning
}  // namespace wlplan
