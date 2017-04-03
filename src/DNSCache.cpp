//
// Created by martin on 28/03/2017.
// TODO: add more error checks
//
#include <string>
#include <iostream>
#include <fstream>
#include <mutex>

#include "../include/DNSCache.h"

//using namespace std;
using std::cout;
using std::endl;

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

    m_lock.lock();
    // we need to start locking when finding index, because some other thread
    // could pop-out the element menawhile and we need to prevent it
    int index_ip = find_elem(ip);
    int index_name = find_elem(name);
    int index;
    // either name or ip must be found to update the record
    if (index_ip >= 0 || index_name >= 0) {
        // update record
        // get index eiter based on IP or a domain name
        // TODO find better way to check
        if (index_ip > 0 )
            index = index_ip;
        else
            index = index_name;

        try {
            // add timestamp
            m_records.at(index).timestamp = set_time_stamp();
            m_records.at(index).name = name;
            m_records.at(index).ip = ip;
            // just sort after every update

        } catch (std::exception& e) {
            cout << "Could not update(update) record. ERROR: " << e.what() << endl;
        }
    } else {
        // add new record
        try {
            // if the limit is exceeded, the oldest unused record is removed
            // TODO: test
            if (m_records.size() >= m_max_size){
                sort(); // sort the oldest record before poping
                // only sorting when adding elem is enough
                m_records.pop_back();
            }
            // always put new elems at the beginning
            auto it = m_records.begin();
            it = m_records.insert(it, Record(name, ip, set_time_stamp()));
            // TODO: check the return value
        }
        catch (std::exception &e) {
            cout << "Could not update(add) record. ERROR: " << e.what() << endl;
        }
    }

    m_lock.unlock();
}


void DNSCache::sort()
{
    // find record with lowest update rank using lambda function and move the
    // at the end of the collection so that the oldest record can be poped out
    // when exceeding the max_limit
    // TODO: perhaps there is a better way than using timestamps
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
    //cout << "Reading records..." << endl;
    std::vector<Record>::const_iterator iter ;
    std::vector<Record>::const_iterator end;

    for (iter = m_records.begin(), end = m_records.end(); iter != end; ++iter) {
        std::cout << iter->ip.data();
        std::cout << " " << iter->name.data() << endl;
    }
    cout << endl;
}


std::string DNSCache::resolve(const std::string& name) throw()
{
    std::string found;
    // we will be searching struct with given name so we can use C++11 and
    // to capture the name as a lambda expression
    int index = find_elem(name);
    if (index == -1) {
        return found;
    } else {
        m_records.at(index).timestamp = set_time_stamp(); // increase operations tracking
        return m_records.at(index).ip;
    }
}


int DNSCache::find_elem(const std::string& text)
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


