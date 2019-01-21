#ifndef BAD_FUNCTION_EXCEPTION_H
#define BAD_FUNCTION_EXCEPTION_H

#include <exception>


class bad_function_call : public std::exception {
public:
    const char* what() const noexcept {
        return "incorrect function call";
    }
};

#endif // BAD_FUNCTION_EXCEPTION_H
