#include "Elog.hpp" 
#include <iostream>
#include <cstring>
#include <algorithm>

#ifdef _MSC_VER
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

namespace elogpp
{
  
  #define NAME_LENGTH 500
  #define TEXT_SIZE 100000
  
  constexpr int maxAttachments{50};
  constexpr int maxNAttributes{50};
  /*------------------------------------------------------------------*/
  size_t strlcpy(char *dst, const char *src, size_t size) {
    char *d = dst;
    const char *s = src;
    size_t n = size;
    
    /* Copy as many bytes as will fit */
    if (n != 0 && --n != 0) {
      do {
        if ((*d++ = *s++) == 0)
          break;
      } while (--n != 0);
    }
    
    /* Not enough room in dst, add NUL and traverse rest of src */
    if (n == 0) {
      if (size != 0)
        *d = '\0'; /* NUL-terminate dst */
        while (*s++)
          ;
    }
    
    return (s - src - 1); /* count does not include NUL */
  }
  
  /*-------------------------------------------------------------------*/
  
  /*
   * Appends src to string dst of size siz (unlike strncat, siz is the
   * full size of dst, not space left).  At most siz-1 characters
   * will be copied.  Always NUL terminates (unless size <= strlen(dst)).
   * Returns strlen(src) + MIN(size, strlen(initial dst)).
   * If retval >= size, truncation occurred.
   */
  size_t strlcat(char *dst, const char *src, size_t size) {
    char *d = dst;
    const char *s = src;
    size_t n = size;
    size_t dlen;
    
    /* Find the end of dst and adjust bytes left but don't go past end */
    while (n-- != 0 && *d != '\0')
      d++;
    dlen = d - dst;
    n = size - dlen;
    
    if (n == 0)
      return (dlen + strlen(s));
    while (*s != '\0') {
      if (n != 1) {
        *d++ = *s;
        n--;
      }
      s++;
    }
    *d = '\0';
    
    return (dlen + (s - src)); /* count does not include NUL */
  }
  
  /*---- string comparison -------------------------------------------*/
  std::string toupper(const std::string &s) {
    std::string eval = s;
    std::transform(eval.begin(), eval.end(), eval.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return eval;
  }
  
  int equal_ustring(const std::string &str1, const std::string &str2) {
    if (toupper(str1) == toupper(str2))
      return true;
    else
      return false;
  }
  
  /*-------------------------------------------------------------------*/
  /* convert all special characters to underscores in a string */
  std::string stou(const std::string& str)
  {
    std::string ret=str;
    for(std::size_t i=0;i!=ret.size();++i)
    {
      if(ret[i] == ' ' || ret[i] == '.' || ret[i] == '/' || ret[i] == '\\' ||ret[i] == '-' || ret[i] == '(' || ret[i] == ')') ret[i] = '_';
    }
    return ret;
  }
  
  /*------------------------------------------------------------------*/
  
  std::string url_encode(const std::string& ps)
  /********************************************************************\
   * Encode the given string in-place by adding %XX escapes
   * \********************************************************************/
  {
    char *pd, *p;
    char str[NAME_LENGTH];
    
    pd = (char *)str;
    p = (char *)&ps[0];
    while (*p && pd < str + 250) {
      if (strchr("%&=#?+", *p) || *p > 127) {
        sprintf((char *)pd, "%%%02X", *p);
        pd += 3;
        p++;
      } else if (*p == ' ') {
        *pd++ = '+';
        p++;
      } else {
        *pd++ = *p++;
      }
    }
    *pd = '\0';
    return str;
  }
  
  /*------------------------------------------------------------------*/
  
  void add_crlf(char *buffer, int bufsize) {
    char *p;
    char *tmpbuf;
    
    tmpbuf = (char *)malloc(bufsize);
    
    /* convert \n -> \r\n */
    p = buffer;
    while ((p = strstr(p, "\n")) != nullptr) {
      
      if (p > buffer && *(p - 1) == '\r') {
        p++;
        continue;
      }
      
      if ((int)strlen(buffer) + 2 >= bufsize) {
        free(tmpbuf);
        return;
      }
      
      strlcpy(tmpbuf, p, bufsize);
      *(p++) = '\r';
      strlcpy(p, tmpbuf, bufsize - (p - buffer));
      p++;
    }
    
    free(tmpbuf);
  }
  
  /*------------------------------------------------------------------*/
  std::string convert_crlf(std::string buffer)
  {
    std::string::size_type pos{0};
    while((pos = buffer.find("\n", pos)) != std::string::npos)
    {
      buffer.replace(pos, 1, "\r\n");
    }
    return buffer;
  }

  void tokenize(std::string const &str, const std::string& delim, std::vector<std::string> &out)
  {
    size_t start;
    size_t end = 0;
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {
      end = str.find(delim, start);
      out.push_back(str.substr(start, end - start));
    }
  }
  
  std::string SupressSpaces(std::string part)
  {
    if(part==""|| part==" "||part==" \n"||part=="\n ") return "";
    part=std::string(part.rbegin(),part.rend());
    while(!isgraph(part[0]))
    {
      if(part.size()==1) return "";
      else part=part.erase(0,1);
    }
    part=std::string(part.rbegin(),part.rend());
    while(!isgraph(part[0]))
    {
      part=part.erase(0,1);
    }
    return std::move(part);
  }
  
  void Elog::setTextFile(const std::string& textfile)
  {
    m_Textfile=textfile;
    text_flag=true;
  }
  
  int Elog::SubmitElog()
  {
    
    if(!m_Textfile.empty()) 
    {
      char text2[100000];
      int fh = open(m_Textfile.c_str(), O_RDONLY | O_BINARY);
      if (fh < 0) 
      {
        std::cout<<"Message file \""<<m_Textfile<<"\" does not exist.\n";
        return 1;
      }
      int size = (int)lseek(fh, 0, SEEK_END);
      lseek(fh, 0, SEEK_SET);
      if (size > (int)(sizeof(text2) - 1)) 
      {
        std::cout<<"Message file \""<<m_Textfile<<"\" is too long ("<<sizeof(text2)<<" bytes max).\n";
        return 1;
      }
      int i = read(fh, text2, size);
      if (i < size) 
      {
        std::cout<<"Cannot fully read message from file "<<m_Textfile<<".\n";
        return 1;
      }
      close(fh);
      add_crlf(text2, sizeof(text2));
      m_Text=text2;
    }
    
    
    /*---- open attachment file ----*/
    
    for(std::size_t i = 0; i < m_Attachments.size(); i++) 
    {
    
      int fh = open(m_Attachments[i].c_str(), O_RDONLY | O_BINARY);
      if (fh < 0) 
      {
        std::cout<<"Attachment file \""<<m_Attachments[i].c_str()<<"\" does not exist.\n";
        return 1;
  }
  att_size.push_back(lseek(fh, 0, SEEK_END));
  lseek(fh, 0, SEEK_SET);
  
  buffer.push_back((char *)malloc(att_size[i] + 1));
  
  int n = read(fh, buffer[i], att_size[i]);
  if (n < att_size[i]) 
  {
    std::cout<<"Cannot fully read attachment file \""<<m_Attachments[i].c_str()<<"\".\n";
  return 1;
  }
  buffer[i][n] = '\0';
  
  close(fh);
  }
  
    std::map<std::string,std::string> old_attrib;
    std::string old_text{""};
    char boundary[80], str[80];
    char* p;
    std::string response{""};
    if(m_Type!=New) response=retrieve_elog(old_attrib, old_text);
    
    if(m_Type == Edit || m_Type == Download) 
    {
      // update attributes
      for(std::map<std::string,std::string>::iterator it=m_Attributes.begin();it!=m_Attributes.end();++it)
      {
        if(old_attrib.find(it->first)!=old_attrib.end())
        {
          old_attrib[it->first]=it->second;
        }
      }
      
      // copy attributes 
      m_Attributes=old_attrib;
      if(m_Text.empty()) m_Text=old_text;
    }
    
    if(m_Type == Download) 
    {
      //m_ID=std::stoi(m_Attributes["$@MID@$:"]);
      //if(strstr(response.c_str(), "$@MID@$:")) printf("%s", strstr(response.c_str(), "$@MID@$:"));
      //else printf("%s", response.c_str());
      //std::cout<<response<<std::endl;
      return 1;
    }
    
    else if (m_Type == Reply) 
    {
      // update attributes
      for(std::map<std::string,std::string>::iterator it=m_Attributes.begin();it!=m_Attributes.end();++it)
      {
        if(old_attrib.find(it->first)!=old_attrib.end())
        {
          old_attrib[it->first]=it->second;
        }
      }
      
      // copy attributes 
      for(std::map<std::string,std::string>::iterator it=old_attrib.begin();it!=old_attrib.end();++it)
      {
        if(old_attrib.find("Reply to")==old_attrib.end()&&old_attrib.find("Date")==old_attrib.end())
          m_Attributes.emplace(it->first,it->second);
      }
      
      // check encoding 
      std::string old_encoding = "plain";
      std::map<std::string,std::string>::iterator it=m_Attributes.find(toupper("encoding"));
      if(it!=m_Attributes.end()) old_encoding=it->second;
      
      if(m_quote_on_reply==true) 
      {
        //std::cout<<"quote"<<std::endl;
        //const char* new_text=m_Text.c_str();
        // precede old text with "> "
        char text[100000];
        p = &old_text[0];
        do 
        {
          if (strchr(p, '\n')) 
          {
            *strchr(p, '\n') = 0;
            if (old_encoding[0] == 'H') 
            {
              strlcat(text, "> ", TEXT_SIZE);
              strlcat(text, p, TEXT_SIZE);
              strlcat(text, "<br>\n", TEXT_SIZE);
            } 
            else 
            {
              strlcat(text, "> ", TEXT_SIZE);
              strlcat(text, p, TEXT_SIZE);
              strlcat(text, "\n", TEXT_SIZE);
            }
            p += strlen(p) + 1;
            if (*p == '\n')
              p++;
          } 
          else 
          {
            if (old_encoding[0] == 'H') 
            {
              strlcat(text, "> ", TEXT_SIZE);
              strlcat(text, p, TEXT_SIZE);
              strlcat(text, "<p>\n", TEXT_SIZE);
            } 
            else 
            {
              strlcat(text, "> ", TEXT_SIZE);
              strlcat(text, p, TEXT_SIZE);
              strlcat(text, "\n\n", TEXT_SIZE);
            }
            break;
          }
          
        } while (1);
        //strlcat(text, new_text, TEXT_SIZE);
        m_Text=text+std::string("\r\n")+m_Text;
      }
    }
    
    m_Connector.connect();
    int content_length = 100000;
    for(std::size_t i = 0; i < m_Attachments.size(); i++) content_length += att_size[i];
    char *content = (char *)malloc(content_length);
    if (content == nullptr) 
    {
      std::cout<<"Not enough memory\n";
      return -1;
    }
    
    /* compose content */
    srand((unsigned)time(nullptr));
    sprintf(boundary, "---------------------------%04X%04X%04X", rand(), rand(),rand());
    strcpy(content, boundary);
    strcat(content,"\r\nContent-Disposition: form-data; name=\"cmd\"\r\n\r\nSubmit\r\n");
    
    if(!m_Uname.empty())
    {
      sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"unm\"\r\n\r\n%s\r\n",boundary, m_Uname.c_str());
      sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"upwd\"\r\n\r\n%s\r\n",boundary, m_Upwd.c_str());
    }
    
    if(!m_Logbook.empty()) sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"exp\"\r\n\r\n%s\r\n",boundary,m_Logbook.c_str());
    
    if(m_Type == Reply) sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"reply_to\"\r\n\r\n%d\r\n",boundary, m_ID);
    
    if (m_Type == Edit) 
    {
      sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"edit_id\"\r\n\r\n%d\r\n",boundary, m_ID);
      sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"skiplock\"\r\n\r\n1\r\n",boundary);
    }
    
    if(m_Suppress==true) sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"suppress\"\r\n\r\n1\r\n",boundary);
    
    if (m_Encoding == 0) sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"encoding\"\r\n\r\nELCode\r\n",boundary);
    else if (m_Encoding == 1) sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"encoding\"\r\n\r\nplain\r\n",boundary);
    else if (m_Encoding == 2) sprintf(content + strlen(content), "%s\r\nContent-Disposition: form-data; name=\"encoding\"\r\n\r\nHTML\r\n",boundary);
    
    for(std::map<std::string,std::string>::iterator it=m_Attributes.begin();it!=m_Attributes.end();++it) 
    {
      sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n",boundary,it->first.c_str(), it->second.c_str());
    }
    
    if(!m_Text.empty()) sprintf(content + strlen(content), "%s\r\nContent-Disposition: form-data; name=\"Text\"\r\n\r\n%s\r\n%s\r\n",boundary, m_Text.c_str(), boundary);
    
    content_length = strlen(content);
    p = content + content_length;
    
    for(std::size_t i = 0; i < m_Attachments.size(); i++)
    {
      sprintf(p,"Content-Disposition: form-data; name=\"attfile%d\"; filename=\"%s\"\r\n\r\n",i + 1, m_Attachments[i].c_str());
      content_length += strlen(p);
      p += strlen(p);
      memcpy(p, buffer[i], att_size[i]);
      p += att_size[i];
      strcpy(p, boundary);
      strcat(p, "\r\n");
      content_length += att_size[i] + strlen(p);
      p += strlen(p);
    }
    
    /* compose request */
    char request[100000];
    strcpy(request, "POST /");
    if(!m_Subdir.empty()) sprintf(request + strlen(request), "%s/", m_Subdir.c_str());
    if(!m_Logbook.empty()) 
    {
      sprintf(request + strlen(request), "%s/",url_encode(m_Logbook).c_str());
    }
    strcat(request, " HTTP/1.0\r\n");
    sprintf(request + strlen(request), "Content-Type: multipart/form-data; boundary=%s\r\n", boundary);
    if (getPort()!= 80) sprintf(str, "%s:%d", getHostname().c_str(),getPort());
    else sprintf(str, "%s", getHostname().c_str());
    sprintf(request + strlen(request), "Host: %s\r\n", str);
    sprintf(request + strlen(request), "User-Agent: ELOG\r\n");
    sprintf(request + strlen(request), "Content-Length: %d\r\n", content_length);
    
    strcat(request, "\r\n");
    m_Connector.send(request);
    m_Connector.send(content,content_length);
    /* receive response */
    std::string resp=m_Connector.receive();
    m_Connector.disconnect();
    /* check response status */
    if(resp.find("302 Found")!=std::string::npos) 
    {
      if (resp.find("Location:")!=std::string::npos) 
      {
        if (resp.find("has moved")!=std::string::npos) printf("Error: elogd server has moved to another location\n");
        else if (resp.find("fail")!=std::string::npos) printf("Error: Invalid user name or password\n");
        else 
        {
          strncpy(str, strstr(resp.c_str(), "Location:") + 10, sizeof(str));
          if (strchr(str, '?')) *strchr(str, '?') = 0;
          if (strchr(str, '\n')) *strchr(str, '\n') = 0;
          if (strchr(str, '\r')) *strchr(str, '\r') = 0;
          
          if (strrchr(str, '/')) printf("Message successfully transmitted, ID=%s\n", strrchr(str, '/') + 1);
          else printf("Message successfully transmitted, ID=%s\n", str);
        }
      } 
      else printf("Message successfully transmitted\n");
    } 
    else if (resp.find("td class=\"errormsg\">")!=std::string::npos) std::cout<<"Error: Wrong logbook name\n"<<std::endl;
    else if(resp.find("upassword")!=std::string::npos) std::cout<<"Error: Missing or invalid user name/password\n"<<std::endl;
    else if (resp.find("Error: Attribute")!=std::string::npos) 
    {
      if (resp.find("not existing")!=std::string::npos) 
      { 
        strncpy(str, strstr(resp.c_str(), "Error: Attribute") + 27, sizeof(str));
        if (strchr(str, '<')) *strchr(str, '<') = 0;
        printf("Error: Non existing attribute option \"%s\"\n", str);
      } 
      else 
      {
        strncpy(str, strstr(resp.c_str(), "Error: Attribute") + 20, sizeof(str));
        if (strchr(str, '<')) *strchr(str, '<') = 0;
        printf("Error: Missing required attribute \"%s\"\n", str);
      }
    } 
    else std::cout<<"Error transmitting message\n";
    return 1;
    
  }
  
  std::string Elog::retrieve_elog(std::map<std::string,std::string>& attrib,std::string& text)
  {
    m_Connector.connect();
    std::string request{"GET /"};
    request.reserve(100000);
    std::string encodedlogbook=url_encode(m_Logbook);
    
    if(!m_Subdir.empty() && !m_Logbook.empty())
    {
      request+=m_Subdir+"/"+encodedlogbook+"/"+std::to_string(m_ID)+"?cmd=download";
    }
    else if (!m_Subdir.empty())
    {
      request+=m_Subdir+"/"+std::to_string(m_ID)+"?cmd=download";
    }
    else if(!m_Logbook.empty())
    {
      request+=encodedlogbook+"/"+std::to_string(m_ID)+"?cmd=download";
    }
    request+=" HTTP/1.0\r\nUser-Agent: ELOG\r\n";
    
    if(!m_Uname.empty()) 
    {
      request+="Cookie: unm="+m_Uname+";upwd="+m_Upwd+";\r\n";
    }
    request+="\r\n";
    
    /* send request */
    m_Connector.send(request);
    /* receive response */
    std::string resp=m_Connector.receive();
    m_Connector.disconnect();
    /* check response status */
    std::size_t start=resp.find("$@MID@$:");
    if(start!=std::string::npos) 
    {
      /* separate attributes and message */
      const std::string separator{"========================================\n"};
      std::size_t split=resp.find(separator);
      text=resp.substr(split+separator.size());
      /* skip first line */
      std::vector<std::string> tokens;
      tokenize(resp.substr(start,split),"\n", tokens);
      for(unsigned int i=0;i!=tokens.size();++i)
      {
        std::size_t found=tokens[i].find(':');
        if(found!=std::string::npos) 
        {
          try
          {
            if(tokens[i].substr(0,found)=="$@MID@$") m_ID=std::stoi(SupressSpaces(tokens[i].substr(found+1)));
            attrib.emplace(SupressSpaces(tokens[i].substr(0,found)),SupressSpaces(tokens[i].substr(found+1)));
          }
          catch(const std::invalid_argument& error)
          {
             m_ID=0;
          }
        } 
      }    
      return resp;
    }
    else if(resp.find("302 Found")!=std::string::npos) 
    {
      if(strstr(resp.c_str(), "Location:")) 
      {
        if (strstr(resp.c_str(), "fail")) printf("Error: Invalid user name or password\n");
        else 
        {
          char str[256];
          strncpy(str, strstr(resp.c_str(), "Location:") + 10, sizeof(str));
          if (strchr(str, '?')) *strchr(str, '?') = 0;
          if (strchr(str, '\n')) *strchr(str, '\n') = 0;
          if (strchr(str, '\r')) *strchr(str, '\r') = 0;
          if (strrchr(str, '/')) printf("Message successfully transmitted, ID=%s\n", strrchr(str, '/') + 1);
          else printf("Message successfully transmitted, ID=%s\n", str);
        }
      }
    } 
    else if (strstr(resp.c_str(), "Logbook Selection")) printf("Error: No logbook specified\n");
    else if (strstr(resp.c_str(), "enter password")) printf("Error: Missing or invalid password\n");
    else if (strstr(resp.c_str(), "form name=form1")) printf("Error: Missing or invalid user name/password\n");
    else printf("Error transmitting message\n");
    
    throw 0;
  }
}
