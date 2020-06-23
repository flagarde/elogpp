/********************************************************************\
 *
 *  Name:         elog.c
 *  Created by:   Stefan Ritt
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
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "Crypt.hpp"
#include "Connector.hpp"

constexpr int maxAttachments{50};
constexpr int maxNAttributes{50};

int usage() {
  std::cout << "usage: elog\n";
  std::cout << "elog -h <hostname> [-p port] [-d subdir]\n";
  std::cout << "Location where elogd is running\n";
  std::cout << "-l logbook/experiment    Name of logbook or experiment\n";
  std::cout << "-s                       Use SSL for communication\n";
  std::cout << "[-v]                     For verbose output\n";
  std::cout << "[-u username password]   User name and password\n";
  std::cout << "[-f <attachment>]        (up to "<< maxAttachments<< " attachments)\n";
  std::cout << "-a <attribute>=<value>   (up to " << maxNAttributes<< " attributes)\n";
  std::cout << "[-r <id>]                Reply to existing message\n";
  std::cout << "[-q]                     Quote original text on reply\n";
  std::cout << "[-e <id>]                Edit existing message\n";
  std::cout << "[-w <id>|last]           Download existing message (last message)\n";
  std::cout << "[-x]                     Suppress email notification\n";
  std::cout << "[-n 0|1|2]               Encoding: 0:ELcode,1:plain,2:HTML\n";
  std::cout << "     -m <textfile>] | <text>\n";
  std::cout << "\nArguments with blanks must be enclosed in quotes\n";
  std::cout << "The elog message can either be submitted on the command line, piped in like\n";
  std::cout << "\"cat text | elog -h ... -l ... -a ...\" or in a file with the -m flag.\n";
  std::cout << "Multiple attributes and attachments can be supplied\n";
  return 1;
}

enum Type { Edit, Download, Reply, New };


#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#ifdef _MSC_VER
#include <io.h>
#include <windows.h>
#else
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#define closesocket(s) ::close(s)
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif

class Elog
{
public:
  
private:
  
};

#define NAME_LENGTH 500
#define TEXT_SIZE 100000

char text[TEXT_SIZE], old_text[TEXT_SIZE], new_text[TEXT_SIZE];

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

void stou(char *str)
/* convert all special characters to underscores in a string */
{
  int i;

  for (i = 0; i < (int)strlen(str); i++)
    if (str[i] == ' ' || str[i] == '.' || str[i] == '/' || str[i] == '\\' ||
        str[i] == '-' || str[i] == '(' || str[i] == ')')
      str[i] = '_';
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

void convert_crlf(char *buffer, int bufsize) {
  char *p;

  /* convert '\n' -> \r\n */
  p = buffer;
  while ((p = strstr(p, "\\n")) != nullptr) {

    if (p - buffer < bufsize - 2) {
      *(p++) = '\r';
      *(p++) = '\n';
    }
  }
}


std::string retrieve_elog(elogpp::Connector& connector,const std::string& subdir,const std::string& logbook, char *uname, char *upwd,const int& ID,char attrib_name[maxNAttributes][NAME_LENGTH],char attrib[maxNAttributes][NAME_LENGTH], char *text)
{
  int  first;
  char str[256];
  connector.connect();
  char request[100000];
  /* compose request */
  strcpy(request, "GET /");
  //strlcpy(str, experiment, sizeof(str));
  std::string encodedlogbook=url_encode(logbook);
//  url_encode(logbook.c_str(), sizeof(logbook.c_str()));
  if(!subdir.empty() && !logbook.empty()) sprintf(request + strlen(request), "%s/%s/%d?cmd=download", subdir.c_str(), encodedlogbook.c_str(),ID);
  else if (!subdir.empty()) sprintf(request + strlen(request), "%s/%d?cmd=download", subdir.c_str(),ID);
  else if(!logbook.empty()) sprintf(request + strlen(request), "%s/%d?cmd=download", encodedlogbook.c_str(),ID);
  strcat(request, " HTTP/1.0\r\n");

  sprintf(request + strlen(request), "User-Agent: ELOG\r\n");

  first = 1;

  if (uname[0]) 
  {
    if (first) sprintf(request + strlen(request), "Cookie: ");
    first = 0;
    sprintf(request + strlen(request), "unm=%s;", uname);
  }

  if (upwd[0]) 
  {
    if (first) sprintf(request + strlen(request), "Cookie: ");
    first = 0;
    sprintf(request + strlen(request), "upwd=%s;", elogpp::do_crypt(upwd).c_str());
  }

  /* finish cookie line */
  if (!first) strcat(request, "\r\n");
  strcat(request, "\r\n");

/* send request */
connector.send(request);

  /* receive response */
  std::string resp=connector.receive();

  connector.disconnect();

  /* check response status */
  if(strstr(resp.c_str(), "$@MID@$:")) 
  {
    int index=0;
    /* separate attributes and message */
    const char* ph = strstr(resp.c_str(), "========================================\n");
    /* skip first line */
    const char* ps = strstr(resp.c_str(), "$@MID@$:");
    while (*ps && *ps != '\n') ps++;
    while (*ps && (*ps == '\n' || *ps == '\r')) ps++;

    for(index = 0; index < maxNAttributes; index++) 
    {
      if (ps >= ph) break;

      strlcpy(attrib_name[index], ps, NAME_LENGTH);
      if (strchr(attrib_name[index], ':')) *(strchr(attrib_name[index], ':')) = 0;

      ps += strlen(attrib_name[index]) + 2;
      strlcpy(attrib[index], ps, NAME_LENGTH);

      for(std::size_t i = 0; i < NAME_LENGTH; i++) 
      {
        if (attrib[index][i] == '\r' || attrib[index][i] == '\n') attrib[index][i] = 0;
        if (attrib[index][i] == 0) break;
      }

      ps += strlen(attrib[index]);
      while (*ps && (*ps == '\n' || *ps == '\r')) ps++;
    }

    attrib_name[index][0] = 0;
    attrib[index][0] = 0;

    ph = strchr(ph, '\n') + 1;
    if (*ph == '\r') ph++;

    strlcpy(text, ph, TEXT_SIZE);

    return resp;
  }
  else if (strstr(resp.c_str(), "302 Found")) 
  {
    if(strstr(resp.c_str(), "Location:")) 
    {
      if (strstr(resp.c_str(), "fail")) printf("Error: Invalid user name or password\n");
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
  } 
  else if (strstr(resp.c_str(), "Logbook Selection")) printf("Error: No logbook specified\n");
  else if (strstr(resp.c_str(), "enter password")) printf("Error: Missing or invalid password\n");
  else if (strstr(resp.c_str(), "form name=form1")) printf("Error: Missing or invalid user name/password\n");
  else printf("Error transmitting message\n");

  throw 0;
}

/*------------------------------------------------------------------*/

int submit_elog(elogpp::Connector& connector,const Type &type, const int &ID,const std::string& subdir,const std::string& logbook, char *uname, char *upwd, int quote_on_reply, int suppress, int encoding, char attrib_name[maxNAttributes][NAME_LENGTH], char attrib[maxNAttributes][NAME_LENGTH], int n_attr, char *text, const std::vector<std::string>& attachments, char *buffer[maxAttachments], int buffer_size[maxAttachments])
{
  int header_length, content_length;
  char boundary[80], str[80], *p;
  char old_attrib_name[maxNAttributes + 1][NAME_LENGTH],
      old_attrib[maxNAttributes + 1][NAME_LENGTH];

      
  std::string response{""};
  if(type!=New) response=retrieve_elog(connector,subdir, logbook, uname, upwd, ID,old_attrib_name, old_attrib, old_text);
  if(type == Edit || type == Download) 
  {
    /* update attributes */
    for(std::size_t index = 0; index < n_attr; index++) 
    {
      int i{0};
      for( i = 0; i < maxNAttributes && old_attrib_name[i][0]; i++) if (equal_ustring(attrib_name[index], old_attrib_name[i])) break;
      if(old_attrib_name[i][0]) strlcpy(old_attrib[i], attrib[index], NAME_LENGTH);
    }

    /* copy attributes */
    for(std::size_t i = 0; i < maxNAttributes && old_attrib_name[i][0]; i++) 
    {
      strlcpy(attrib_name[i], old_attrib_name[i], NAME_LENGTH);
      strlcpy(attrib[i], old_attrib[i], NAME_LENGTH);
    }
    if (text[0] == 0) strlcpy(text, old_text, TEXT_SIZE);
  }

  if(type == Download) 
  {
    if (strstr(response.c_str(), "$@MID@$:")) printf("%s", strstr(response.c_str(), "$@MID@$:"));
    else printf("%s", response.c_str());
    return 1;
  }

  if (type == Reply) 
  {
    /* update attributes */
    for(std::size_t index = 0; index < n_attr; index++) 
    {
      int i{0};
      for(i = 0; i < maxNAttributes && old_attrib_name[i][0]; i++) if (equal_ustring(attrib_name[index], old_attrib_name[i])) break;
      if(old_attrib_name[i][0]) strlcpy(old_attrib[i], attrib[index], NAME_LENGTH);
    }

    /* copy attributes */
    for(std::size_t i = 0; i < maxNAttributes && old_attrib_name[i][0]; i++) 
    {
      if (equal_ustring(old_attrib_name[i], "Reply to") || equal_ustring(old_attrib_name[i], "Date")) 
      {
        attrib_name[i][0] = 0;
        attrib[i][0] = 0;
      } 
      else 
      {
        strlcpy(attrib_name[i], old_attrib_name[i], NAME_LENGTH);
        strlcpy(attrib[i], old_attrib[i], NAME_LENGTH);
      }
    }

    /* check encoding */
    std::string old_encoding = "plain";
    int i{0};
    for(i = 0; i < n_attr; i++) if (equal_ustring(attrib_name[i], "encoding")) break;

    if (i < n_attr) old_encoding = attrib[i];

    if (quote_on_reply) 
    {
      strlcpy(new_text, text, sizeof(new_text));
      /* precede old text with "> " */
      text[0] = 0;
      p = old_text;
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
      strlcat(text, new_text, TEXT_SIZE);
    }
  }
 
  connector.connect();
  content_length = 100000;
  for(std::size_t i = 0; i < attachments.size(); i++) content_length += buffer_size[i];
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

  if (uname[0]) sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"unm\"\r\n\r\n%s\r\n",boundary, uname);

  if (upwd[0]) 
  {
    sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"upwd\"\r\n\r\n%s\r\n",boundary, elogpp::do_crypt(upwd).c_str());
  }

  if(!logbook.empty()) sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"exp\"\r\n\r\n%s\r\n",boundary,logbook.c_str());

  if (type == Reply) sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"reply_to\"\r\n\r\n%d\r\n",boundary, ID);

  if (type == Edit) 
  {
    sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"edit_id\"\r\n\r\n%d\r\n",boundary, ID);
    sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"skiplock\"\r\n\r\n1\r\n",boundary);
  }

  if (suppress) sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"suppress\"\r\n\r\n1\r\n",boundary);

  if (encoding == 0) sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"encoding\"\r\n\r\nELCode\r\n",boundary);
  else if (encoding == 1) sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"encoding\"\r\n\r\nplain\r\n",boundary);
  else if (encoding == 2) sprintf(content + strlen(content), "%s\r\nContent-Disposition: form-data; name=\"encoding\"\r\n\r\nHTML\r\n",boundary);

  for(std::size_t i = 0; i < n_attr; i++) 
  {
    strcpy(str, attrib_name[i]);
    if (str[0]) 
    {
      stou(str);
      sprintf(content + strlen(content),"%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n",boundary, str, attrib[i]);
    }
  }

  if (text[0]) sprintf(content + strlen(content), "%s\r\nContent-Disposition: form-data; name=\"Text\"\r\n\r\n%s\r\n%s\r\n",boundary, text, boundary);

  content_length = strlen(content);
  p = content + content_length;

  for(std::size_t i = 0; i < attachments.size(); i++)
  {
      sprintf(p,"Content-Disposition: form-data; name=\"attfile%d\"; filename=\"%s\"\r\n\r\n",i + 1, attachments[i].c_str());
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
  if (subdir[0]) sprintf(request + strlen(request), "%s/", subdir);
  if(!logbook.empty()) 
  {
    sprintf(request + strlen(request), "%s/",url_encode(logbook).c_str());
  }
  strcat(request, " HTTP/1.0\r\n");

  sprintf(request + strlen(request), "Content-Type: multipart/form-data; boundary=%s\r\n", boundary);
  if (connector.getPort()!= 80) sprintf(str, "%s:%d", connector.getHostname().c_str(),connector.getPort());
  else sprintf(str, "%s", connector.getHostname().c_str());
  sprintf(request + strlen(request), "Host: %s\r\n", str);
  sprintf(request + strlen(request), "User-Agent: ELOG\r\n");
  sprintf(request + strlen(request), "Content-Length: %d\r\n", content_length);

  strcat(request, "\r\n");

  header_length = strlen(request);

connector.send(request);
connector.send(content,content_length);
  /* receive response */
  std::string resp=connector.receive();

  connector.disconnect();

  /* check response status */
  if (strstr(resp.c_str(), "302 Found")) 
  {
    if (strstr(resp.c_str(), "Location:")) 
    {
      if (strstr(resp.c_str(), "has moved")) printf("Error: elogd server has moved to another location\n");
      else if (strstr(resp.c_str(), "fail")) printf("Error: Invalid user name or password\n");
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
  else if (strstr(resp.c_str(), "Logbook Selection")) printf("Error: No logbook specified\n");
  else if (strstr(resp.c_str(), "enter password")) printf("Error: Missing or invalid password\n");
  else if (strstr(resp.c_str(), "Error: Attribute")) 
  {
    if (strstr(resp.c_str(), "not existing")) 
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
  else if (strstr(resp.c_str(), "form name=form1")) printf("Error: Missing or invalid user name/password\n");
  else printf("Error transmitting message\n");
  return 1;
}

/*------------------------------------------------------------------*/

int main(int argc, char *argv[]) 
{
  Type type{New};
  int ID{0};
  elogpp::Connector connector;
  std::vector<std::string> attachments;
  std::string logbook{""};
  std::string subdir{""};
  std::string textfile{""};
  bool text_flag{false};
  std::map<std::string,std::string> attributes;
  
  
  char str[1000], uname[80], upwd[80];
  char *buffer[maxAttachments];
  int att_size[maxAttachments];
  int i, n, n_att, n_attr, port,  quote_on_reply ,encoding, suppress, size;
  char attr_name[maxNAttributes][NAME_LENGTH],
      attrib[maxNAttributes][NAME_LENGTH];

  text[0] = uname[0] = upwd[0] = suppress = quote_on_reply = 0;
  n_att = n_attr = encoding = 0;

  for(std::size_t i = 0; i < maxAttachments; i++) 
  {
    buffer[i] = nullptr;
    att_size[i] = 0;
  }

  /* parse command line parameters */
  for(std::size_t i = 1; i < argc; i++) 
  {
    if (argv[i][0] == '-' && argv[i][1] == 'v') connector.setVerbosity(true);
    else if (argv[i][0] == '-' && argv[i][1] == 's') connector.setSSL(true);
    else if (argv[i][0] == '-' && argv[i][1] == 'q')
      quote_on_reply = 1;
    else if (argv[i][0] == '-' && argv[i][1] == 'x')
      suppress = 1;
    else {
      if (argv[i][0] == '-') {
        if (i + 1 >= argc || argv[i + 1][0] == '-')
          usage();
        if (argv[i][1] == 'h')  connector.setHostname(std::string(argv[++i]));
        else if (argv[i][1] == 'p') connector.setPort(atoi(argv[++i]));
        else if (argv[i][1] == 'l') logbook=std::string(argv[++i]);
        else if (argv[i][1] == 'd')  subdir=std::string(argv[++i]);
        else if (argv[i][1] == 'u') 
        {
          strcpy(uname, argv[++i]);
          strcpy(upwd, argv[++i]);
        } 
        else if (argv[i][1] == 'a') 
        {
          strcpy(str, argv[++i]);
          if (strchr(str, '=')) 
          {
            
            //attributes.emplace(
            strcpy(attrib[n_attr], strchr(str, '=') + 1);
            *strchr(str, '=') = 0;
            strcpy(attr_name[n_attr], str);
            n_attr++;
          } 
          else 
          {
            std::cout<<"Error: Attributes must be supplied in the form \"-a <attribute>=<value>\".\n";
            return 1;
          }
        } 
        else if (argv[i][1] == 'f') attachments.push_back(std::string(argv[++i]));
        else if (argv[i][1] == 'r') 
        {
          type = Reply;
          ID = atoi(argv[++i]);
        } 
        else if (argv[i][1] == 'e') 
        {
          type = Edit;
          ID = atoi(argv[++i]);
        } 
        else if (argv[i][1] == 'w') 
        {
          type = Download;
          if (argv[i + 1][0] == 'l') ID = -1;
          else
          ID = std::stoi(argv[++i]); 
        } 
        else if (argv[i][1] == 'n') encoding = atoi(argv[++i]);
        else if (argv[i][1] == 'm') 
        {
          textfile=std::string(argv[++i]);
          text_flag = true;
        } 
        else usage();
      } 
      else 
      {
        strcpy(text, argv[i]);
        convert_crlf(text, sizeof(text));
        text_flag = true;
      }
    }
  }

  if(logbook.empty()) 
  {
    std::cout<<"Please specify logbook with the \"-l\" flag.\n";
    return 1;
  }


  if(!textfile.empty()) 
  {
    int fh = open(textfile.c_str(), O_RDONLY | O_BINARY);
    if (fh < 0) 
    {
      std::cout<<"Message file \""<<textfile<<"\" does not exist.\n";
      return 1;
    }
    int size = (int)lseek(fh, 0, SEEK_END);
    lseek(fh, 0, SEEK_SET);
    if (size > (int)(sizeof(text) - 1)) 
    {
      std::cout<<"Message file \""<<textfile<<"\" is too long ("<<sizeof(text)<<" bytes max).\n";
      return 1;
    }
    int i = read(fh, text, size);
    if (i < size) 
    {
      std::cout<<"Cannot fully read message from file "<<textfile<<".\n";
      return 1;
    }
    close(fh);
    add_crlf(text, sizeof(text));
  }

  if(text_flag == false && (type==New || type==Reply)) 
  {
    /* read from stdin */
    int n{0};
    int i{0};
    do 
    {
      i = getchar();
      text[n++] = i;
    } while (i != EOF);
    if(n > 0) text[n - 1] = 0;
    /* change CR -> CRLF for unix text files */
    add_crlf(text, sizeof(text));
  }

  /*---- open attachment file ----*/

  for(std::size_t i = 0; i < attachments.size(); i++) 
  {

    int fh = open(attachments[i].c_str(), O_RDONLY | O_BINARY);
    if (fh < 0) 
    {
      std::cout<<"Attachment file \""<<attachments[i].c_str()<<"\" does not exist.\n";
      return 1;
    }

    att_size[i] = lseek(fh, 0, SEEK_END);
    lseek(fh, 0, SEEK_SET);

    buffer[i] = (char *)malloc(att_size[i] + 1);

    n = read(fh, buffer[i], att_size[i]);
    if (n < att_size[i]) 
    {
      std::cout<<"Cannot fully read attachment file \""<<attachments[i].c_str()<<"\".\n";
      return 1;
    }
    buffer[i][n] = 0;

    close(fh);
  }

  /* now submit message */
  submit_elog(connector,type, ID, subdir, logbook, uname, upwd,quote_on_reply, suppress, encoding, attr_name, attrib, n_attr,text, attachments, buffer, att_size);

  for(std::size_t i = 0; i < maxAttachments; i++) if(buffer[i]) free(buffer[i]);
  return 0;
}
