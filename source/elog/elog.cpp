/********************************************************************\
 * 
 *  Name:         elog.c
 *  Created by:   Stefan Ritt
 *  Modified by:  Francois Lagarde
 *  Copyright 2000 + Stefan Ritt
 * 
 *  ELOG is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  ELOG is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with ELOG.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * 
 *  Contents:     Electronic logbook utility
 * 
 * \********************************************************************/

#include <string>
#include <iostream>
#include <algorithm>

/* ELOG identification */
constexpr const char* elogid{"elogd 3.1.4"};
constexpr int maxAttachments{50};
constexpr int maxNAttributes{50};

int usage()
{
  std::cout<<elogid<<"\n";
  std::cout<<"usage: elog\n";
  std::cout<<"elog -h <hostname> [-p port] [-d subdir]\n";
  std::cout<<"                              Location where elogd is running\n";
  std::cout<<"     -l logbook/experiment    Name of logbook or experiment\n";
  std::cout<<"     -s                       Use SSL for communication\n";
  std::cout<<"     [-v]                     For verbose output\n";
  std::cout<<"     [-u username password]   User name and password\n";
  std::cout<<"     [-f <attachment>]        (up to "<<maxAttachments<<" attachments)\n";
  std::cout<<"     -a <attribute>=<value>   (up to "<<maxNAttributes<<" attributes)\n";
  std::cout<<"     [-r <id>]                Reply to existing message\n";
  std::cout<<"     [-q]                     Quote original text on reply\n";
  std::cout<<"     [-e <id>]                Edit existing message\n";
  std::cout<<"     [-w <id>|last]           Download existing message (last message)\n";
  std::cout<<"     [-x]                     Suppress email notification\n";
  std::cout<<"     [-n 0|1|2]               Encoding: 0:ELcode,1:plain,2:HTML\n";
  std::cout<<"     -m <textfile>] | <text>\n";
  std::cout<<"\nArguments with blanks must be enclosed in quotes\n";
  std::cout<<"The elog message can either be submitted on the command line, piped in like\n";
  std::cout<<"\"cat text | elog -h ... -l ... -a ...\" or in a file with the -m flag.\n";
  std::cout<<"Multiple attributes and attachments can be supplied\n";
  return 1;
}

void checkResponse(const std::string& response)
{
  if(response.find("302 Found")!=std::string::npos) 
  {
    if(response.find("Location:")!=std::string::npos) 
    {
      if(response.find("has moved")!=std::string::npos) std::cout<<"Error: elogd server has moved to another location\n";
      if(response.find("fail")!=std::string::npos) std::cout<<"Error: Invalid user name or password\n";
      else 
      {
        std::size_t loc=response.find("Location:")+10;
        std::string str=response.substr(loc);
        loc=str.find('?');
        if(loc!=std::string::npos) str=str.substr(0,loc);
        loc=str.find('\n');
        if(loc!=std::string::npos) str=str.substr(0,loc);
        loc=str.find('\r');
        if(loc!=std::string::npos) str=str.substr(0,loc);
        loc=str.find_last_of('/')+1;
        if(loc!=std::string::npos)
        {
          str=str.substr(loc);
          std::cout<<"Message successfully transmitted, ID="<<str<<std::endl;
        }
        else std::cout<<"Message successfully transmitted, ID="<<str<<"\n";
      }
    }
    else std::cout<<"Message successfully transmitted\n"<<std::endl;
  } 
  else if(response.find("Error: logbook ")!=std::string::npos) std::cout<<"Error: Logbook doesn't exists\n";
  else if(response.find("Forgot password?")!=std::string::npos) std::cout<<"Error: Missing or invalid name/password\n";
  else if(response.find("Error: Attribute")!=std::string::npos)
  {
    if(response.find("not existing")!=std::string::npos)
    {
      std::size_t loc=response.find("Error: Attribute")+27;
      std::string str=response.substr(loc);
      if(loc=response.find('<');loc!=std::string::npos) response.substr(0,loc);
      std::cout<<"Error: Non existing attribute option \""<<str<<"\"\n";
    }
    else
    {
      std::size_t loc=response.find("Error: Attribute")+20;
      std::string str=response.substr(loc);
      if(loc=response.find('<');loc!=std::string::npos) response.substr(0,loc);
      std::cout<<"Error: Missing required attribute \""<<str<<"\"\n";
    } 
  }
  else std::cout<<"Error transmitting message\n";
}

enum Type
{
 Reply,
 Download,
 Edit,
 New 
};

/* SSL includes */
#ifdef HAVE_SSL
#include <openssl/ssl.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#ifdef _MSC_VER
#include <windows.h>
#include <io.h>
#else
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#define closesocket(s) ::close(s)
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif

class Elog
{
public:
  Elog()
  {
    #if defined( _MSC_VER )
    {
      WSADATA WSAData;
      /* Start windows sockets */
      if (WSAStartup(MAKEWORD(1, 1), &WSAData) != 0) return -1;
    }
    #endif
  }
  void setVerbose(const bool& verbose)
  {
    m_Verbose=verbose;
  }
  void setHostname(const std::string& hostname)
  {
    m_Hostname=hostname;
  }
  std::string getHostname()
  {
    return m_Hostname;
  }
  void setPort(const unsigned int& port)
  {
    m_Port=port;
  }
  unsigned int getPort()
  {
    return m_Port;
  }
  void setSSL(const bool& ssl)
  {
    m_SSL=ssl;
    #ifndef HAVE_SSL
    if(ssl)
    {
      std::cout<<"SLL support not compiled into this program\n";
      throw 1;
    }
    #endif
  }
  int connect()
  {
    if(m_Hostname.empty())
    {
      std::cout<<"Please specify hostname.\n";
      return -1;
    }
    /* create socket */
    if((m_Sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
      std::cout<<"cannot create socket\n";
      return -1;
    }
    /* compose remote address */
    struct sockaddr_in bind_addr;
    memset(&bind_addr, 0, sizeof(bind_addr));
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = 0;
    bind_addr.sin_port = htons((unsigned short) m_Port);
    
    struct hostent* phe = gethostbyname(m_Hostname.c_str());
    if(phe == nullptr) 
    {
      std::cout<<"cannot get host name\n";
      return -1;
    }
    memcpy((char *) &(bind_addr.sin_addr), phe->h_addr, phe->h_length);
    /* connect to server */
    int status = ::connect(m_Sock, (const sockaddr*) &bind_addr, sizeof(bind_addr));
    if (status != 0) 
    {
      std::cout<<"Cannot connect to host "<<m_Hostname<<", port "<<m_Port<<"\n";
      return -1;
    }
    if(m_Verbose) std::cout<<"Successfully connected to host "<<m_Hostname<<", port "<<m_Port<<"\n";
    #ifdef HAVE_SSL
    if(m_SSL && ssl_connect() < 0) 
    {
      std::cout<<"elogd server does not run SSL protocol\n";
      return -1;
    }
    #endif
    return 0;
  }
  int ssl_connect()
  {
    #ifdef HAVE_SSL
    SSL_library_init();
    SSL_load_error_strings();
    #if OPENSSL_VERSION_NUMBER > 0x1010000fL
    SSL_METHOD* meth{(SSL_METHOD *) TLS_method()};
    #else
    SSL_METHOD* meth{(SSL_METHOD *) TLSv1_2_method()};
    #endif
    SSL_CTX* ctx{SSL_CTX_new(meth)};
    m_SSL_con={SSL_new(ctx)};
    SSL_set_fd(m_SSL_con, m_Sock);
    if(SSL_connect(m_SSL_con) <= 0) return -1;
    X509* cert{SSL_get_peer_certificate(m_SSL_con)};
    if(cert == nullptr) return -1;
    long int i{SSL_get_verify_result(m_SSL_con)};
    if(i != X509_V_OK) std::cout<<"Possibly invalid certificate, continue on your own risk!\n";
    #endif
    return 0;
  }
  void sendRequest(const std::string& request,std::size_t length=0)
  {
    if(length==0) length=strlen(request.c_str());
    #ifdef HAVE_SSL
    if(m_SSL)
      SSL_write(m_SSL_con,request.c_str(),length);
    else
      #endif
      send(m_Sock,request.c_str(),length,0);
    if(m_Verbose) 
    {
      std::cout<<"Request sent to host:\n";
      puts(request.c_str());
    }
  }
  std::string receiveRespond()
  {
    char response[100000];
    memset(response, 0, sizeof(response));
    #ifdef HAVE_SSL
    int i{0};
    if(m_SSL)
      i=SSL_read(m_SSL_con,response, sizeof(response) - 1);
    else
      #endif
      i=recv(m_Sock, response, sizeof(response) - 1, 0);
    if(i < 0)
    {
      std::cout<<"Cannot receive response"<<std::endl;
      throw -1;
    }
    /* discard remainder of response */
    int n{i};
    while (i > 0) 
    {
      #ifdef HAVE_SSL
      if(m_SSL)
        i = SSL_read(m_SSL_con, response + n, sizeof(response) - 1 - n);
      else
        #endif
        i = recv(m_Sock, response + n, sizeof(response) - 1 - n, 0);
      if(i > 0)n += i;
    }
    response[n] = 0;
    if(m_Verbose)
    {
      std::cout<<"Response received:\n";
      puts(response);
    }
    return std::string(response);
  }
  void close()
  {
    #ifdef HAVE_SSL
    if(m_SSL) 
    {
      SSL_shutdown(m_SSL_con);
      SSL_free(m_SSL_con);
    }
    #endif
    closesocket(m_Sock);
  }
  ~Elog()
  {
    #ifdef HAVE_SSL
    if(m_SSL) 
    {
      SSL_shutdown(m_SSL_con);
      SSL_free(m_SSL_con);
    }
    #endif
    closesocket(m_Sock);
  }
private:
  std::string m_Hostname{""};
  unsigned int m_Port{80};
  bool m_Verbose{false};
  int m_Sock{0};
  #ifdef HAVE_SSL
  SSL* m_SSL_con{nullptr};
  #endif
  bool m_SSL{false};
};





#define NAME_LENGTH     500


#define TEXT_SIZE    100000

int verbose;

char text[TEXT_SIZE], old_text[TEXT_SIZE], new_text[TEXT_SIZE];


/*------------------------------------------------------------------*/

size_t strlcpy(char *dst, const char *src, size_t size)
{
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
      *d = '\0';             /* NUL-terminate dst */
      while (*s++);
  }
  
  return (s - src - 1);        /* count does not include NUL */
}

/*-------------------------------------------------------------------*/

/*
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left).  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless size <= strlen(dst)).
 * Returns strlen(src) + MIN(size, strlen(initial dst)).
 * If retval >= size, truncation occurred.
 */
size_t strlcat(char *dst, const char *src, size_t size)
{
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
  
  return (dlen + (s - src));   /* count does not include NUL */
}



/*---- string comparison -------------------------------------------*/
std::string toupper(const std::string& s) 
{
  std::string eval=s;
  std::transform(eval.begin(), eval.end(), eval.begin(), [](unsigned char c){ return std::toupper(c); });
  return eval;
}

int equal_ustring(const std::string& str1,const std::string& str2)
{
  if(toupper(str1)==toupper(str2)) return true;
  else return false;
}

char *sha256_crypt(const char *key, const char *salt);

void do_crypt(char *s, char *d, int size)
{
  strlcpy(d, sha256_crypt(s, "$5$") + 4, size);
}

/*-------------------------------------------------------------------*/

void stou(char *str)
/* convert all special characters to underscores in a string */
{
  int i;
  
  for (i = 0; i < (int) strlen(str); i++)
    if (str[i] == ' ' || str[i] == '.' || str[i] == '/' ||
      str[i] == '\\' || str[i] == '-' || str[i] == '(' || str[i] == ')')
      str[i] = '_';
}

/*------------------------------------------------------------------*/

void url_encode(char *ps, int size)
/********************************************************************\
 * Encode the given string in-place by adding %XX escapes
 * \********************************************************************/
{
  unsigned char *pd, *p;
  unsigned char str[NAME_LENGTH];
  
  pd = (unsigned char *) str;
  p = (unsigned char *) ps;
  while (*p && pd < str + 250) {
    if (strchr("%&=#?+", *p) || *p > 127) {
      sprintf((char *) pd, "%%%02X", *p);
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
  strlcpy(ps, (char *) str, size);
}

/*------------------------------------------------------------------*/

void add_crlf(char *buffer, int bufsize)
{
  char *p;
  char *tmpbuf;
  
  tmpbuf = (char *)malloc(bufsize);
  
  /* convert \n -> \r\n */
  p = buffer;
  while ((p = strstr(p, "\n")) != NULL) {
    
    if (p > buffer && *(p - 1) == '\r') {
      p++;
      continue;
    }
    
    if ((int) strlen(buffer) + 2 >= bufsize) {
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

void convert_crlf(char *buffer, int bufsize)
{
  char *p;
  
  /* convert '\n' -> \r\n */
  p = buffer;
  while ((p = strstr(p, "\\n")) != NULL) {
    
    if (p - buffer < bufsize - 2) {
      *(p++) = '\r';
      *(p++) = '\n';
    }
  }
}

/*------------------------------------------------------------------*/

char *content;

std::string retrieve_elog(Elog& elog, char *subdir, char *experiment,
                  char *uname, char *upwd, int message_id,
                  char attrib_name[maxNAttributes][NAME_LENGTH], char attrib[maxNAttributes][NAME_LENGTH], char *text)
/********************************************************************\
 * 
 *  Routine: retrive_elog
 * 
 *  Purpose: Retrive an ELog entry for edit/reply
 * 
 *  Input:
 *    char   *host            Host name where ELog server runs
 *    int    port             ELog server port number
 *    char   *subdir          Subdirectoy to elog server
 *    int    ssl              Flag for using SSL layer
 *    char   *uname           User name
 *    char   *upwd            User password
 *    int    message_id       Message to retrieve
 *    char   *attrib_name     Attribute names
 *    char   *attrib          Attribute values
 *    char   *text            Message text
 * 
 *  Function value:
 *    EL_SUCCESS              Successful completion
 * 
 * \********************************************************************/
{
  char request[100000];
  int i, n, first, index, sock;
  char str[256], encrypted_passwd[256], *ph, *ps;
    
    /* compose request */
    strcpy(request, "GET /");
  strlcpy(str, experiment, sizeof(str));
  url_encode(str, sizeof(str));
  if (subdir[0] && experiment[0])
    sprintf(request + strlen(request), "%s/%s/%d?cmd=download", subdir, str, message_id);
  else if (subdir[0])
    sprintf(request + strlen(request), "%s/%d?cmd=download", subdir, message_id);
  else if (experiment[0])
    sprintf(request + strlen(request), "%s/%d?cmd=download", str, message_id);
  strcat(request, " HTTP/1.0\r\n");
  
  sprintf(request + strlen(request), "User-Agent: ELOG\r\n");
  
  first = 1;
  
  if (uname[0]) {
    if (first)
      sprintf(request + strlen(request), "Cookie: ");
    first = 0;
    
    sprintf(request + strlen(request), "unm=%s;", uname);
  }
  
  if (upwd[0]) {
    if (first)
      sprintf(request + strlen(request), "Cookie: ");
    first = 0;
    
    do_crypt(upwd, encrypted_passwd, sizeof(encrypted_passwd));
    sprintf(request + strlen(request), "upwd=%s;", encrypted_passwd);
  }
  
  /* finish cookie line */
  if (!first)
    strcat(request, "\r\n");
  
  strcat(request, "\r\n");
  
  elog.connect();
  /* send request */
  elog.sendRequest(request);
  
  /* receive response */
  std::string response=elog.receiveRespond();
  elog.close();
  
  /* check response status */
  if (strstr(response.c_str(), "$@MID@$:")) 
  {
    /* separate attributes and message */
    
    const char* ph = strstr(response.c_str(), "========================================\n");
    
    /* skip first line */
    const char* ps = strstr(response.c_str(), "$@MID@$:");
    while (*ps && *ps != '\n')
      ps++;
    while (*ps && (*ps == '\n' || *ps == '\r'))
      ps++;
    
    for (index = 0; index < maxNAttributes; index++) {
      if (ps >= ph)
        break;
      
      strlcpy(attrib_name[index], ps, NAME_LENGTH);
      if (strchr(attrib_name[index], ':'))
        *(strchr(attrib_name[index], ':')) = 0;
      
      ps += strlen(attrib_name[index]) + 2;
      strlcpy(attrib[index], ps, NAME_LENGTH);
      
      for (i = 0; i < NAME_LENGTH; i++) {
        if (attrib[index][i] == '\r' || attrib[index][i] == '\n')
          attrib[index][i] = 0;
        
        if (attrib[index][i] == 0)
          break;
      }
      
      ps += strlen(attrib[index]);
      while (*ps && (*ps == '\n' || *ps == '\r'))
        ps++;
    }
    
    attrib_name[index][0] = 0;
    attrib[index][0] = 0;
    
    ph = strchr(ph, '\n') + 1;
    if (*ph == '\r')
      ph++;
    
    strlcpy(text, ph, TEXT_SIZE);
    return std::move(response);
  }
  else
  {
    checkResponse(response);
    throw 0;
  }    
}

/*------------------------------------------------------------------*/

int submit_elog(Elog& elog, char *subdir, char *experiment,
                char *uname, char *upwd,
                int reply,
                int quote_on_reply,
                int edit,
                int download,
                int suppress,
                int encoding,
                char attrib_name[maxNAttributes][NAME_LENGTH],
                char attrib[maxNAttributes][NAME_LENGTH],
                int n_attr,
                char *text, char afilename[maxAttachments][256],
                char *buffer[maxAttachments], int buffer_size[maxAttachments])
/********************************************************************\
 * 
 *  Routine: submit_elog
 * 
 *  Purpose: Submit an ELog entry
 * 
 *  Input:
 *    char   *subdir          Subdirectoy to elog server
 *    char   *uname           User name
 *    char   *upwd            User password
 *    int    reply            Reply to existing message
 *    int    edit             Edit existing message
 *    int    download         Download existing message
 *    int    suppress         Suppress Email notification
 *    int    encoding         0:ELCode,1:plain,2:HTML
 *    char   *attrib_name     Attribute names
 *    char   *attrib          Attribute values
 *    char   *text            Message text
 * 
 *    char   afilename[]      File names of attachments
 *    char   *buffer[]        Attachment contents
 *    int    buffer_size[]    Size of buffer in bytes
 * 
 *  Function value:
 *    EL_SUCCESS              Successful completion
 * 
 * \********************************************************************/
{
  int status, sock, i, n, header_length, content_length, index;
  char  boundary[80], str[80], encrypted_passwd[256], *p;
  char old_attrib_name[maxNAttributes+1][NAME_LENGTH], old_attrib[maxNAttributes+1][NAME_LENGTH];

  std::string response{""};
  if (edit || download) {
    if (edit)
      response = retrieve_elog(elog,subdir,experiment, uname, upwd, edit,
                             old_attrib_name, old_attrib, old_text);
      else
        response = retrieve_elog(elog,subdir,experiment, uname, upwd, download,
                               old_attrib_name, old_attrib, old_text);

        
        /* update attributes */
        for (index = 0; index < n_attr; index++) {
          for (i = 0; i < maxNAttributes && old_attrib_name[i][0]; i++)
            if (equal_ustring(attrib_name[index], old_attrib_name[i]))
              break;
            
            if (old_attrib_name[i][0])
              strlcpy(old_attrib[i], attrib[index], NAME_LENGTH);
        }
        
        /* copy attributes */
        for (i = 0; i < maxNAttributes && old_attrib_name[i][0]; i++) {
          strlcpy(attrib_name[i], old_attrib_name[i], NAME_LENGTH);
          strlcpy(attrib[i], old_attrib[i], NAME_LENGTH);
        }
        
        n_attr = i;
        
        if (text[0] == 0)
          strlcpy(text, old_text, TEXT_SIZE);
  }
  
  if (download) {
    std::cout<<"LLL"<<std::endl;
    if (strstr(response.c_str(), "$@MID@$:"))
      printf("%s", strstr(response.c_str(), "$@MID@$:"));
    else
      printf("%s", response.c_str());
    return 1;
  }
  
  if (reply) {
    response =
    retrieve_elog(elog,subdir, experiment, uname, upwd, reply,
                  old_attrib_name, old_attrib, old_text);
    
    /* update attributes */
    for (index = 0; index < n_attr; index++) {
      for (i = 0; i < maxNAttributes && old_attrib_name[i][0]; i++)
        if (equal_ustring(attrib_name[index], old_attrib_name[i]))
          break;
        
        if (old_attrib_name[i][0])
          strlcpy(old_attrib[i], attrib[index], NAME_LENGTH);
    }
    
    /* copy attributes */
    for (i = 0; i < maxNAttributes && old_attrib_name[i][0]; i++) {
      if (equal_ustring(old_attrib_name[i], "Reply to") || equal_ustring(old_attrib_name[i], "Date")) {
        attrib_name[i][0] = 0;
        attrib[i][0] = 0;
      } else {
        strlcpy(attrib_name[i], old_attrib_name[i], NAME_LENGTH);
        strlcpy(attrib[i], old_attrib[i], NAME_LENGTH);
      }
    }
    
    n_attr = i;
    
    /* check encoding */
    std::string old_encoding = "plain";
    
    for (i = 0; i < n_attr; i++)
      if (equal_ustring(attrib_name[i], "encoding"))
        break;
      
      if (i < n_attr)
        old_encoding = attrib[i];
      
      if (quote_on_reply) {
        strlcpy(new_text, text, sizeof(new_text));
        
        /* precede old text with "> " */
        text[0] = 0;
        p = old_text;
        
        do {
          if (strchr(p, '\n')) {
            *strchr(p, '\n') = 0;
            
            if (old_encoding[0] == 'H') {
              strlcat(text, "> ", TEXT_SIZE);
              strlcat(text, p, TEXT_SIZE);
              strlcat(text, "<br>\n", TEXT_SIZE);
            } else {
              strlcat(text, "> ", TEXT_SIZE);
              strlcat(text, p, TEXT_SIZE);
              strlcat(text, "\n", TEXT_SIZE);
            }
            
            p += strlen(p) + 1;
            if (*p == '\n')
              p++;
          } else {
            if (old_encoding[0] == 'H') {
              strlcat(text, "> ", TEXT_SIZE);
              strlcat(text, p, TEXT_SIZE);
              strlcat(text, "<p>\n", TEXT_SIZE);
            } else {
              strlcat(text, "> ", TEXT_SIZE);
              strlcat(text, p, TEXT_SIZE);
              strlcat(text, "\n\n", TEXT_SIZE);
            }
            
            break;
          }
          
        } while (1);
        
        strlcat(text, new_text, TEXT_SIZE);
      }
  }
    
    content_length = 100000;
  for (i = 0; i < maxAttachments; i++)
    if (afilename[i][0])
      content_length += buffer_size[i];
    content = (char *)malloc(content_length);
  if (content == NULL) {
    printf("Not enough memory\n");
    return -1;
  }
  
  /* compose content */
  srand((unsigned) time(NULL));
  sprintf(boundary, "---------------------------%04X%04X%04X", rand(), rand(), rand());
  strcpy(content, boundary);
  strcat(content, "\r\nContent-Disposition: form-data; name=\"cmd\"\r\n\r\nSubmit\r\n");
  
  if (uname[0])
    sprintf(content + strlen(content),
            "%s\r\nContent-Disposition: form-data; name=\"unm\"\r\n\r\n%s\r\n", boundary, uname);
    
    if (upwd[0]) {
      do_crypt(upwd, encrypted_passwd, sizeof(encrypted_passwd));
      sprintf(content + strlen(content),
              "%s\r\nContent-Disposition: form-data; name=\"upwd\"\r\n\r\n%s\r\n", boundary,
              encrypted_passwd);
    }
    
    if (experiment[0])
      sprintf(content + strlen(content),
              "%s\r\nContent-Disposition: form-data; name=\"exp\"\r\n\r\n%s\r\n", boundary, experiment);
      
      if (reply)
        sprintf(content + strlen(content),
                "%s\r\nContent-Disposition: form-data; name=\"reply_to\"\r\n\r\n%d\r\n", boundary, reply);
        
        if (edit) {
          sprintf(content + strlen(content),
                  "%s\r\nContent-Disposition: form-data; name=\"edit_id\"\r\n\r\n%d\r\n", boundary, edit);
          sprintf(content + strlen(content),
                  "%s\r\nContent-Disposition: form-data; name=\"skiplock\"\r\n\r\n1\r\n", boundary);
        }
        
        if (suppress)
          sprintf(content + strlen(content),
                  "%s\r\nContent-Disposition: form-data; name=\"suppress\"\r\n\r\n1\r\n", boundary);
          
          if (encoding == 0)
            sprintf(content + strlen(content),
                    "%s\r\nContent-Disposition: form-data; name=\"encoding\"\r\n\r\nELCode\r\n", boundary);
            else if (encoding == 1)
              sprintf(content + strlen(content),
                      "%s\r\nContent-Disposition: form-data; name=\"encoding\"\r\n\r\nplain\r\n", boundary);
              else if (encoding == 2)
                sprintf(content + strlen(content),
                        "%s\r\nContent-Disposition: form-data; name=\"encoding\"\r\n\r\nHTML\r\n", boundary);
                
                for (i = 0; i < n_attr; i++) {
                  strcpy(str, attrib_name[i]);
                  if (str[0]) {
                    stou(str);
                    sprintf(content + strlen(content),
                            "%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n", boundary, str, attrib[i]);
                  }
                }
                
                if (text[0])
                  sprintf(content + strlen(content),
                          "%s\r\nContent-Disposition: form-data; name=\"Text\"\r\n\r\n%s\r\n%s\r\n",
                          boundary, text, boundary);
                  
                  content_length = strlen(content);
                p = content + content_length;
                
                for (i = 0; i < maxAttachments; i++)
                  if (afilename[i][0]) {
                    sprintf(p,
                            "Content-Disposition: form-data; name=\"attfile%d\"; filename=\"%s\"\r\n\r\n",
                            i + 1, afilename[i]);
                    
                    content_length += strlen(p);
                    p += strlen(p);
                    memcpy(p, buffer[i], buffer_size[i]);
                    p += buffer_size[i];
                    strcpy(p, boundary);
                    strcat(p, "\r\n");
                    
                    content_length += buffer_size[i] + strlen(p);
                    p += strlen(p);
                  }
                  
                  /* compose request */
                  char request[100000];
                  strcpy(request, "POST /");
                  if (subdir[0])
                    sprintf(request + strlen(request), "%s/", subdir);
                  if (experiment[0]) {
                    strcpy(str, experiment);
                    url_encode(str, sizeof(str));
                    sprintf(request + strlen(request), "%s/", str);
                  }
                  strcat(request, " HTTP/1.0\r\n");
                  
                  sprintf(request + strlen(request), "Content-Type: multipart/form-data; boundary=%s\r\n", boundary);
                  if(elog.getPort() != 80) sprintf(str, "%s:%d", elog.getHostname().c_str(), elog.getPort());
                  else sprintf(str, "%s", elog.getHostname().c_str());
                  sprintf(request + strlen(request), "Host: %s\r\n", str);
                  sprintf(request + strlen(request), "User-Agent: ELOG\r\n");
                  sprintf(request + strlen(request), "Content-Length: %d\r\n", content_length);
                  
                  strcat(request, "\r\n");
                  
                  header_length = strlen(request);
                  
                  elog.connect();
                  /* send request */
                  elog.sendRequest(request);
                  
                  /* send content */
                  elog.sendRequest(content, content_length);
                  
                  /* receive response */
                  response=elog.receiveRespond();
                  elog.close();
                  /* check response status */
                  checkResponse(response);
                  
                  return 1;
}

/*------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
  Elog elog;
  Type type{New};
  char str[1000], uname[80], upwd[80];
  char logbook[32], textfile[256], subdir[256];
  char *buffer[maxAttachments], attachment[maxAttachments][256];
  int att_size[maxAttachments];
  int i, n, fh, n_att, n_attr, port, reply, quote_on_reply, edit, download, encoding, suppress, size, ssl,
  text_flag;
  char attr_name[maxNAttributes][NAME_LENGTH], attrib[maxNAttributes][NAME_LENGTH];
  
  text[0] = textfile[0] = uname[0] = upwd[0] = suppress = quote_on_reply = 0;
  logbook[0] = subdir[0] = 0;
  n_att = n_attr = reply = edit = download = encoding = 0;
  text_flag = 0;
  
  for (i = 0; i < maxAttachments; i++) {
    attachment[i][0] = 0;
    buffer[i] = NULL;
    att_size[i] = 0;
  }
  
  /* parse command line parameters */
  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-' && argv[i][1] == 'v')
      verbose = 1;
    else if (argv[i][0] == '-' && argv[i][1] == 's')
      elog.setSSL(true);
    else if (argv[i][0] == '-' && argv[i][1] == 'q')
      quote_on_reply = 1;
    else if (argv[i][0] == '-' && argv[i][1] == 'x')
      suppress = 1;
    else {
      if (argv[i][0] == '-') {
        if (i + 1 >= argc || argv[i + 1][0] == '-') return usage();
        if (argv[i][1] == 'h') elog.setHostname(argv[++i]);
        else if (argv[i][1] == 'p') elog.setPort(atoi(argv[++i]));
        else if (argv[i][1] == 'l')
          strcpy(logbook, argv[++i]);
        else if (argv[i][1] == 'd')
          strcpy(subdir, argv[++i]);
        else if (argv[i][1] == 'u') {
          strcpy(uname, argv[++i]);
          strcpy(upwd, argv[++i]);
        } else if (argv[i][1] == 'a') {
          strcpy(str, argv[++i]);
          if (strchr(str, '=')) {
            strcpy(attrib[n_attr], strchr(str, '=') + 1);
            *strchr(str, '=') = 0;
            strcpy(attr_name[n_attr], str);
            n_attr++;
          } else {
            printf("Error: Attributes must be supplied in the form \"-a <attribute>=<value>\".\n");
            return 1;
          }
        } else if (argv[i][1] == 'f')
          strcpy(attachment[n_att++], argv[++i]);
        else if (argv[i][1] == 'r')
          reply = atoi(argv[++i]);
        else if (argv[i][1] == 'e')
          edit = atoi(argv[++i]);
        else if (argv[i][1] == 'w') {
          if (argv[i+1][0] == 'l')
            download = -1;
          else
            download = atoi(argv[++i]);
        } else if (argv[i][1] == 'n')
          encoding = atoi(argv[++i]);
        else if (argv[i][1] == 'm') {
          strcpy(textfile, argv[++i]);
          text_flag = 1;
        } 
        else return usage();
      } else {
        strcpy(text, argv[i]);
        convert_crlf(text, sizeof(text));
        text_flag = 1;
      }
    }
  }

  

  
  if (logbook[0] == 0) {
    printf("Please specify logbook with the \"-l\" flag.\n");
    return 1;
  }
  
  if (n_attr == 0 && !edit && !reply && !download) {
    printf("Please specify attribute(s) with the \"-a\" flag.\n");
    return 1;
  }
  
  fh = -1;
  
  if (textfile[0]) {
    fh = open(textfile, O_RDONLY | O_BINARY);
    if (fh < 0) {
      printf("Message file \"%s\" does not exist.\n", textfile);
      return 1;
    }
    
    size = (int) lseek(fh, 0, SEEK_END);
    lseek(fh, 0, SEEK_SET);
    
    if (size > (int) (sizeof(text) - 1)) {
      printf("Message file \"%s\" is too long (%zd bytes max).\n", textfile, sizeof(text));
      return 1;
    }
    
    i = read(fh, text, size);
    
    if (i < size) {
      printf("Cannot fully read message from file %s.\n", textfile);
      return 1;
    }
    
    close(fh);
  }
  
  if (text_flag == 0 && !edit && !download) {
    /* read from stdin */
    
    n = 0;
    
    do {
      i = getchar();
      
      text[n++] = i;
      
    } while (i != EOF);
    
    if (n > 0)
      text[n - 1] = 0;
  }
  
  /* change CR -> CRLF for unix text files */
  add_crlf(text, sizeof(text));
  
  /*---- open attachment file ----*/
  
  for (i = 0; i < maxAttachments; i++) {
    if (!attachment[i][0])
      break;
    
    fh = open(attachment[i], O_RDONLY | O_BINARY);
    if (fh < 0) {
      printf("Attachment file \"%s\" does not exist.\n", attachment[i]);
      return 1;
    }
    
    att_size[i] = lseek(fh, 0, SEEK_END);
    lseek(fh, 0, SEEK_SET);
    
    buffer[i] = (char *)malloc(att_size[i] + 1);
    
    n = read(fh, buffer[i], att_size[i]);
    if (n < att_size[i]) {
      printf("Cannot fully read attachment file \"%s\".\n", attachment[i]);
      return 1;
    }
    buffer[i][n] = 0;
    
    close(fh);
  }

  /* now submit message */
  submit_elog(elog,subdir, logbook,
              uname, upwd, reply, quote_on_reply, edit, download, suppress, encoding, attr_name, attrib, n_attr, text,
              attachment, buffer, att_size);

  for (i = 0; i < maxAttachments; i++)
    if (buffer[i])
      free(buffer[i]);
    
    return 0;
}
