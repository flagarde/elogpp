# elogpp

Wrapper for elog https://elog.psi.ch/elog/

## Goal

elogpp allows user to send and retrieve/parse entry from an elog server (see example).

<h1>
  
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

* Write a configuration file :

```json
{
    "ElogServers":
    [
        {
            "Name":"Elog",
            "Description":"Elog test server",
            "Hostname":"localhost",
            "Port":"8080",
            "SSL":false,
            "Logbooks":
            [
                {"Name":"demo", "Description":"demo"}
            ]
        }
    ],
    "ElogUsers":
    [
        {"Name":"toto","Password":"123456"}
    ]
}
```

* Export this configuration :
```bash
export ELOG_CONFIG_PATH=path
export ELOG_CONFIG_FILE=Elog.json
```

## Example :
```cpp
#include "ElogManager.hpp"
#include <iostream>

using namespace elogpp;

int main() 
{
    ElogManager manager;
    manager.print();
    ElogEntry entry= manager.createEntry();
    std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"* 1) Last entry is :                                *"<<std::endl;
    std::cout<<"*****************************************************"<<std::endl;
    entry.user("toto").to("Elog","demo").receiveEntry("last");
    entry.print();
    
    std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"* 2) Now creating a new entry                       *"<<std::endl; 
    std::cout<<"*****************************************************"<<std::endl;
    ElogEntry entry4= manager.createEntry();
    entry4.setAttribute("Type","Problem Fixed");
    entry4.setAttribute("Category","Other");
    entry4.setAttribute("Author","Boss");
    entry4.setMessage("Please say something !");
    entry4.user("toto").to("Elog","demo").send("V");
    entry4.print();
    
    std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"* 3) Now replying to it                             *"<<std::endl;
    std::cout<<"*****************************************************"<<std::endl;
    ElogEntry entry2= manager.createEntry();
    entry2.setAttribute("Type","Routine");
    entry2.setAttribute("Author","Toto");
    entry2.setMessage("Something !");
    entry2.user("toto").to("Elog","demo").replyTo("last").send();
    entry2.print();
  
    
    std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"* 4) Now the last entry is :                        *"<<std::endl;
    std::cout<<"*****************************************************"<<std::endl;
    ElogEntry entry3= manager.createEntry();
    entry3.user("toto").to("Elog","demo").receiveEntry("last");
    entry3.print();
    
    std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"* 5) I can extract data from the new last entry !   *"<<std::endl;
    std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"His Author is : "<<entry.getAttribute("Author")<<std::endl;
    std::cout<<"And he said : "<<entry.getText()<<std::endl;
}
```

