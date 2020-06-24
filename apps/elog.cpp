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
#include "Elog.hpp"

#include <iostream>

int usage() 
{
  std::cout << "usage: elog\n";
  std::cout << "elog -h <hostname> [-p port] [-d subdir]\n";
  std::cout << "Location where elogd is running\n";
  std::cout << "-l logbook/experiment    Name of logbook or experiment\n";
  std::cout << "-s                       Use SSL for communication\n";
  std::cout << "[-v]                     For verbose output\n";
  std::cout << "[-u username password]   User name and password\n";
  std::cout << "[-f <attachment>]        (up to "<< elogpp::Elog::getMaxAttachments()<< " attachments)\n";
  std::cout << "-a <attribute>=<value>   (up to " << elogpp::Elog::getMaxAttributes()<< " attributes)\n";
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

int main(int argc, char *argv[]) 
{
  elogpp::Elog elog;
  for(std::size_t i = 1; i < argc; i++) 
  {
    std::string key{argv[i]};
    if(key=="-v") elog.setVerbosity(true);
    else if(key=="-s") elog.setSSL(true);
    else if(key=="-q") elog.setQuoteInReply(true);
    else if(key=="-x") elog.setSupressEmails(true);
    else if(key[0]=='-')
    {
      if(i + 1 >= argc || argv[i + 1][0] == '-') usage();
      std::string value{argv[++i]};
      if(key == "-h")  elog.setHostname(value);
      else if(key == "-p") elog.setPort(std::stoi(value));
      else if(key == "-l") elog.setLogbook(value);
      else if(key == "-d")  elog.setSubdir(value);
      else if(key == "-u") 
      {
        elog.setUserName(value);
        elog.setPassword(argv[++i]);
      } 
      else if(key == "-a") 
      {
        std::size_t found=value.find('=');
        if(found!=std::string::npos) 
        {
          elog.addAttribute(value.substr(0,found),value.substr(found+1));
        } 
        else 
        {
          std::cout<<"Error: Attributes must be supplied in the form \"-a <attribute>=<value>\".\n";
          return 1;
        }
      } 
      else if(key == "-f") elog.addAttachment(value);
      else if(key == "-r") 
      {
        elog.setType(elogpp::Reply);
        elog.setID(std::stoi(value));
      } 
      else if(key == "-e") 
      {
        elog.setType(elogpp::Edit);
        elog.setID(std::stoi(value));
      } 
      else if(key == "-w") 
      {
        elog.setType(elogpp::Download);
        if(value == "l") elog.setID(-1);
        else elog.setID(std::stoi(value)); 
      } 
      else if(key == "-n") elog.setEncoding(std::stoi(value));
      else if(key == "-m") 
      {
        elog.setTextFile(value);
      } 
      else usage();
    } 
    else 
    {
      elog.setText(key);
    }
  }


  if(elog.getLogbook().empty()) 
  {
    std::cout<<"Please specify logbook with the \"-l\" flag.\n";
    return 1;
  }

  if(elog.hasText() == false && (elog.getType()==elogpp::New || elog.getType()==elogpp::Reply)) 
  {
    std::cout<<"Please provide a text or a text file"<<std::endl;
    return 1;
  }

  /* now submit message */
  elog.SubmitElog();
  
  if(elog.getType()==elogpp::Download)
  {
    std::map<std::string,std::string> attributes=elog.getAttributes();
    for(std::map<std::string,std::string>::iterator it=attributes.begin();it!=attributes.end();++it)
    {
      std::cout<<it->first<<":"<<it->second<<std::endl;
    }
    std::cout<<"========================================"<<std::endl;
    std::cout<<elog.getText()<<std::endl;
  }
  
  return 0;
}
