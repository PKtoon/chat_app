#include "stream.h"

// #include "external/json/single_include/nlohmann/json.hpp"
#include "nlohmann/json.hpp"

std::string Stream::getSerialized()
{
    nlohmann::json j;
    constexpr Header comp = static_cast<Header>(Header::ACK|Header::PING|Header::ERROR|Header::SOCKET_CLOSE);

    j["head"] = head;

    if((head & comp) || head==Header::EMPTY)
    {
        j["sender"] = sender;
        j["receiver"] = receiver;
    }
    else
    {
        //switch has fall-through
        switch (head)
        {
        case Header::LOCAL_FILE:
            j["currentPart"] = currentPart;
            j["totalParts"] = totalParts;
        case Header::GROUP_MESSAGE:
            j["data2"] = data2;
        case Header::MESSAGE:
            j["receiver"] = receiver;
            j["data1"] = data1;
        case Header::INIT:
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
    constexpr Header comp = static_cast<Header>(Header::ACK|Header::PING|Header::ERROR|Header::SOCKET_CLOSE);

    head = (j["head"]);

    if((head & comp) || head==Header::EMPTY)
    {
        sender = j["sender"];
        receiver = j["receiver"];
    }
    else
    {
        //switch has fall-through
        switch (head)
        {
        case Header::LOCAL_FILE:
            currentPart = j["currentPart"];
            totalParts = j["totalParts"];
        case Header::GROUP_MESSAGE:
            data2 = j["data2"];
        case Header::MESSAGE:
            receiver = j["receiver"];
            data1 = j["data1"];
        case Header::INIT:
            sender = j["sender"];
        default:
            break;
        }
    }
}
