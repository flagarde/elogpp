#include "Manager.hpp"
#include "User.hpp"
#include "Server.hpp"
#include <iostream>

using namespace cxx::elog;

int main()
{
    User user("me","1234567");
    Server server = Server().setHostname("127.0.0.1").setPort("4444").setSSL(true).setName("Elog").setDescription("Elog test server").setCACertificateFile("/home/works/elog/ssl/server.crt");
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
    entry4.addAttachment("/home/works/elogpp/build/toto.jpg");
    entry4.addAttachment("/home/works/elogpp/build/1.png");
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
