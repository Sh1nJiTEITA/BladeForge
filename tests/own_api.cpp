#include <catch2/catch_test_macros.hpp>

#include "bfBase.h"
#include "bfEvent.h"
#include "bfWindow.h"

BfEvent return_event() {
    BfSingleEvent se{};
    se.action = BfEnActionType::BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_SUCCESS;
    se.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

    return BfEvent(se);
}

TEST_CASE("BF_EVENT_H", "[single-file]") {
    
    //BfBase base;
    
    int event_number = 6;

    std::vector<BfSingleEvent> events(event_number);
    std::vector<BfEnActionType> event_actions(event_number, BfEnActionType::BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_SUCCESS);
    std::vector<BfEnSingleEventType> event_types(event_number, BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT);

    for (int i = 0; i < event_number; i++) {
        events[i].action = event_actions[i];
        events[i].type = event_types[i];

        BfSingleEvent local_event{ events[i] };

        BfEvent l(local_event);
    }

    return_event();

    //BfEvent(event);
    
    int i_start = 0;
    for (const auto& it : BfEventHandler::single_events) {
        //std::cout << it.code.value() << " " << it.info << " " << it.time << " " << (int)it.type << std::endl;
        
        int i_ = event_number - i_start;

        if (i_ == event_number) {
            REQUIRE(it.action == BfEnActionType::BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_SUCCESS);
            REQUIRE(it.type == BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT);
          
        }
        else {
            REQUIRE(it.action == event_actions[i_]);
            REQUIRE(it.type == event_types[i_]);
        }
        

        i_start++;
    }

}

TEST_CASE("BfHandler-test") {
    bfSetWindowSize(800, 600);
    bfCreateWindow();
    bfCreateInstance();
    bfCreateDebugMessenger();
    bfCreateSurface();

    REQUIRE(BfEventHandler::is_all_ok() == true);
}