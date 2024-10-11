#ifndef H_CDF660B9_96FD_4088_A438_FB35612B30BC
#define H_CDF660B9_96FD_4088_A438_FB35612B30BC

#include <filesystem>

#include <instrumental/interface.h>
#include <instrumental/types.h>

namespace srv
{

struct IDocumentManager : public ufa::IBase
{
    /**
     * @brief Restore the Document object using rel path
     * @param relPath resulting path, containing relative to root path
     * @return ufa::Result: WRONG_FORMAT if normaled path is above root (contains ..), NOT_FOUND if file not exists
     */
    virtual ufa::Result RestoreDocument(std::filesystem::path& relPath, bool checkExistance = false) = 0;

    virtual void SetRoot(std::filesystem::path&& rootPath) = 0;
};

}  // namespace srv

#endif  // H_CDF660B9_96FD_4088_A438_FB35612B30BC