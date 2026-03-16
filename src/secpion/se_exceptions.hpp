/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines all the exceptions that are used in this code base.
*/

#ifndef FILE_SE_EXCEPTIONS_HPP_INCLUDED
#define FILE_SE_EXCEPTIONS_HPP_INCLUDED

#include <stdexcept>

namespace secpion {
class SEConfigurationException: public std::runtime_error {
public:
  SEConfigurationException(const char *msg): std::runtime_error(msg) { }
};

class SEIndividualException: public std::runtime_error {
public:
  SEIndividualException(const char *msg): std::runtime_error(msg) { }
};

class SERNGException: public std::runtime_error {
public:
  SERNGException(const char *msg): std::runtime_error(msg) { }
};

class SEServerIOException: public std::runtime_error {
public:
  SEServerIOException(const char *msg): std::runtime_error(msg) { }
};

class SEPopulationException: public std::runtime_error {
public:
  SEPopulationException(const char *msg): std::runtime_error(msg) { }
};
}

#endif // FILE_SE_EXCEPTIONS_HPP_INCLUDED
