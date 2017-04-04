//
// Created by martin on 28/03/2017.
//

#ifndef DNS_DNSCACHE_H
#define DNS_DNSCACHE_H


#include <string>
#include <vector>
#include <mutex>
#include "../include/Exception.h"

class DNSCache
{
public:

    /**
     *  @brief CLass destructor
     */
    virtual ~DNSCache() { delete_list(); }

    /**
     * @brief Either updates an existing record or adds a new one. WIll use add.
     *
     * @param name
     * @param ip
     */
    void update(const std::string& name, const std::string& ip) throw ();

    /**
     * @brief Gets the IP address for the given name grom the cache.
     *
     * @param name
     * @return IP or empty string
     */
    std::string resolve(const std::string& name) throw();

    /**
     *  @brief all records from the list for DEBUG purposes only. TODO: move to protected for release
     */
    void print_records() throw ();

    /**
     * @brief Find record with lowest update rank using lambda function and move
     *        the record at the end of the collection so that the oldest record can be
     *        poped out when exceeding the max_limit. TODO: move to protected for release
     *
     * @returns void
     */
    void sort();

    /**
     * @brief Static access method to provide a singleton.
     *
     * @param max_size Maximum size of cache records.
     * @returns pointer to one and ony instance of this class
     */
    static DNSCache* get_instance(size_t max_size);

private:

    /**
     * @brief Private constructor to prevent instancing
     *
     * @param max_size Maximum size of cache records.
     */
    explicit DNSCache(size_t m_max_size);

    /**
     * @brief Pointer to an address where the only instance of this class will
     *        be stored as this is a singleton class.
     */
    static DNSCache* instance;

protected:

   /**
    * Struct to hold name and ip address pair
    *
    */
    struct Record
    {
        /**
         *  @brief Struct constructor.
         *
         *  @param name String holding the domain name.
         *  @param ip String holding the IP address in dot notation.
         */
        Record(const std::string& name, const std::string& ip, std::chrono::system_clock::time_point stamp) :
                ip(ip), name(name), timestamp(stamp)
        {
        }

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
     *  @brief  List of records.
     */
    std::vector<Record> m_records;

    // mutex to lock critical section
    //std::mutex m_lock;

    /**
     * @brief mutex
     */
    std::mutex m_mutex;

    /**
     *  Deletes all records from the list. Used in destructor
     */
    void delete_list() throw();

    /**
     * @brief Finds an index of a record based on given text
     *
     * @param text (ip or name)
     * @return index of record if found, -1 otherwise
     */
    int get_record_index(const std::string &text);

    /**
     * @brief Adds a new element into the vector container. If max_size limit is
     *        exceeded, the oldest unused record is removed.
     *
     * @param name
     * @param ip
     * @returns void TODO: add return value
     */
    void add(const std::string& name, const std::string& ip);

    /**
     * @brief Modifies either name or ip of the record
     *
     * @param name
     * @param ip
     * @return index of record or -1 if record not found
     */
    int modify(const std::string& name, const std::string& ip);

    /**
     * @brief Sets current time into timestamp of a record.
     *
     * @return current system timestamp
     */
    std::chrono::system_clock::time_point set_time_stamp();

};


#endif //DNS_DNSCACHE_H
