//
// Created by martin on 28/03/2017.
//

#ifndef DNS_DNSCACHE_H
#define DNS_DNSCACHE_H


#include <string>
#include <vector>
#include "../include/Exception.h"

class DNSCache
{
public:


    /**
     *  Destructor
     */
    virtual ~DNSCache() { delete_list(); }

    /**
     * Either updates an existing record or adds a new one. WIll use add.
     * @param name
     * @param ip
     */
    void update(const std::string& name, const std::string& ip) throw ();

    /**
     * Returns from cache the IP address for the given name. Will use convert() and find()
     * @param name
     * @return IP or empty string
     */
    std::string resolve(const std::string& name) throw();


    /**
     *  Prints all records from the list.
     */
    void print_records() throw ();

    void sort();

    /* Static access method. */
    static DNSCache* get_instance(size_t max_size);


private:
    /* Here will be the instance stored. */
    static DNSCache* instance;
    // private constructor to prevent instancing
    explicit DNSCache(size_t m_max_size);


protected:

    /**
    * Struct to hold name and ip address pair
    *
    */
    struct Record
    {
        /**
         *  Struct constructor.
         *  @param name String holding the domain name.
         *  @param ip String holding the IP address in dot notation.
         */
        Record(const std::string& name, const std::string& ip, std::chrono::system_clock::time_point stamp) : ip(ip),
               name(name), timestamp(stamp) { }

       /**
        *  Constructor
        */
       Record(){ }

       /**
         *  IP address in dot notation.
         */
        std::string ip;

        /**
         *  Domain name.
         */
        std::string name;

        /**
         *  Timestamp to determine old records
         */
        std::chrono::system_clock::time_point timestamp;
   };

    size_t m_max_size;

   /**
     *  Pointer to the start of the list of records.
     */
    std::vector<Record> m_records;

    // mutex to lock critical section
    std::mutex m_lock;


    /**
    *  Deletes all records from the list. Used in destructor
    */
    void delete_list() throw();

    // finds element based on given text
    int find_elem(const std::string& text);



    std::chrono::system_clock::time_point set_time_stamp();

};


#endif //DNS_DNSCACHE_H
