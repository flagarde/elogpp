#include "ElogEntry.h"
#include <iostream>
#include <vector>
namespace elogpp
{

bool ElogEntry::Sended(std::string message)
{
    if(message.find("Message successfully transmitted")!=std::string::npos) return true;
    else return false;
}

std::string ElogEntry::SupressSpaces(std::string part)
{
    std::string space=" ";
    std::size_t foundbegin = part.find_first_not_of(space);
    if(foundbegin!=std::string::npos)part=part.erase(0,foundbegin);
    part=std::string(part.rbegin(),part.rend());
    std::size_t foundend=part.find_first_not_of(space);
    if(foundend!=std::string::npos) part=part.erase(0,foundend);
    part=std::string(part.rbegin(),part.rend());
    return std::move(part);
}
    
std::pair<std::string,std::string> ElogEntry::ParseAttribute(std::string line)
{
    std::string delimiter=":";
    std::size_t found = line.find(delimiter);
    std::pair<std::string,std::string> attribute(SupressSpaces(line.substr(0,found)),SupressSpaces(line.substr(found+delimiter.size())));
    return std::move(attribute);
}
void ElogEntry::ParseHeader(std::string header)
{
    std::string newline="\n";
    std::size_t found = 0;
    do
    {
        found = header.find(newline);
        if(header.substr(0,found)!="")
        {
            std::pair<std::string,std::string> attribute=ParseAttribute(header.substr(0,found));
            if(header.substr(0,found).find("$@MID@$")!=std::string::npos) attribute.first="ID";
            message.SetAttribute(std::move(attribute));
        }
        header=header.erase(0,found+1);
    }
    while(found!=std::string::npos);
}

void ElogEntry::ParseReceivedCommand(std::string entry)
{
    std::string Delimiter="========================================";
    std::size_t found=entry.find(Delimiter);
    std::size_t BeginingMessage=found+Delimiter.size();
    message.SetMessage(SupressSpaces(entry.substr(BeginingMessage)));
    entry.erase(found);
    ParseHeader(entry);
}
        
ElogEntry& ElogEntry::ReceiveEntry(int id)
{
    return ReceiveEntry(std::to_string(id));
}
    
ElogEntry& ElogEntry::ReceiveEntry(std::string id)
{
    Command.AddToCommand(id,Command.SetAsDownload());
    Command.BuildCommand();
    ParseReceivedCommand(ExecuteCommand());
    return *this;
}
    
ElogEntry & ElogEntry::Edit(int id)
{
    return Edit(std::to_string(id));
}

ElogEntry & ElogEntry::Edit(std::string id)
{
    Command.AddToCommand(id,Command.SetAsReply());
    return *this;
}

ElogEntry & ElogEntry::ReplyTo(int id,std::string option)
{
    return ReplyTo(std::to_string(id),option);
}

ElogEntry & ElogEntry::ReplyTo(std::string id,std::string option)
{
    Command.AddToCommand(id,option);
    return *this;
}

ElogEntry& ElogEntry::User(std::string user)
{
    if(conf.HasUser(user))Command.AddToCommand(conf.GetUser(user));
    else std::cout<<"User "<<user<<" unknown ! Please check your configuration files !\n";
    return *this;
}

bool ElogEntry::Send(std::string option)
{
    Command.AddToCommand(option,Command.SetAsOption());
    Command.AddToCommand(message);
    Command.BuildCommand();
    std::string MessageFromElog=ExecuteCommand();
    if(option.find("V-")!=std::string::npos) ;
    else if(option.find("V")!=std::string::npos||Sended(MessageFromElog)==false)std::cout<<MessageFromElog<<std::endl;
    return Sended(MessageFromElog);
}

std::string ElogEntry::ExecuteCommand()
{
    return std::move(Command.Execute());
}

ElogEntry& ElogEntry::To(std::string server,std::string logbook)
{
    if(conf.HasServer(server))Command.AddToCommand(conf.GetServer(server),logbook);
    else std::cout<<"Server "<<server<<" unknown ! Please check your configuration files !\n";
    return *this;
}

void ElogEntry::SetAttribute(std::string attribute, std::string value)
{
    message.SetAttribute(attribute,value);
}

std::string ElogEntry::GetAttribute(std::string attribute)
{
    return message.GetAttribute(attribute);
}

bool ElogEntry::IsAttribute(std::string attribute) 
{
    return message.IsAttribute(attribute);
}

bool ElogEntry::IsAttributeEmpty(std::string attribute)
{
    return message.IsAttributeEmpty(attribute);
}

void ElogEntry::AddAttachment(std::string attachment)
{
    message.AddAttachment(attachment);
}

void ElogEntry::SetMessage(std::string message_)
{
    message.SetMessage(message_);
}

void ElogEntry::AddtoMessage(std::string message_)
{
    message.AddtoMessage(message_);
}

void ElogEntry::AddFileMessage(std::string filemessage)
{
    message.AddFileMessage(filemessage);
}

void ElogEntry::Print()
{
    message.Print();
}

bool ElogEntry::HasAttachment()
{
    return message.HasAttachment();
}

bool ElogEntry::HasAttributes()
{
    return message.HasAttributes();
}

bool ElogEntry::HasFileMessage()
{
    return message.HasFileMessage();
}


bool ElogEntry::HasTextMessage()
{
    return message.HasTextMessage();
}

std::vector<std::string> & ElogEntry::GetAttachments()
{
    return message.GetAttachments();
}

std::map<std::string, std::string> & ElogEntry::GetAttributes()
{
    return message.GetAttributes();
}

std::string ElogEntry::GetFileMessage()
{
    return message.GetFileMessage();
}

std::string ElogEntry::GetTextMessage()
{
    return message.GetTextMessage();
}
}
