#ifndef H_27F7780D_D408_4BDD_A5B4_A58EA86EFE77
#define H_27F7780D_D408_4BDD_A5B4_A58EA86EFE77

#include <tuple>

#include "add_req_in_spec/task.h"
#include "add_user/task.h"
#include "approve_requirement/task.h"
#include "authorization/task.h"
#include "create_dependency/task.h"
#include "create_project/task.h"
#include "create_requirement/task.h"
#include "create_specification/task.h"
#include "edit_requirement/task.h"
#include "generate_report/task.h"
#include "get_project_info/task.h"
#include "get_projects/task.h"
#include "reject_requirement/task.h"
#include "send_spec_to_approve/task.h"

namespace taskmgr
{
namespace tasks
{

using TasksList = std::tuple<Authorization,
    CreateRequirement,
    AddReqInSpec,
    AddUser,
    CreateDependency,
    CreateSpecification,
    GetProjectInfo,
    GetProjects,
    CreateProject,
    EditRequirement,
    ApproveSpecification,
    RejectSpecification,
    SendSpecToApprove,
    GenerateReport>;

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_27F7780D_D408_4BDD_A5B4_A58EA86EFE77