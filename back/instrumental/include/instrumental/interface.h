#ifndef H_EA56490C_CF38_4161_85F1_C3FB03A1DD69
#define H_EA56490C_CF38_4161_85F1_C3FB03A1DD69

#include <memory>

#include "types.h"

namespace ufa
{

/**
 * @brief base interface struct, provides virtual destructor
 */
struct IBase
{
    virtual ~IBase() noexcept = default;
};

}  // namespace ufa

#endif  // H_EA56490C_CF38_4161_85F1_C3FB03A1DD69