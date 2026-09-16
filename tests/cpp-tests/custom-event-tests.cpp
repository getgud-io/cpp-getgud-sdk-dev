// Serialization contract for the Custom Event action, checked without a running middleware.
// Build against the static library, e.g. from a cmake build dir:
//   c++ -std=c++11 -I../include tests/cpp-tests/custom-event-tests.cpp -L_build -lGetgudSDK -lz -lcurl -o custom-event-tests
#include <cassert>
#include <iostream>
#include <sstream>
#include "GetgudSDK.h"

static std::string serialize(GetgudSDK::CustomEventActionData& action) {
    std::ostringstream oss;
    action.ToString(oss);
    return oss.str();
}

int main() {
    const std::string match = "match-1";
    const long long ts = 1757961234567LL;
    const std::string json = "{\"choiceId\":\"weapon_upgrade\",\"option\":\"damage\",\"level\":3}";
    const std::string jsonBase64 = "eyJjaG9pY2VJZCI6IndlYXBvbl91cGdyYWRlIiwib3B0aW9uIjoiZGFtYWdlIiwibGV2ZWwiOjN9";

    GetgudSDK::CustomEventActionData playerEvent(match, ts, "Player_123", "Choice_Selected", 1, json);
    assert(playerEvent.IsValid());
    assert(serialize(playerEvent) == "1757961234567,C,Player_123,Choice_Selected,1," + jsonBase64 + ",");

    // no player guid -> PvE, no version -> 0, plain string payloads are fine too
    GetgudSDK::CustomEventActionData matchEvent(match, ts, "", "StormPhaseChanged", 0, "phase,2");
    assert(matchEvent.IsValid());
    assert(serialize(matchEvent) == "1757961234567,C,PvE,StormPhaseChanged,0,cGhhc2UsMg==,");

    // payload and version are passed through without validation, an empty payload is an empty field
    GetgudSDK::CustomEventActionData emptyPayload(match, ts, "Player_123", "Choice_Selected", -1, "");
    assert(emptyPayload.IsValid());
    assert(serialize(emptyPayload) == "1757961234567,C,Player_123,Choice_Selected,-1,,");

    // event guids get the same sanitizing as affect guids: bad chars replaced, capped at 36
    GetgudSDK::CustomEventActionData dirtyGuid(match, ts, "Player_123", "Choice$Selected", 0, json);
    assert(dirtyGuid.IsValid());
    assert(serialize(dirtyGuid).find(",Choice_Selected,") != std::string::npos);

    GetgudSDK::CustomEventActionData* clone = playerEvent.Clone();
    assert(serialize(*clone) == serialize(playerEvent));
    delete clone;

    std::cout << "custom-event-tests passed" << std::endl;
    return 0;
}
