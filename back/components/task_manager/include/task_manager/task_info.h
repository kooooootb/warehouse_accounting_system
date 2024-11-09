#ifndef H_0A34CCB5_A237_4DF0_A9C7_0ABA0680D38C
#define H_0A34CCB5_A237_4DF0_A9C7_0ABA0680D38C

#include <instrumental/common.h>

#include "callback.h"
#include "task_identificator.h"

namespace taskmgr
{

/**
 * @brief contains info necessary for executing task
 * 
 */
struct TaskInfo
{
    /**
     * @brief task identificator
     */
    TaskIdentificator identificator;

    /**
     * @brief json string with task options
     */
    std::string body;

    /**
     * @brief callback which will called with task results after its execution
     */
    callback_t callback;

    /**
     * @brief userid of the initiative user
     */
    userid_t initiativeUserid;
};

}  // namespace taskmgr

#endif  // H_0A34CCB5_A237_4DF0_A9C7_0ABA0680D38C