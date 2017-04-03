//
// Created by martin on 29/03/2017.
//

#ifndef DNS_EXCEPTION_H
#define DNS_EXCEPTION_H

#include <exception>
#include <string>

namespace dns
{

/**
 *  Exception class extends standard exception funtionality and adds it the text
 *  message to inform about the reason of the exception thrown.
 */
    class Exception : public std::exception {
    public:
        /**
         *  Constructor
         *  @param text Information text to be filled with the reasons of the exception
         */
        Exception(std::string& text) : m_text(text) { }
        virtual ~Exception() throw() { }

        /**
         *  Returns the information text string
         *  @return The information text
         */
        const char* what() const throw() {

            return m_text.data();
        }

    private:
        std::string m_text;
    };
}

#endif //DNS_EXCEPTION_H
