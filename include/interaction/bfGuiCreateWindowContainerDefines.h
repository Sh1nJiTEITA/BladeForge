#ifndef BF_GUI_CREATE_WINDOW_CONTAINER_DEFINES
#define BF_GUI_CREATE_WINDOW_CONTAINER_DEFINES

#include <bfDrawObjectDefineType.h>
#include <bfGreekFont.h>
#include <bfIconsFontAwesome6.h>
#include <bfLayerHandler.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <functional>
#include <memory>
#include <string>
#include <typeindex>

class BfConfigManager;
class BfGuiCreateWindow;
class BfGuiCreateWindowContainer;
class BfGuiCreateWindowContainerObj;
class BfGuiCreateWindowContainerPopup;

// === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === ===

using BfGuiCreateWindowMake =
    std::function<std::shared_ptr<BfGuiCreateWindowContainer>()>;

struct BfGuiCreateWindowTypeRegistryStruct
{
   std::string name;
   BfGuiCreateWindowMake make;
};

class BfGuiCreateWindowTypeRegistry
{
public:
   // clang-format off
   using TypeMap = std::map<std::type_index, BfGuiCreateWindowTypeRegistryStruct>;
   // clang-format on

   static BfGuiCreateWindowTypeRegistry& instance()
   {
      static BfGuiCreateWindowTypeRegistry reg;
      return reg;
   }

   void registerType(std::type_index index, const std::string& name)
   {
      __typeMap.emplace(index, BfGuiCreateWindowTypeRegistryStruct(name, []() {
                           return std::make_shared<BfGuiCreateWindowContainer>(
                               std::weak_ptr<BfGuiCreateWindowContainer>()
                           );
                        }));
   }

   // const TypeMapStr& getStrMap() const { return __typeMap; }
   const TypeMap& get() const { return __typeMap; }
   TypeMap& __get() { return __typeMap; }
   // const TypeMap& getMapStr() const { return __typeMap; }

private:
   TypeMap __typeMap;
};

class BfGuiCreateWindowAutoRegister
{
public:
   BfGuiCreateWindowAutoRegister(std::type_index index, const std::string& name)
   {
      BfGuiCreateWindowTypeRegistry::instance().registerType(index, name);
   }
};

#define BF_GUI_CREATE_WINDOW_REGISTER_TYPE(TYPE) \
   static BfGuiCreateWindowAutoRegister _register_type_##TYPE

#define BF_GUI_CREATE_WINDOW_REGISTER_TYPE_SOURCE(TYPE)               \
   inline BfGuiCreateWindowAutoRegister TYPE::_register_type_##TYPE = \
       BfGuiCreateWindowAutoRegister(std::type_index(typeid(TYPE)), #TYPE)

// === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === ===

// clang-format off
#define BfGuiCreateWindowContainer_ButtonType_All (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3)
#define BfGuiCreateWindowContainer_ButtonType_Left (1 << 0)
#define BfGuiCreateWindowContainer_ButtonType_Right (1 << 1)
#define BfGuiCreateWindowContainer_ButtonType_Top (1 << 2)
#define BfGuiCreateWindowContainer_ButtonType_Bot (1 << 3)

using swapFuncType = std::function<void(const std::string&, const std::string&)>;
using moveFuncType = std::function<void(const std::string&, const std::string&)>;
using ptrContainer = std::shared_ptr<BfGuiCreateWindowContainer>;
using wptrContainer = std::weak_ptr<BfGuiCreateWindowContainer>;
// clang-format on

#define BfGuiCreateWindowContainerPopup_Side_Left 0
#define BfGuiCreateWindowContainerPopup_Side_Right 1
#define BfGuiCreateWindowContainerPopup_Side_Top 2
#define BfGuiCreateWindowContainerPopup_Side_Bot 3

#endif
