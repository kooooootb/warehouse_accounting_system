#ifndef H_D4AFC819_7297_4302_BC08_35144F1768C4
#define H_D4AFC819_7297_4302_BC08_35144F1768C4

namespace srv
{
namespace db
{
namespace qry
{

/**
     * @brief allows iterating through queries props
     * @warning will lock queries internal mutex so dispose is asayc
     */
class QueriesLock
{
public:
    QueriesLock(std::mutex& queriesMutex, const std::map<uint64_t, std::unique_ptr<IQueryOptions>>& queries);

private:
    const std::map<uint64_t, std::unique_ptr<IQueryOptions>>& m_queries;
    std::lock_guard<std::mutex> m_lock;
};

}  // namespace qry
}  // namespace db
}  // namespace srv

#endif  // H_D4AFC819_7297_4302_BC08_35144F1768C4