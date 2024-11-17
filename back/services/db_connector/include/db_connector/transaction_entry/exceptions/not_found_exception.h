#ifndef H_D0021A22_9627_423F_841D_7550E7DFD3A3
#define H_D0021A22_9627_423F_841D_7550E7DFD3A3

#include <stdexcept>

namespace srv
{
namespace db
{
namespace exps
{

class NotFound : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

}  // namespace exps
}  // namespace db
}  // namespace srv

#endif  // H_D0021A22_9627_423F_841D_7550E7DFD3A3