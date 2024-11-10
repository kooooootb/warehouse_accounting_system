#ifndef H_CDF660B9_96FD_4088_A438_FB35612B30BC
#define H_CDF660B9_96FD_4088_A438_FB35612B30BC

#include <filesystem>

#include <instrumental/common.h>
#include <instrumental/settings.h>

#include <locator/service.h>

namespace srv
{
namespace docmgr
{

class DocumentManagerSettings : public ufa::settings::SettingsBase
{
public:
    SETTINGS_INIT(DocumentManagerSettings)
    {
        SETTINGS_INIT_FIELD(documentRoot);
    }

    SETTINGS_FIELD(documentRoot, std::string);
};

}  // namespace docmgr

struct IDocumentManager : public srv::IService
{
    DECLARE_IID(0X3260C4C8);  // 845202632

    /**
     * @brief Restore the Document object using rel path
     * @param relPath resulting path, containing relative to root path
     * @return ufa::Result: WRONG_FORMAT if normaled path is above root (contains ..), NOT_FOUND if file not exists
     */
    virtual ufa::Result RestoreDocument(std::filesystem::path& relPath, bool checkExistance = false) = 0;

    virtual void SetSettings(const docmgr::DocumentManagerSettings& settings) = 0;
};

}  // namespace srv

#endif  // H_CDF660B9_96FD_4088_A438_FB35612B30BC