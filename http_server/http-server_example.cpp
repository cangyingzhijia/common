#include <iostream>
#include <pthread.h>
#include "common/http_server/http-server.h"

using namespace std;

string Map2String(map<string, string> smap) {
    string result;
    for(map<string, string>::iterator mit = smap.begin(); 
            mit != smap.end(); ++mit) {
        result += "  ";
        result += mit->first + ":";
        result += mit->second + "\n";
    }
    return result;
}

class AbcHandler : public common::HttpHandler {
public:
    void Run(common::HttpRequest *request, common::HttpResponse *response, Closure *done) {
        cout << "GetRequestURI:" << request->GetRequestURI() << endl;
        cout <<"GetQueryString:" << request->GetQueryString() << endl;
        cout << "GetRemoteHost:" << request->GetRemoteHost() << endl;
        cout << "Parameters:" << Map2String(request->GetParameterMap()) << endl;
        cout << "Headers:" << Map2String(request->GetHeaderMap()) << endl;
        cout << "Cookies:" << Map2String(request->GetCookieMap()) << endl;
        cout << "thread:" << pthread_self() << endl;
        response->SetResponse("abc");
        response->AddHeader("222222", "a=b;c=6;");
        done->Run();
    }
};

class EchoHandler: public common::HttpHandler {
public:
    void Run(common::HttpRequest *request, common::HttpResponse *response, Closure *done) {
        string echo = request->GetQueryString();
        response->SetResponse(echo);
        done->Run();
    }
};

class AsyncEchoHandler: public common::HttpHandler {
public:
    void DoProcess(common::HttpRequest *request, common::HttpResponse *response, Closure *done) {
        string echo = request->GetQueryString();
        response->SetResponse(echo);
        done->Run();
    }

    void Run(common::HttpRequest *request, common::HttpResponse *response, Closure *done) {
        common::HttpServer * owner_server = request->GetHttpServer();
        //DoProcess(request, response, done);
        Closure * async_echo_closure = NewClosure(this, &AsyncEchoHandler::DoProcess, request, response, done);
        owner_server->GetThreadPool()->Submit(async_echo_closure);
    }
};

int main() {
    common::HttpServer * server = common::NewEvhtpHttpServer("ipv4:0.0.0.0:12345", 0);
    server->RegisterHandler("/abc", new AbcHandler());
    server->RegisterHandler("/air/t", new AbcHandler());
    server->RegisterHandler("/", new AbcHandler());
    server->RegisterHandler("/arrival", new AbcHandler());
    server->RegisterHandler("/echo", new EchoHandler());
    server->RegisterHandler("/async_echo", new AsyncEchoHandler());
    server->Run();
    return 0;
}
