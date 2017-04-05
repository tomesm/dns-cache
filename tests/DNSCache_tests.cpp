/**
 * Tests for DNSCache project
 *
 * CATCH framework is used
 */

#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "../include/DNSCache.h"
#include "../include/Exception.h"

// using just for testing purposes
using namespace std;

// lets declare auxiliary functions. They are implemented at the end of this file.
void parse_line(string line, DNSCache *cache);
void init(const std::string& filename, DNSCache* cache) throw(dns::Exception);

// Start with testing using BDD approach

SCENARIO("DNS cache is initialized, can be accessed, records can be updated and resolved")
{
    GIVEN("DNS cache is created and allocated")
    {
        size_t max = 6;
        DNSCache* cache = DNSCache::get_instance(max);

        REQUIRE(cache->m_max_size == max);

        WHEN("Cache is initialized with some data")
        {
            CHECK_THROWS_AS(init("", cache), dns::Exception);
            REQUIRE_NOTHROW(init("hosts", cache));

            THEN("Cached domain names can be resolved")
            {
                REQUIRE(cache->resolve("localhost") == "127.0.0.1");
                REQUIRE(cache->resolve("www.google.com") ==
                        "2001:4860:4860:0:0:0:0:8888");
            }

            AND_THEN("Empty string si returned for not cached names")
            {
                // domains not in the cache
                REQUIRE(cache->resolve("www..com") == "");
                REQUIRE(cache->resolve("google.com") == "");
                REQUIRE(cache->resolve("") == "");
            }
        }

        WHEN("Record is updated with valid IP")
        {
            CHECK_NOTHROW(cache->update("www.google.com", "8.8.8.8"));
            CHECK_NOTHROW(cache->update("test.apple.com",
                                        "2001:db8:ffff:1:201:02ff:fe03:0405"));
            CHECK_NOTHROW(cache->update("martintomes.com", "10.0.0.2"));

            THEN("If record exists, only IP or a name is modified")
            {
                REQUIRE(cache->resolve("www.google.com") == "8.8.8.8");
                REQUIRE(cache->resolve("test.apple.com") ==
                                "2001:db8:ffff:1:201:02ff:fe03:0405");
            }

            AND_THEN("If this is a new record it is added")
            {
                REQUIRE(cache->resolve("martintomes.com") == "10.0.0.2");
            }
        }

        WHEN("Adding a new record and max limit is reached")
        {
            CHECK_NOTHROW(cache->update("maximum_reached.eu", "255.255.255.255"));

            THEN("The the oldest one must be removed")
            {
                REQUIRE(cache->resolve("www.example.com") == "");
            }
        }

        WHEN("Updating record with invalid IP format")
        {
            THEN("Current record will not be modified")
            {
                REQUIRE_THROWS_AS(cache->update("martintomes.com", ":::::"),
                                  dns::Exception);
            }

            AND_THEN("New record shall not pass")
            {
                REQUIRE_THROWS_AS(cache->update("yahoo.com", "redfgsdfhsh"),
                                dns::Exception);
            }
        }
    }
}

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
