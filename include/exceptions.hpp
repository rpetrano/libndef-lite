#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

/// NDEF Record creation exception
class NDEFException : public std::exception {
public:
  // Constructor/Destructor
  NDEFException(std::string exMsg) : msg(exMsg) {}
  ~NDEFException() throw() {}

  // Exception implementation
  virtual const char* what() const throw() { return this->msg.c_str(); }

private:
  std::string msg;
};

#endif // EXCEPTIONS_H