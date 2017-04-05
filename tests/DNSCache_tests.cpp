/**
 * Tests for DNSCache project
 *
 * CATCH framework is used
 */

#define CATCH_CONFIG_MAIN
#include "../include/catch.hpp"

#include "../include/DNSCache.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <thread>



using namespace std;

/**
 * @brief Parses a raw line from a file into ip and name
 *
 * @param line a raw line from hosts file
 * @param cache pointer to the DNS cache instance
 */
void parse_line(string line, DNSCache *cache)
{
    string::size_type addr_end_positon = line.find_first_of(" ");

    if (addr_end_positon == string::npos)
        return;

    string::size_type name_start_position = line.find_last_of(" ");

    if (name_start_position == string::npos)
        return;

    name_start_position += 1;
    string ip(line, 0, addr_end_positon);
    string name(line, name_start_position, line.length());

    cache->update(name, ip);
}

/**
 *  Open the hosts file and read it to stores the ipAddress-hostname pairs.
 *  @param filename Name of the file containing the pairs.
 */
void init(const std::string& filename, DNSCache* cache) throw(dns::Exception)
{
    std::ifstream file(filename.data());

    if (!file) {
        std::string text("Could not open file: ");
        text += filename;
        dns::Exception e(text);
        throw(e);
    }

    std::string line;

    while (!file.eof()) {
        std::getline(file, line);
        parse_line(line, cache);
    }

    file.close();
}



TEST_CASE("DNS records properly stored")
{
    DNSCache* cache = DNSCache::get_instance(10);

    init("hosts", cache);

    std::string output;

    SECTION("resolve method")
    {
        cout << "Resolving..." << endl;
        cout << cache->resolve("localhost") << endl;
        cout << cache->resolve("www.ericsson.es") << endl;
        cout << cache->resolve("www.google.com") << endl;
        cout << cache->resolve("www..com") << endl;
        cout << cache->resolve("google.com") << endl;
        cout << cache->resolve("dfeqwef") << endl;
        cout << cache->resolve("") << endl;

    }

}
