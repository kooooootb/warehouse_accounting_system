#ifndef H_F77F9905_8417_44BB_BEF3_7C92E4487685
#define H_F77F9905_8417_44BB_BEF3_7C92E4487685

#include <optional>
#include <string>

namespace taskmgr
{
namespace tasks
{

struct InvoiceItem
{
    std::optional<int64_t> invoice_id;
    std::optional<int64_t> product_id;
    std::optional<int64_t> count;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_F77F9905_8417_44BB_BEF3_7C92E4487685