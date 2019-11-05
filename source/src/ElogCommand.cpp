#include "ElogCommand.h"
#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include <streambuf>
#include "pstream.h"

namespace elogpp
{
    
void ElogCommand::ResetReplyEditDownload()
{
    ReplyEditDownload="";
}

std::string ElogCommand::Execute()
{
    redi::ipstream in(Command.c_str());
    std::string str((std::istreambuf_iterator<char>(in)),std::istreambuf_iterator<char>());
    ResetReplyEditDownload();
    return std::move(str);
}

ElogCommand::Type ElogCommand::SetAsReply()
{
 return Type::Reply;   
}

ElogCommand::Type ElogCommand::SetAsDownload()
{
    return Type::Download;
}

ElogCommand::Type ElogCommand::SetAsOption()
{
    return Type::Option;
}

void ElogCommand::AddToCommand(std::string id,ElogCommand::Type type)
{
    if(type==Type::Reply)ReplyEditDownload=ReplaceMessageParam+id;
    else if(type==Type::Download) ReplyEditDownload=DownloadMessageParam+id;
    else if(type==Type::Option)
    {
        if(id.find("V+")!=std::string::npos) OptionsPart=VerboseParam;
        if(id.find("X")!=std::string::npos||id.find("x")!=std::string::npos) OptionsPart+=SupressEmailNotificationParam;
        if(id.find("plain")!=std::string::npos||id.find("PLAIN")!=std::string::npos||id.find("1")!=std::string::npos)
        {
            OptionsPart+=EncodingParam+"1";
        }
        else if (id.find("ELcode")!=std::string::npos||id.find("elcode")!=std::string::npos||id.find("ELCODE")!=std::string::npos||id.find("0")!=std::string::npos)
        {
            OptionsPart+=EncodingParam+"0";
        }
        else if (id.find("HTML")!=std::string::npos||id.find("html")!=std::string::npos||id.find("2")!=std::string::npos)
        {
            OptionsPart+=EncodingParam+"2";
        }
    }
}

void ElogCommand::AddToCommand(std::string id,std::string option)
{
    if(option!="q")ReplyEditDownload=ReplyParam+id;
    else if(option=="q")ReplyEditDownload=QuoteOriginalTextParam+ReplyParam+id;
}

void ElogCommand::AddToCommand(ElogMessage& mess)
{
    MessagePart="";
    if(mess.HasAttachment()==true)
    {
        for(unsigned int i=0;i!=mess.GetAttachments().size();++i)MessagePart+=AttachmentParam+"\""+mess.GetAttachments()[i]+"\"";
    }
    if(mess.HasAttributes()==true)
    {
        for(std::map<std::string,std::string>::iterator it=mess.GetAttributes().begin();it!=mess.GetAttributes().end();++it)
        {
            MessagePart+=AttributeParam+"\""+it->first+"\"=\""+it->second+"\"";
        }
    }
    if(mess.HasFileMessage()==true && mess.HasTextMessage()==true) 
    {
        std::ifstream file(mess.GetFileMessage().c_str());
        file.seekg(0, std::ios::end);   
        std::string stringfile;
        stringfile.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        stringfile.assign((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
        MessagePart+=" \""+stringfile+"\n"+mess.GetTextMessage()+"\"";
    }
    else if(mess.HasFileMessage()==true && mess.HasTextMessage()==false)
    {
        MessagePart+=TextFileParam+"\""+mess.GetFileMessage()+"\"";
    }
    else if(mess.HasFileMessage()==false && mess.HasTextMessage()==true)
    {
        MessagePart+=" \""+mess.GetTextMessage()+"\"";
    }
    else MessagePart+=" \" \"";
}

void ElogCommand::BuildCommand()
{
    Command=Elog+ServerPart+UserPart+ReplyEditDownload+OptionsPart+MessagePart;
}

void ElogCommand::AddToCommand(ElogUser& user)
{
    UserPart=UsernamePasswordParam+user.GetName()+" "+user.GetPassword();
}

void ElogCommand::AddToCommand(ElogServer& server,std::string elog)
{
    ServerPart=HostParam+server.GetHostname();
    if(server.GetPort()!="")ServerPart+=PortParam+server.GetPort();
    if(server.GetSubDir()!="")ServerPart+=SubDirParam+server.GetSubDir();
    if(server.GetSSL()==true)ServerPart+=SSLParam;
    ServerPart+=LogbookParam+elog;
}

std::string ElogCommand::GetCommand()
{
    return Command;
}
}
