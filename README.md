# dns-cache


### Description
This class stores mapping between the name and IP address. The maximum number of records available for storage is initialized in the constructor with the _max_size_ parameter. The class is impelemented as a singleton and tested with the [Catch](https://github.com/philsquared/Catch) test framework.

### Behavior

#### General

Function call __update()__ either updates the existing record or adds a new one. The __resolve()__ method returns the IP address from cache for a given name parameter or an empty string if not found.

If the limit set by _max_size_ is exceeded, the oldest unused records should be deleted from cache. The oldest pair (name and IP-address) is the one which is not involved in __update()__ or __resolve()__ procedures longer than the others.

#### Performance

For sure this implementation can be optimized. Perhaps by avoiding sorting the whole vector after _max_size_ is exceeded just by adding updated/resolved record at the beginning of the vector so that the oldest ones remain at the end.

#### Multithreading

The class supports correct operation in a multithreaded application, when __update()__ and __resolve()__ are called from different threads at the same time.

### Complexity of operations:

Inserting new element - void DNSCache::add(const std::string& name, const std::string& ip):

    sort() O(n(log(n))
    insert() O(n)
    pop_back() O(1)

Updating - int DNSCache::modify(const std::string &name, const std::string &ip)

    searching (get_record_index()) O(log(n))
    modify O(1)

### Installation

#### Requirements

For simplicity, the class operates with a standard library only (it is good to consider to extend it with the [Boost](http://www.boost.org) library). To build on a linux system, the following dependencies
must be installed:

    g++, cmake, git

NOTE: __cmake__ version must be __3.6__ or __higher__.
#### Build

NOTE: For simplicity, the class is not going to be built as a shared _.so_ library. The purpose (for now) is just to show a demo with threads and tests.

Clone this repository:

    git clone https://github.com/tomesm/dns-cache.git

Then go inside the folder:

    cd dns-cache

Use cmake or higher. Ideally create a separate build folder:

    mkdir build
    cd build
    cmake ..

This will create a MAKEFILE inside the _build_ folder. Now you can just comile with:

    make

This command creates two binaries:

    demo dns-test

and one shared library:

    libDNSCache.so

### Usage

While still inside the _build_ directory, move the _hosts_ file with some sample input data (using _/etc/hosts_ format) into build folder:

    mv ../hosts hosts

The _demo_ file is a very dummy binary to show working with threads. Just run:

    ./demo

Be aware that the executable runs in an __endless loop__! You need to stop executing it with:

    ^C

To run tests just put:

    ./dns-test
