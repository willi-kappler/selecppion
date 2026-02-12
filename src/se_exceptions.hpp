/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines all the exceptions that are used in this code base.
*/

#ifndef FILE_SE_EXCEPTIONS_HPP_INCLUDED
#define FILE_SE_EXCEPTIONS_HPP_INCLUDED

#include <stdexcept>

namespace Selecppion {
/*
class NCInvalidKeyException: public std::invalid_argument {
public:
  NCInvalidKeyException(): std::invalid_argument("Size of secret key must be 32 bytes.") { }
};

class NCCompressionException: public std::runtime_error {
public:
  NCCompressionException(): std::runtime_error("Compression error.") { }
};

class NCDecompressionException: public std::runtime_error {
public:
  NCDecompressionException(): std::runtime_error("Decompression error.") { }
};

class NCEncryptionException: public std::runtime_error {
public:
  NCEncryptionException(const char *msg): std::runtime_error(msg) { }
};

class NCDecryptionException: public std::runtime_error {
public:
  NCDecryptionException(const char *msg): std::runtime_error(msg) { }
};
*/
class SEConfigurationException: public std::runtime_error {
public:
  SEConfigurationException(const char *msg): std::runtime_error(msg) { }
};

}

#endif // FILE_SE_EXCEPTIONS_HPP_INCLUDED
