#ifndef H_C51A1D89_510C_4FCB_831D_F67363379F2C
#define H_C51A1D89_510C_4FCB_831D_F67363379F2C

#include <pqxx/pqxx>

#include <db_connector/value_collector.h>

namespace srv
{
namespace db
{

using placeholder_t = pqxx::placeholders<uint32_t>;
using params_t = ValueCollector;
using result_t = pqxx::result;

}  // namespace db
}  // namespace srv

#endif  // H_C51A1D89_510C_4FCB_831D_F67363379F2C