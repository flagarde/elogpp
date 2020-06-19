#pragma once
#include "ElogMessage.hpp"
#include "ElogCommand.hpp"
#include "ElogConfig.hpp"

namespace elogpp
{
class ElogEntry
{
public:
    void SetAttribute(std::string,std::string);
    std::string GetAttribute(std::string);
    bool IsAttribute(std::string);
    bool IsAttributeEmpty(std::string);
    void AddAttachment(std::string);
    void SetMessage(std::string);
    void AddtoMessage(std::string);
    void AddFileMessage(std::string);
    void Print();
    bool HasAttachment();
    bool HasAttributes();
    bool HasFileMessage();
    bool HasTextMessage();
    std::vector<std::string>& GetAttachments();
    std::map<std::string,std::string>& GetAttributes();
    std::string GetFileMessage();
    std::string GetTextMessage();
    bool Send(std::string="");
    ElogEntry& User(std::string);
    ElogEntry& ReplyTo(std::string,std::string="");
    ElogEntry& ReplyTo(int,std::string="");
    ElogEntry& Edit(int);
    ElogEntry& Edit(std::string);
    ElogEntry& To(std::string,std::string);
    ElogEntry& ReceiveEntry(std::string);
    ElogEntry& ReceiveEntry(int);
private :
    std::string GetLastID();
    bool Sended(std::string);
    std::string SupressSpaces(std::string);
    void ParseHeader(std::string);
    void ParseReceivedCommand(std::string);
    std::pair<std::string,std::string> ParseAttribute(std::string);
    std::string ExecuteCommand();
    ElogMessage message;
    ElogCommand Command;
    ElogConfig conf;
};
}
