#ifndef ElogCommand_h
#define ElogCommand_h
#include <string>
#include "ElogServer.h"
#include "ElogUser.h"
#include "ElogMessage.h"

namespace elogpp
{
class ElogCommand 
{
public:
    enum class Type{Reply,Download,Option}; 
    void BuildCommand();
    std::string Execute();
    void AddToCommand(ElogServer,std::string);
    void AddToCommand(ElogUser);
    void AddToCommand(ElogMessage&);
    void AddToCommand(std::string,Type);
    void AddToCommand(std::string,std::string);
    std::string GetCommand();
    Type SetAsReply();
    Type SetAsDownload();
    Type SetAsOption();
    void ResetReplyEditDownload();
private: 
    static inline const std::string Elog = "elog";
    //Server Stufs
    static inline const std::string HostParam = " -h ";
    static inline const std::string PortParam = " -p ";
    static inline const std::string SubDirParam = " -d ";
    static inline const std::string LogbookParam = " -l ";
    static inline const std::string SSLParam = " -s";
    std::string ServerPart;
    //User stufs
    static inline const std::string UsernamePasswordParam = " -u ";
    std::string UserPart;
    //Message Stufs
    static inline const std::string AttachmentParam = " -f ";
    static inline const std::string AttributeParam = " -a ";
    static inline const std::string TextFileParam= " -m ";
    std::string MessagePart;
    //////////////
    //Miscellanous
    static inline const std::string ReplyParam = " -r ";
    static inline const std::string ReplaceMessageParam = " -e ";
    static inline const std::string QuoteOriginalTextParam = " -q";
    //Download
    static inline const std::string DownloadMessageParam = " -w ";    
    std::string ReplyEditDownload;
    ///////////////
    //Options
    static inline const std::string VerboseParam = " -v";
    static inline const std::string SupressEmailNotificationParam= " -x ";
    static inline const std::string EncodingParam= " -n ";
    std::string OptionsPart{""};
    enum class Encoding{ELcode,plain,HTML};
    
    std::string Command{""};
};
}
#endif