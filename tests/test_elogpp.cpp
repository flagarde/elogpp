#include "ElogManager.hpp"
#include<iostream>

using namespace elogpp;

int main() 
{
    ElogManager manager;
    //manager.Print();
    //manager.PrintServer();
    //manager.PrintUser();
    //manager.SetDefaultUser("Moi");
    ElogEntry entry= manager.CreateEntry();
    std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"* 1) Last entry is :                                *"<<std::endl;
    std::cout<<"*****************************************************"<<std::endl;
    entry.User("DAQ").To("NAS","test").ReceiveEntry("last");
    entry.Print();
    
    /*std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"* 2) Now creating a new entry                       *"<<std::endl; 
    std::cout<<"*****************************************************"<<std::endl;
    ElogEntry entry4= manager.CreateEntry();
    entry4.SetAttribute("Type","Other");
    entry4.SetAttribute("Author","Boss");
    //entry.AddAttachment("MyPicture.jpg");
    entry4.SetMessage("Please say something !");
    entry4.User("admin").To("Server1","demo").Send("V");
    entry4.Print();
    
    std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"* 3) Now replying to it                             *"<<std::endl;
    std::cout<<"*****************************************************"<<std::endl;
    ElogEntry entry2= manager.CreateEntry();
    entry2.SetAttribute("Type","Routine");
    entry2.SetAttribute("Author","Toto");
    entry2.SetMessage("Something !");
    entry2.User("admin").To("Server1","demo").ReplyTo("last").Send();
    entry2.Print();
    
    
    std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"* 4) Now the last entry is :                        *"<<std::endl;
    std::cout<<"*****************************************************"<<std::endl;
    ElogEntry entry3= manager.CreateEntry();
    entry3.User("admin").To("Server1","demo").ReceiveEntry("last");
    entry3.Print();
    
    std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"* 5) I can extract data from the new last entry !   *"<<std::endl;
    std::cout<<"*****************************************************"<<std::endl;
    std::cout<<"His Author is : "<<entry.GetAttribute("Author")<<std::endl;
    std::cout<<"And this idiot said : "<<entry.GetTextMessage()<<std::endl;*/
}
