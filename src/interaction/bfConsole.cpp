#include "bfConsole.h"

// std::string BfConsole::make_str(VkExtent2D extent2d)
//{
//	std::stringstream ss;
//	ss //<< "VkExtent2D(w = "
//		<< extent2d.width
//		<< ", "
//		// <<"h = "
//		<< extent2d.height
//		//<< ") "
//		;
//	return ss.str();
// }
//
// std::string BfConsole::make_str(VkSurfaceTransformFlagBitsKHR in)
//{
//	switch (in) {
//		case VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR:
//			return "VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR";
//		case VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR:
//			return "VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR";
//		case VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR:
//			return "VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR";
//		case VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR:
//			return "VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR";
//		case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR:
//			return "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR";
//		case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR:
//			return
//"VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR"; 		case
// VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR:
// return "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR";
// case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR:
// return "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR";
// case VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR: 			return
//"VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR"; 		default:
// return "INVALID";
//	}
// }
//
// std::string BfConsole::make_str(VkCompositeAlphaFlagBitsKHR in)
//{
//	switch (in) {
//		case VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR:
//			return "VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR";
//		case VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR:
//			return "VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR";
//		case VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR:
//			return "VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR";
//		case VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR:
//			return "VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR";
//		default:
//			return "INVALID";
//	}
// }
//
// std::string BfConsole::make_str(VkImageUsageFlagBits in)
//{
//	switch (in) {
//		case VK_IMAGE_USAGE_TRANSFER_SRC_BIT:
//			return "VK_IMAGE_USAGE_TRANSFER_SRC_BIT";
//		case VK_IMAGE_USAGE_TRANSFER_DST_BIT:
//			return "VK_IMAGE_USAGE_TRANSFER_DST_BIT";
//		case VK_IMAGE_USAGE_SAMPLED_BIT:
//			return "VK_IMAGE_USAGE_SAMPLED_BIT";
//		case VK_IMAGE_USAGE_STORAGE_BIT:
//			return "VK_IMAGE_USAGE_STORAGE_BIT";
//		case VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT:
//			return "VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT";
//		case VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT:
//			return "VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT";
//		case VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT:
//			return "VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT";
//		case VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT:
//			return "VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT";
//		case VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR:
//			return "VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR";
//		case VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR:
//			return "VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR";
//		case VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR:
//			return "VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR";
//		case VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT:
//			return "VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT";
//		case VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR:
//			return
//"VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR|VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV";
//		case VK_IMAGE_USAGE_HOST_TRANSFER_BIT_EXT:
//			return "VK_IMAGE_USAGE_HOST_TRANSFER_BIT_EXT";
// #ifdef VK_ENABLE_BETA_EXTENSIONS
//		case VK_IMAGE_USAGE_VIDEO_ENCODE_DST_BIT_KHR:
//			return "VK_IMAGE_USAGE_VIDEO_ENCODE_DST_BIT_KHR";
//		case VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR:
//			return "VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR";
//		case VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR:
//			return "VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR";
// #endif
//		case VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT:
//			return
//"VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT"; 		case
// VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI: 			return
//"VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI"; 		case
// VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM: 			return
//"VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM"; 		case
// VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM: 			return
//"VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM"; 		default:
// return "INVALID";
//	}
// }
//
// std::string BfConsole::make_str(VkSurfaceCapabilitiesKHR in)
//{
//	std::stringstream ss;
//	ss << "currentExtent\t= "		<<
// BfConsole::make_str(in.currentExtent)	<< "\n"; 	ss <<
// "currentTransform\t= "
//<< BfConsole::make_str(in.currentTransform) << "\n"; 	ss <<
//"maxImageExtent\t="
//<< BfConsole::make_str(in.maxImageExtent)	<< "\n"; 	ss <<
//"minImageExtent\t="		<< BfConsole::make_str(in.minImageExtent)
//<< "\n"; 	ss << "maxImageArrayLayers\t="  << in.maxImageArrayLayers
//<< "\n"; 	ss << "maxImageCount\t="		<< in.maxImageCount
//<< "\n"; 	ss << "minImageCount"			<< in.minImageCount
//<< "\n"; 	ss << "minImageCount" << in.supportedCompositeAlpha	 <<
//"\n"; 	ss << "minImageCount" << in.minImageCount << "\n"; 	ss <<
//"minImageCount" << in.minImageCount << "\n";
//
//
// }

void
BfConsole::print_all_single_events(int types)
{
   auto getTimeStr = [](const time_t& timeValue) {
      struct tm* timeInfo = std::localtime(&timeValue);
      int hours = timeInfo->tm_hour;
      int minutes = timeInfo->tm_min;
      int seconds = timeInfo->tm_sec;

      std::stringstream ss;
      ss << "[" << hours << ":" << minutes << ":" << seconds << "]";
      return ss.str();
   };

   for (const auto& it : BfEventHandler::single_events)
   {

      if (!(types & (int)it.type))
         continue;

      auto it_standart_messege = bfSetActionsStr.find((int)it.action);
      if (it_standart_messege != bfSetActionsStr.end())
      {
         fmt::print(
             "{} {} {}\n",
             getTimeStr(it.time),
             it_standart_messege->second,
             it.info
         );
      }
      else
      {
         fmt::print("INVALID_PRINT_PARAMETERS\n");
      }
   }
}

void
BfConsole::print_single_single_event(BfSingleEvent event)
{
   auto getTimeStr = [](const time_t& timeValue) {
      struct tm* timeInfo = std::localtime(&timeValue);
      int hours = timeInfo->tm_hour;
      int minutes = timeInfo->tm_min;
      int seconds = timeInfo->tm_sec;

      std::stringstream ss;
      ss << "[" << hours << ":" << minutes << ":" << seconds << "]";
      return ss.str();
   };

   auto it_standart_messege = bfSetActionsStr.find((int)event.action);
   if (it_standart_messege != bfSetActionsStr.end())
   {
      fmt::print(
          "{} {} {}\n",
          getTimeStr(event.time),
          it_standart_messege->second,
          event.info
      );
   }
   else
   {
      fmt::print("INVALID_PRINT_PARAMETERS\n");
   }
}

std::string
BfConsole::print_single_single_event_to_string(BfSingleEvent event)
{
   static std::stringstream ss;

   /*auto getTimeStr = [](const time_t& timeValue) {
           struct tm* timeInfo = std::localtime(&timeValue);
           int hours = timeInfo->tm_hour;
           int minutes = timeInfo->tm_min;
           int seconds = timeInfo->tm_sec;


           ss << "[" << hours << ":" << minutes << ":" << seconds << "]";
           return ss.str();
   };*/

   auto it_standart_messege = bfSetActionsStr.find((int)event.action);
   if (it_standart_messege != bfSetActionsStr.end())
   {
      struct tm* timeInfo = std::localtime(&event.time);
      int hours = timeInfo->tm_hour;
      int minutes = timeInfo->tm_min;
      int seconds = timeInfo->tm_sec;

      ss << "[" << hours << ":" << minutes << ":" << seconds << "] ";

      ss << it_standart_messege->second << " " << event.info << "\n";
   }
   else
   {
      ss << "INVALID_PRINT_PARAMETERS" << "\n";
   }

   return ss.str();
}

std::string
BfConsole::print_single_single_event_to_string_single(BfSingleEvent event)
{
   return std::string();
}
