//
// Created by martin on 28/03/2017.
// TODO: add more error checks
//
#include <string>
#include <iostream>
#include <fstream>


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

void DNSCache::update(const std::string& name, const std::string& ip) throw()
{
    int index;

    try {
        index = modify(name, ip);
    } catch (std::exception& e) {
        std::cout << "Could not update(update) record. ERROR: " << e.what() << std::endl;
    }

    if (index == -1){
        try {
            add(name, ip);
        } catch (std::exception &e) {
            std::cout << "Could not update(add) record. ERROR: " << e.what() << std::endl;
            // TODO: change to cerr
        }
    }
}

int DNSCache::modify(const std::string &name, const std::string &ip)
{
    /* We need to start locking when finding index, because some other thread
     * could pop-out the element meanwhile and we need to prevent it
     * using a mutex wrapper std::lock_guard: When lock goes out of scope, its
     * destructor unlocks the underlying mutex m_mutex
     */
    std::lock_guard<std::mutex> lock(m_mutex);

    int index_ip = get_record_index(ip); // find index based on ip
    int index_name = get_record_index(name); // find index based on name
    int rval = -1;
    // either name or ip must be found to modify the record
    if (index_ip >= 0 || index_name >= 0) {
        if (index_ip >= 0 ) {
            m_records.at(index_ip).timestamp = set_time_stamp();
            m_records.at(index_ip).ip = ip;
            rval = index_ip;
        } else {
            m_records.at(index_name).timestamp = set_time_stamp();
            m_records.at(index_name).name = name;
            rval = index_name;
        }
    }
    return rval;
}

void DNSCache::add(const std::string& name, const std::string& ip)
{
    //
    if (m_records.size() >= m_max_size){
        sort(); // move the oldest record at the end of collection
        m_records.pop_back();
    }
    // always put new elems at the beginning
    auto it = m_records.begin();
    it = m_records.insert(it, Record(name, ip, set_time_stamp()));
    // TODO: check the return value
}

void DNSCache::sort()
{
    std::sort(m_records.begin(),m_records.end(),
         [](const Record &x, const Record &y) {
            return (x.timestamp > y.timestamp);
         }
    );
}

void DNSCache::delete_list() throw()
{
    m_records.clear();
}

void DNSCache::print_records() throw()
{
    std::vector<Record>::const_iterator iter;
    std::vector<Record>::const_iterator end;

    for (iter = m_records.begin(), end = m_records.end(); iter != end; ++iter) {
        std::cout << iter->ip.data();
        std::cout << " " << iter->name.data() << std::endl;
    }

    std::cout << std::endl;
}

std::string DNSCache::resolve(const std::string& name) throw()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string found;
    int index = get_record_index(name);

    if (index == -1) {
        return found;
    } else {
        m_records.at(index).timestamp = set_time_stamp();
        found = m_records.at(index).ip;
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

std::chrono::system_clock::time_point DNSCache::set_time_stamp()
{
    return std::chrono::system_clock::now();
}


