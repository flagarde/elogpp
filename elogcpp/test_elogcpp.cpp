#include "ElogManager.h"
#include<iostream>

using namespace elogpp;

int main() 
{
    ElogManager manager;
    //manager.Print();
    //manager.PrintServer();
    //manager.PrintUser();
    //manager.SetDefaultUser("Moi");
    //for(unsigned i=0;i!=10;++i)
    //{
        ElogEntry entry= manager.CreateEntry();
  
    entry.SetAttribute("Type","Comments");
    //entry.SetAttribute("Subject","where are you? "+std::to_string(i));
        //entry.User("Machine").To("demo","Accelerator").Send();
    //Message& mess2= manager.CreateMessage();
    //mess2.SetAttribute("titi","teta");
    //mess2.SetMessage("Mais où est donc Ornicar");
    //entry.AddAttachment("jfjjf.txt");
        //entry.AddAttachment("ldldl.jpg");
    //entry.AddFileMessage("kk.txt");
    entry.SetMessage("Ça marche");
    //entry.Print();
   // mess2.Print();
   // std::cout<<"Send"<<std::endl;
    
    //std::cout<<"Edit"<<std::endl;
    //entry.User("Machine").To("demo","Accelerator").Edit(2).Send();
    //std::cout<<"Reply"<<std::endl;
    //entry.User("Machine").To("demo","Accelerator").ReplyTo(2).Send();
    //std::cout<<"Reply+Quote"<<std::endl;
    //entry.User("Machine").To("demo","Accelerator").Send();
    //std::cout<<"Download"<<std::endl;
    //entry.User("Machine").To("demo","Accelerator").ReceiveEntry("last");
    //entry.Print();
    //entry.SetMessage("I'm need to talk with you!");
    //if(entry.User("Machine").To("demo","Accelerator").Send("V-X0"))std::cout<<"WOOOOOW! ÇA MARCHE!"<<std::endl;
    //else std::cout<<"OH! OH!"<<std::endl;
    entry.User("lagarde").To("IPNL","RPC-H2").Send("V+");
    //}
   // entry.Print();
   // std::cout<<entry.GetAttribute("Category")<<std::endl;
}
