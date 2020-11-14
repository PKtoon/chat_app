#include "stream.hpp"

// #include "external/json/single_include/nlohmann/json.hpp"
#include "nlohmann/json.hpp"

std::string Stream::getSerialized()
{
    nlohmann::json j;

    j["head"] = head;
    j["sender"] = sender;
    j["receiver"] = receiver;
    j["data1"] = data1;
    j["msgID"] = msgID;
    j["currentPart"] = currentPart;
    j["totalParts"] = totalParts;
    j["data2"] = data2;

    return j.dump();
}

void Stream::getUnSerialized(std::string& inData)
{
    nlohmann::json j = nlohmann::json::parse(inData);

    head = j["head"];
    sender = j["sender"];
    receiver = j["receiver"];
    data1 = j["data1"];
    msgID = j["msgID"];
    currentPart = j["currentPart"];
    totalParts = j["totalParts"];
    data2 = j["data2"];

}
