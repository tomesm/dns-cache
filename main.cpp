/**
 * This is a very dummy demo to show the functionality of the DNSCache class.
 *
 * @author Martin Tomes
 *
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <thread>


#include "include/DNSCache.h"

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

void multi_update(DNSCache* cache, int id)
{
    while (1) {
        cout << id << ": Resolving...microsoft" << endl;
        cout << cache->resolve("www.microsoft.com") << endl;
        cout << id << ": adding tomes.io" << endl;
        cache->update("tomes.io", "1.1.1.1");
        cout << id << ": Resolving...tomes" << endl;
        cout << cache->resolve("tomes.io") << endl;
    }
}

int main(int argc, char* argv[])
{
    DNSCache* Cache = DNSCache::get_instance(5);

    init(argv[1], Cache);

    thread t1(multi_update, Cache, 1);
    thread t2(multi_update, Cache, 2);
    thread t3(multi_update, Cache, 3);
    thread t4(multi_update, Cache, 4);
    thread t5(multi_update, Cache, 5);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    delete Cache;

    return 0;
}
