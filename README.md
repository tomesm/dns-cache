# dns-cache
Class for a DNS cache

This class stores mapping between the name and IP address. The maximum number of records available for storage is initialized in the constructor with the max_size parameter.

Function call update() either updates the existing record or adds a new one.

The resolve() method returns the IP address from cache for a given name parameter or an empty string if not found.

If the limit set by max_size is exceeded, the oldest unused records should be deleted from cache. The oldest pair (name and IP-address) is the one which is not involved in update() or resolve() procedures longer than the others.

The class supports correct operation in a multithreaded application, when update() and resolve() are called from different threads at the same time.

COMPLEXITY of operations:

Inserting new element - void DNSCache::add(const std::string& name, const std::string& ip):

    sort() O(n(log(n))
    insert() O(n)
    pop_back() O(1)

Updating - int DNSCache::modify(const std::string &name, const std::string &ip)

    Finding - get_record_index() O(log(n))



