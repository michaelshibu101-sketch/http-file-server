#include <winsock2.h>
#include <ws2tcpip.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

namespace fs = std::filesystem;

const int PORT = 8080;
const std::string PUBLIC_DIR = "public";
const std::string DOCS_DIR = "docs";

std::vector<std::string> docs = {
    "machine_learning.txt",
    "distributed_systems.txt",
    "databases.txt"
};

struct Result
{
    std::string file;
    int score;
};

std::vector<Result> search_documents(const std::string& query)
{
    std::vector<Result> results;

    for(const auto& file : docs)
    {
        std::string path = DOCS_DIR + "/" + file;

        std::ifstream f(path);

        if(!f)
            continue;

        std::stringstream buffer;
        buffer << f.rdbuf();

        std::string text = buffer.str();

        int count = 0;
        size_t pos = text.find(query);

        while(pos != std::string::npos)
        {
            count++;
            pos = text.find(query, pos + query.length());
        }

        if(count > 0)
        {
            results.push_back({file, count});
        }
    }

    std::sort(results.begin(), results.end(),
        [](const Result& a, const Result& b)
        {
            return a.score > b.score;
        });

    return results;
}

std::string build_results_page(const std::string& query)
{
    auto results = search_documents(query);

    std::stringstream html;

    html << "<html><body style='font-family:Arial;background:#0f172a;color:white;padding:40px;'>";

    html << "<h1>Results for: " << query << "</h1>";

    if(results.empty())
    {
        html << "<p>No results found.</p>";
    }
    else
    {
        html << "<ul>";

        for(auto& r : results)
        {
            html << "<li>"
                 << r.file
                 << " (score: "
                 << r.score
                 << ")</li>";
        }

        html << "</ul>";
    }

    html << "<br><a href='/'>Back</a>";

    html << "</body></html>";

    return html.str();
}

std::string read_file(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);

    std::stringstream buffer;

    buffer << file.rdbuf();

    return buffer.str();
}

void send_response(SOCKET client,const std::string& body)
{
    std::stringstream response;

    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: " << body.size() << "\r\n";
    response << "Connection: close\r\n\r\n";
    response << body;

    std::string res = response.str();

    send(client, res.c_str(), res.size(), 0);
}

void handle_client(SOCKET client)
{
    char buffer[4096];

    int bytes = recv(client, buffer, sizeof(buffer)-1, 0);

    if(bytes <= 0)
    {
        closesocket(client);
        return;
    }

    buffer[bytes] = '\0';

    std::string request(buffer);

    std::string method, path;

    std::stringstream ss(request);

    ss >> method >> path;

    if(path == "/")
    {
        std::string body = read_file(PUBLIC_DIR + "/index.html");
        send_response(client, body);
    }
    else if(path.find("/search?q=") == 0)
    {
        std::string query = path.substr(10);

        std::string body = build_results_page(query);

        send_response(client, body);
    }
    else
    {
        send_response(client, "<h1>404 Not Found</h1>");
    }

    closesocket(client);
}

int main()
{
    WSADATA wsa;

    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr));

    listen(server_fd, SOMAXCONN);

    std::cout << "Server running on http://127.0.0.1:" << PORT << std::endl;

    while(true)
    {
        sockaddr_in client_addr{};
        int len = sizeof(client_addr);

        SOCKET client = accept(server_fd,(sockaddr*)&client_addr,&len);

        std::thread(handle_client, client).detach();
    }

    return 0;
}