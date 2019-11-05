#include "ElogMessage.h"
#include <string>
#include <iostream>

namespace elogpp
{
void ElogMessage::Print()
{
    std::cout<<"Message :\n"
             <<"\t-> Attribute(s) :\n";
    for(std::map<std::string,std::string>::iterator it=Attributes.begin();it!=Attributes.end();++it)
    {
        std::cout<<"\t\t"<<"["<<it->first<<"] : "<<it->second<<"\n";
    }
    std::cout<<"\t-> Attachement(s) :\n";
    for(unsigned int i=0;i!=Attachments.size();++i) std::cout<<"\t\t"<<Attachments[i]<<"\n";
    std::cout<<"\t-> Message :\n";
    std::cout<<"\t\t\""<<TextMessage<<"\"\n";
    std::cout<<"\t-> File Message :\n";
    std::cout<<"\t\t\""<<FileMessage<<"\"\n";
}


void ElogMessage::SetAttribute(std::pair<std::string,std::string> attribute)
{
    SetAttribute(attribute.first,attribute.second);
}

void ElogMessage::SetAttribute(std::string attribute,std::string value)
{
    Attributes.insert(std::pair<std::string,std::string>(attribute,value));
}

std::string ElogMessage::GetAttribute(std::string attribute)
{
    if(IsAttribute(attribute)==true) return Attributes[attribute];
    else
    {
        std::cout<<"Unknow attribute !\n";
        return "";
    }
}

bool ElogMessage::IsAttribute(std::string attribute)
{
    if(Attributes.find(attribute)!=Attributes.end()) return true;
    else return false;
}

bool ElogMessage::IsAttributeEmpty(std::string attribute)
{
    if(IsAttribute(attribute)==true)
    {
        if(Attributes[attribute]=="") return true;
        else return false;
    }
    else
    {
        std::cout<<"Unknow attribute !\n";
        return true;
    }
}

void ElogMessage::AddAttachment(std::string attachment)
{
    Attachments.push_back(attachment);
}

void ElogMessage::SetMessage(std::string message)
{
    TextMessage=message;
}

void ElogMessage::AddtoMessage(std::string message)
{
    TextMessage+=message;
}

bool ElogMessage::HasFileMessage()
{
    if(FileMessage!="")return true;
    else return false;
}

std::string ElogMessage::GetFileMessage()
{
    return FileMessage;
}

std::string ElogMessage::GetTextMessage()
{
    return TextMessage;
}

bool ElogMessage::HasTextMessage()
{
    if(TextMessage!="")return true;
    else return false;
}


void ElogMessage::AddFileMessage(std::string filemessage)
{
    FileMessage=filemessage;
}

bool ElogMessage::HasAttachment()
{
    if(Attachments.empty()) return false;
    else return true;
}

bool ElogMessage::HasAttributes()
{
    if(Attributes.empty()) return false;
    else return true;
}

std::vector<std::string> & ElogMessage::GetAttachments()
{
    return Attachments;
}

std::map<std::string, std::string> & ElogMessage::GetAttributes()
{
    return Attributes;
}
}
