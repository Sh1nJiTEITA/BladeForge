#include "bfEvent.h"

// Static members ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

std::list<BfSingleEvent> BfEventHandler::single_events{};
std::list<std::string> BfEventHandler::single_event_time{};


BfEventHandler::FunctionPointer BfEventHandler::funcPtr = nullptr;
BfEventHandler::FunctionPointerStr BfEventHandler::funcPtrStr = nullptr;
std::string BfEventHandler::whole_log = "";
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Static members //

// BfEventHandler ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
void BfEventHandler::add_single_event(BfSingleEvent event)
{
	if (funcPtr != nullptr) {
		// Вызываем функцию
		funcPtr(event);
	}
	if (funcPtrStr != nullptr) {
		BfEventHandler::whole_log = funcPtrStr(event);
	}
	single_events.push_front(event);
	single_event_time.push_front(time_t_to_text(event.time));
}

void BfEventHandler::add_multiple_event(BfMultipleEvent event)
{
}

const std::list<BfSingleEvent>& BfEventHandler::get_single_events()
{
	return single_events;
}

std::time_t BfEventHandler::get_time()
{
	auto time = std::chrono::system_clock::now();
	return std::chrono::system_clock::to_time_t(time);;
}
bool BfEventHandler::is_all_ok()
{
	for (const auto& event : BfEventHandler::single_events) {
		int action_index = static_cast<int>(event.action);
		if (action_index < 0) {
			std::stringstream ss;
			auto action = bfSetActionsStr.find(action_index);

			ss << "LOCAL EXCEPTION: " << action->second << event.info;

			throw std::runtime_error(ss.str());
			
			return false;
		}
	}

	return true;
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ BfEventHandler //



bool BfMultipleEvent::operator<(const BfMultipleEvent& other) const
{
	return this->type < other.type;
}

// BfEvent ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

BfEvent::BfEvent() : 
	current_event_type{BfEnEventType::BF_EVENT_TYPE_INVALID},
	single_event{},
	multiple_event{}
{}

BfEvent::BfEvent(BfSingleEvent single_event_) :
	current_event_type{BfEnEventType::BF_EVENT_TYPE_SINGLE},
	multiple_event{}
{
	single_event = single_event_;
	single_event.time = BfEventHandler::get_time();
	record_event();
}

BfEvent::BfEvent(BfMultipleEvent multiple_event_) :
	current_event_type{ BfEnEventType::BF_EVENT_TYPE_MULTIPLE },
	multiple_event{multiple_event_},
	single_event{}
{
	record_event();
}

void BfEvent::record_event()
{
	switch (this->current_event_type) {
	case BfEnEventType::BF_EVENT_TYPE_INVALID:
		break;
	
	case BfEnEventType::BF_EVENT_TYPE_SINGLE:
		BfEventHandler::add_single_event(this->single_event);
		break;

	case BfEnEventType::BF_EVENT_TYPE_MULTIPLE:
		BfEventHandler::add_multiple_event(this->multiple_event);
		break;
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ BfEvent //

std::string time_t_to_text(const time_t& time) {
	struct tm* timeInfo = std::localtime(&time);
	int hours = timeInfo->tm_hour;
	int minutes = timeInfo->tm_min;
	int seconds = timeInfo->tm_sec;

	std::stringstream ss;
	ss << "[" << hours << ":" << minutes << ":" << seconds << "]";
	return ss.str();
}