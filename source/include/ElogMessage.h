#ifndef ElogMessage_h
#define ElogMessage_h
#include <vector>
#include <string>
#include <map>

namespace elogpp 
{
class ElogMessage
{
public:
    void SetAttribute(std::string,std::string);
    void SetAttribute(std::pair<std::string,std::string>);
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
    //void RemoveAttachment(int i);
    //void RemoveAttachment(std::string);
private:
    std::vector<std::string> Attachments;
    std::map<std::string,std::string> Attributes;
    std::string TextMessage{""};
    std::string FileMessage{""};
};
}
#endif
