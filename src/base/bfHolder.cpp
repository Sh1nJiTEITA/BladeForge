#include "bfHolder.h"

BfHolder* BfHolder::__bfpHolder = nullptr;

BfEvent bfBindHolder(BfHolder* holder)
{
   BfHolder::__bfpHolder = holder;

   BfSingleEvent event{};
   event.type   = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_HOLDER_EVENT;
   event.action = BfEnActionType::BF_ACTION_TYPE_BIND_HOLDER;
   return BfEvent(event);
}

BfHolder::BfHolder() { __bfpHolder = this; }

BfHolder* bfGetpHolder() { return BfHolder::__bfpHolder; }

BfEvent bfHoldWindow(BfWindow*& window)
{
   if (window == nullptr)
   {
      BfHolder::__bfpHolder->windows.push_back(BfWindow());
      BfHolder::__bfpHolder->windows.back().holder_index =
          BfHolder::__bfpHolder->windows.size() - 1;
      window = &BfHolder::__bfpHolder->windows.back();
   }
   else
   {
      BfHolder::__bfpHolder->windows.push_back(*window);
      BfHolder::__bfpHolder->windows.back().holder_index =
          BfHolder::__bfpHolder->windows.size() - 1;
      window = &BfHolder::__bfpHolder->windows.back();
   }

   BfSingleEvent event{};
   {
      event.type   = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_HOLDER_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_HOLDER_ADD_BF_WINDOW;
   }
   return BfEvent(event);
}

BfEvent bfHoldPhysicalDevice(BfPhysicalDevice*& physical_device)
{
   if (physical_device == nullptr)
   {
      BfHolder::__bfpHolder->physical_devices.push_back(BfPhysicalDevice());
      BfHolder::__bfpHolder->physical_devices.back().holder_index =
          BfHolder::__bfpHolder->physical_devices.size() - 1;
      physical_device = &BfHolder::__bfpHolder->physical_devices.back();
   }
   else
   {
      BfHolder::__bfpHolder->physical_devices.push_back(*physical_device);
      BfHolder::__bfpHolder->physical_devices.back().holder_index =
          BfHolder::__bfpHolder->physical_devices.size() - 1;
      physical_device = &BfHolder::__bfpHolder->physical_devices.back();
   }

   BfSingleEvent event{};
   {
      event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_HOLDER_EVENT;
      event.action =
          BfEnActionType::BF_ACTION_TYPE_HOLDER_ADD_BF_PHYSICAL_DEVICE;
   }
   return BfEvent(event);
}
