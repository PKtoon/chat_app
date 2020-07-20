#include "stream.h"

// #include "external/json/single_include/nlohmann/json.hpp"
#include "nlohmann/json.hpp"

std::string Stream::getSerialized()
{
    nlohmann::json j;
    constexpr Header comp = static_cast<Header>(Header::ack|Header::ping|Header::error|Header::socket_close);

    j["head"] = head;

    if(!((head & comp) || head==Header::empty))
    {
        //switch has fall-through
        switch (head)
        {
        case Header::local_file:
            j["currentPart"] = currentPart;
            j["totalParts"] = totalParts;
        case Header::group_message:
            j["data2"] = data2;
        case Header::message: case Header::init:
            j["data1"] = data1;
            j["receiver"] = receiver;
            j["sender"] = sender;
        default:
            break;
        }
    }
    return j.dump();
}

void Stream::getUnSerialized(std::string& inData)
{
    nlohmann::json j = nlohmann::json::parse(inData);
    constexpr Header comp = static_cast<Header>(Header::ack|Header::ping|Header::error|Header::socket_close);

    head = (j["head"]);

    if(!((head & comp) || head==Header::empty))
    {
        //switch has fall-through
        switch (head)
        {
        case Header::local_file:
            currentPart = j["currentPart"];
            totalParts = j["totalParts"];
        case Header::group_message:
            data2 = j["data2"];
        case Header::message: case Header::init:
            receiver = j["receiver"];
            data1 = j["data1"];
            sender = j["sender"];
        default:
            break;
        }
    }
}
