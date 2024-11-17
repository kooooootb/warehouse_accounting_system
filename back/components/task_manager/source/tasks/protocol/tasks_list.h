#ifndef H_27F7780D_D408_4BDD_A5B4_A58EA86EFE77
#define H_27F7780D_D408_4BDD_A5B4_A58EA86EFE77

#include <tuple>

#include "authorization/authorization.h"
#include "create_user/create_user.h"
#include "create_warehouse/create_warehouse.h"
#include "products_create/products_create.h"
#include "products_move/products_move.h"
#include "products_remove/products_remove.h"
#include "reports_by_period/reports_by_period.h"

namespace taskmgr
{
namespace tasks
{

using TasksList =
    std::tuple<Authorization, CreateUser, ProductsCreate, ProductsMove, ProductsRemove, ReportsByPeriod, CreateWarehouse>;

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_27F7780D_D408_4BDD_A5B4_A58EA86EFE77