#ifndef FUNCTION_H
#define FUNCTION_H

#include <memory>

#include "badfunctionexception.h"


template <typename>
class function;

template<typename R, typename... Args>
class function<R (Args...)> {
public:
    function(std::nullptr_t) noexcept : small_object(false), big_pointer(nullptr) {}

    function() noexcept : function(nullptr) {}

    function(const function& other) {
        if (other.small_object) {
            small_object = true;
            std::copy(std::begin(other.buffer), std::end(other.buffer), buffer);
        } else {
            small_object = false;
            big_pointer = other.big_pointer->clone();
        }
    }

    function(function&& other) noexcept : function() {
        swap(other);
    }

    template <typename F>
    function(F f) {
        if constexpr (sizeof(f) > MAX_SIZE) {
            small_object = false;
            new (buffer) std::unique_ptr<wrapper_function<F>>(
                        std::make_unique<wrapper_function<F>>(std::move(f)));
        } else {
            small_object = true;
            new (buffer) wrapper_function<F>(std::move(f));
        }
    }

    template <typename F, typename Class>
    function(F Class::* member) {
        if constexpr (sizeof(member) > MAX_SIZE) {
            small_object = false;
            new (buffer) std::unique_ptr<wrapper_member<F, Class>>(
                        std::make_unique<wrapper_member<F, Class>>(std::move(member)));
        } else {
            small_object = true;
            new (buffer) wrapper_member<F, Class>(member);
        }
    }

    ~function() {
        if (!small_object) {
            big_pointer.~unique_ptr();
        }
    }

    function& operator=(const function& other) {
        function tmp(other);
        swap(tmp);
        return *this;
    }

    function& operator=(function&& other) noexcept {
        swap(other);
        return *this;
    }

    void swap(function& other) noexcept {
        std::swap(small_object, other.small_object);
        std::swap(buffer, other.buffer);
    }

    explicit operator bool() const noexcept {
        return small_object || static_cast<bool>(big_pointer);
    }

    R operator()(Args... args) const {
        if (!static_cast<bool>(*this)) {
            throw bad_function_call();
        }
        return small_object ? ((wrapper*)(buffer))->call(std::forward<Args>(args)...) :
                                       big_pointer->call(std::forward<Args>(args)...);
    }

private:
    class wrapper {
    public:
        wrapper() {}

        virtual ~wrapper() {}

        virtual R call(Args&&...) = 0;

        virtual std::unique_ptr<wrapper> clone() const = 0;
    };

    template <typename T>
    class wrapper_function : public wrapper {
    public:
        wrapper_function(const T& callable) : wrapper(), callable(callable) {}

        wrapper_function(T&& callable) : wrapper(), callable(std::move(callable)) {}

        ~wrapper_function() {}

        R call(Args&&... args) {
            return callable(std::forward<Args>(args)...);
        }

        std::unique_ptr<wrapper> clone() const {
            return std::make_unique<wrapper_function<T>>(callable);
        }

    private:
        T callable;
    };

    template <typename F, typename Class, typename... FunArgs>
    class wrapper_member : public wrapper {
    public:
        using member = F Class::*;

        wrapper_member(member callable) : wrapper(), callable(std::move(callable)) {}

        ~wrapper_member() {}

        R call(Class&& object, FunArgs&&... fun_args) {
            return (object.*callable)(std::forward<FunArgs>(fun_args)...);
        }

        std::unique_ptr<wrapper> clone() const {
            return std::make_unique<wrapper_member>(callable);
        }

    private:
        member callable;
    };

    static constexpr int MAX_SIZE = 64;
    bool small_object;

    union {
        std::unique_ptr<wrapper> big_pointer;
        char buffer[MAX_SIZE];
    };
};

#endif // FUNCTION_H
