# elogpp

Wrapper for elog https://elog.psi.ch/elog/

## Goal

elogpp allows user to send and retrieve/parse entry from an elog server (see example).

> [📖 Documentation](https://flagarde.github.io/elogpp/) [![Docs](https://github.com/flagarde/elogpp/actions/workflows/Docs.yml/badge.svg)](https://github.com/flagarde/elogpp/actions/workflows/Docs.yml)
</h1> 

[LC]: https://github.com/flagarde/elogpp/actions/workflows/Linux-Clang.yml
[LCB]: https://github.com/flagarde/elogpp/actions/workflows/Linux-Clang.yml/badge.svg

[LG]: https://github.com/flagarde/elogpp/actions/workflows/Linux-GCC.yml
[LGB]: https://github.com/flagarde/elogpp/actions/workflows/Linux-GCC.yml/badge.svg

[MC]: https://github.com/flagarde/elogpp/actions/workflows/MacOS-Clang.yml
[MCB]: https://github.com/flagarde/elogpp/actions/workflows/MacOS-Clang.yml/badge.svg

[MG]: https://github.com/flagarde/elogpp/actions/workflows/MacOS-GCC.yml
[MGB]: https://github.com/flagarde/elogpp/actions/workflows/MacOS-GCC.yml/badge.svg

[MS]: https://github.com/flagarde/elogpp/actions/workflows/Windows-MSYS2.yml
[MSB]: https://github.com/flagarde/elogpp/actions/workflows/Windows-MSYS2.yml/badge.svg

[MM]: https://github.com/flagarde/elogpp/actions/workflows/Windows-MSVC.yml
[MMB]: https://github.com/flagarde/elogpp/actions/workflows/Windows-MSVC.yml/badge.svg

## Builds

|        | Linux Clang | Linux GCC | MacOS Clang | MacOS GCC | Windows M2sys | Windows MSVC |
|--------|-------------|-----------|-------------|-----------|---------------|--------------|
| Github |[![Linux Clang][LCB]][LC]|[![Linux GCC][LGB]][LG]|[![MacOS Clang][MCB]][MC]|[![MacOS GCC][MGB]][MG]|[![Windows MSYS2][MSB]][MS]|[![Windows MSVC][MMB]][MM]|

## Use

```cpp
#include "Manager.hpp"
#include "User.hpp"
#include "Server.hpp"
#include <iostream>

using namespace cxx::elog;

int main()
{
  User user("me","1234567");
  Server server = Server().setHostname("127.0.0.1").setPort("4444").setSSL(true).setName("Elog").setDescription("Elog test server").setCACertificateFile("./server.crt");
  server.addLogbook("d", "Demo logbook");
  Manager manager;
  Entry entry= manager.createEntry();
  std::cout<<"*****************************************************"<<std::endl;
  std::cout<<"* 1) Last entry is :                                *"<<std::endl;
  std::cout<<"*****************************************************"<<std::endl;
  std::cout << "Last ID: " << entry.user(user).to(server,"d").getLastID() << std::endl;
  entry.user(user).to(server,"d").receiveEntry("last").print();

  std::cout<<"*****************************************************"<<std::endl;
  std::cout<<"* 2) Now creating a new entry                       *"<<std::endl;
  std::cout<<"*****************************************************"<<std::endl;
  Entry entry4= manager.createEntry();
  entry4.setAttribute("Type","Problem Fixed");
  entry4.setAttribute("Category","Other");
  entry4.setAttribute("Author","Boss");
  entry4.setMessage("Please say something !");
  entry4.user(user).to(server,"d").send("V");
  entry4.print();

  std::cout<<"*****************************************************"<<std::endl;
  std::cout<<"* 3) Now replying to it                             *"<<std::endl;
  std::cout<<"*****************************************************"<<std::endl;
  Entry entry2= manager.createEntry();
  entry2.setAttribute("Type","Routine");
  entry2.setAttribute("Author","Toto");
  entry2.setMessage("Something !");
  entry2.user(user).to(server,"d").replyTo("last").send();
  entry2.print();

  std::cout<<"*****************************************************"<<std::endl;
  std::cout<<"* 4) Now the last entry is :                        *"<<std::endl;
  std::cout<<"*****************************************************"<<std::endl;
  Entry entry3= manager.createEntry();
  entry3.user(user).to(server,"d").receiveEntry("last");
  entry3.print();

  std::cout<<"*****************************************************"<<std::endl;
  std::cout<<"* 5) I can extract data from the new last entry !   *"<<std::endl;
  std::cout<<"*****************************************************"<<std::endl;
  std::cout<<"His Author is : "<<entry3.getAttribute("Author")<<std::endl;
  std::cout<<"And he said : "<<entry3.getText()<<std::endl;
}
```

