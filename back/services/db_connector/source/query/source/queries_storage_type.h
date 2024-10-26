#ifndef H_656FCD52_9AAB_4D96_84B6_096146DFE469
#define H_656FCD52_9AAB_4D96_84B6_096146DFE469

#include <db_connector/query/query_identificator.h>

#include <query/query_manager.h>

namespace srv
{
namespace db
{
namespace qry
{

using queriesStorage_t = std::map<QueryIdentificator, std::vector<std::pair<queryid_t, std::unique_ptr<IQueryOptions>>>>;

}  // namespace qry
}  // namespace db
}  // namespace srv

#endif  // H_656FCD52_9AAB_4D96_84B6_096146DFE469
