#ifndef H_C9709071_DC2B_4057_AA62_4F28461F0846
#define H_C9709071_DC2B_4057_AA62_4F28461F0846

#include <queue>

#include <pqxx/pqxx>

#include <instrumental/string_converters.h>

namespace srv
{
namespace db
{

/**
 * @brief this class collects values, like pqxx::params
 * it was created because we cannot access stored values in pqxx::params and its not comfortable
 */
class ValueCollector
{
public:
    ValueCollector() = default;

    using list_t = std::list<std::string>;

    /**
     * @brief collect value
     * @return ValueCollector& *this for chaining
     */
    template <typename T>
    ValueCollector& Append(T&& value)
    {
        m_values.emplace_back(string_converters::ToString(std::forward<T>(value)));
        return *this;
    }

    /**
     * @brief convert to native pqxx::params
     */
    pqxx::params ToPqxx()
    {
        pqxx::params result;

        while (!m_values.empty())
        {
            result.append(m_values.front());
            m_values.pop_front();
        }

        return result;
    }

    list_t::const_iterator begin() const
    {
        return std::begin(m_values);
    }

    list_t::const_iterator end() const
    {
        return std::end(m_values);
    }

private:
    list_t m_values;
};

}  // namespace db
}  // namespace srv

namespace string_converters
{

inline std::string ToString(const srv::db::ValueCollector& collector)
{
    using namespace std::literals;
    return ToString(collector.begin(), collector.end(), ", "sv);
}

}  // namespace string_converters

#endif  // H_C9709071_DC2B_4057_AA62_4F28461F0846