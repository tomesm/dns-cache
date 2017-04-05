//
// Created by martin on 28/03/2017.
//
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <arpa/inet.h>

#include "../include/DNSCache.h"

/* Null, because instance will be initialized on demand. */
DNSCache* DNSCache::instance = NULL;

// Only one instance can be created. Everytime the same address in the memory is returned.
DNSCache* DNSCache::get_instance(size_t max_size)
{
    if (instance == NULL) {
        instance = new DNSCache(max_size);
    }

    return instance;
}

DNSCache::DNSCache(size_t max_size): m_max_size(max_size)
{
}

void DNSCache::update(const std::string& name, const std::string& ip) throw(dns::Exception)
{
    int index = 0;

    try {
        index = modify(name, ip);
    } catch (dns::Exception& e) {
        std::cerr << "Could not update(update) record. ERROR: " << e.what() << std::endl;
        throw(e);
    }

    if (index == -1){
        try {
            add(name, ip);
        } catch (dns::Exception& e) {
            std::cerr << "Could not update(add) record. ERROR: " << e.what() << std::endl;
            throw(e);
        }
    }
}

int DNSCache::modify(const std::string &name, const std::string &ip) throw(dns::Exception)
{
    /**
     * Using a mutex wrapper std::lock_guard: When lock goes out of scope, its
     * destructor unlocks the underlying mutex m_mutex
     */
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!is_valid_ip(ip)) {
        std::string text("Invalid IP: ");
        text += ip;
        dns::Exception e(text);
        throw(e);
    }

    int index_ip = get_record_index(ip); // find index based on ip
    int index_name = get_record_index(name); // find index based on name
    int rval = -1;
    // either name or ip must be found to modify the record
    if (index_ip >= 0 || index_name >= 0) {
        if (index_ip >= 0 ) {
            // addres found, chane name
            m_records.at(index_ip).timestamp = set_time_stamp();
            m_records.at(index_ip).name = name;
            rval = index_ip;
        } else {
            // name found, change ip
            m_records.at(index_name).timestamp = set_time_stamp();
            m_records.at(index_name).ip = ip;
            rval = index_name;
        }
    }
    return rval;
}

void DNSCache::add(const std::string& name, const std::string& ip) throw(dns::Exception)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!is_valid_ip(ip)) {
        std::string text("Ivalid IP: ");
        text += ip;
        dns::Exception e(text);
        throw(e);
    }

    if (m_records.size() >= m_max_size){
        sort(); // find the oldest record at move it in the end of collection
        m_records.pop_back();
    }
    // always put new elems at the beginning
    auto it = m_records.begin();
    it = m_records.insert(it, Record(name, ip, set_time_stamp()));
}

void DNSCache::sort()
{
    std::sort(m_records.begin(),m_records.end(),
         [](const Record &x, const Record &y) {
            return (x.timestamp > y.timestamp);
         }
    );
}

void DNSCache::delete_list()
{
    m_records.clear();
}

std::string DNSCache::resolve(const std::string& name) throw(dns::Exception)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string found;
    int index = get_record_index(name);

    if (index == -1) {
        return found;
    } else {
        try {
            m_records.at(index).timestamp = set_time_stamp();
            found = m_records.at(index).ip;
        } catch (dns::Exception& e) {
            std::cerr << "Could not resolve a record. ERROR: " << e.what() << std::endl;
            throw(e);
        }
        return found;
    }
}

int DNSCache::get_record_index(const std::string &text)
{
    // we will be searching struct with given name or ip so we can use C++11 and
    // to capture the name as a lambda expression
    auto predicate = [text](const Record& item) {
        return item.name == text || item.ip == text;
    };

    auto it = std::find_if(m_records.begin(), m_records.end(), predicate);
    // if found, return index of eleent, otherwise -1
    if (it == m_records.end())
        return -1;
    else
        return (int) std::distance(m_records.begin(), it);
}

bool DNSCache::is_valid_ip(const std::string &ip)
{
    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;

    bool is_ipv4 = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr))!=0;
    bool is_ipv6 = inet_pton(AF_INET6, ip.c_str(), &(sa6.sin6_addr))!=0;

    return is_ipv4 || is_ipv6;
}

std::chrono::system_clock::time_point DNSCache::set_time_stamp()
{
    return std::chrono::system_clock::now();
}
