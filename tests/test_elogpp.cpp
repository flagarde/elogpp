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
    entry.user("me").to("Elog","Test").receiveEntry("last");
    
    std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"* 2) Now creating a new entry                       *"<<std::endl; 
    std::cout<<"*****************************************************"<<std::endl;
    ElogEntry entry4= manager.createEntry();
    entry4.setAttribute("Type","Problem Fixed");
    entry4.setAttribute("Category","Other");
    entry4.setAttribute("Author","Boss");
    entry4.setMessage("Please say something !");
    entry4.user("me").to("Elog","Test").send("V");
    entry4.print();
    
    std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"* 3) Now replying to it                             *"<<std::endl;
    std::cout<<"*****************************************************"<<std::endl;
    ElogEntry entry2= manager.createEntry();
    entry2.setAttribute("Type","Routine");
    entry2.setAttribute("Author","Toto");
    entry2.setMessage("Something !");
    entry2.user("me").to("Elog","Test").replyTo("last").send();
    entry2.print();
  
    
    std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"* 4) Now the last entry is :                        *"<<std::endl;
    std::cout<<"*****************************************************"<<std::endl;
    ElogEntry entry3= manager.createEntry();
    entry3.user("me").to("Elog","Test").receiveEntry("last");
    entry3.print();
    
    std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"* 5) I can extract data from the new last entry !   *"<<std::endl;
    std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"His Author is : "<<entry.getAttribute("Author")<<std::endl;
    std::cout<<"And he said : "<<entry.getText()<<std::endl;
}
