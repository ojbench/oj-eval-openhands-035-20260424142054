#ifndef SIMPLE_STRING_SIMPLESTRING_HPP
#define SIMPLE_STRING_SIMPLESTRING_HPP

#include <stdexcept>
#include <cstring>
#include <algorithm>

class MyString {
private:
    union {
        char* heap_ptr;
        char small_buffer[16];
    };
    size_t length;
    size_t cap;
    
    static constexpr size_t SSO_CAPACITY = 15;
    
    bool is_sso() const {
        return cap == SSO_CAPACITY;
    }
    
    char* data_ptr() {
        return is_sso() ? small_buffer : heap_ptr;
    }
    
    const char* data_ptr() const {
        return is_sso() ? small_buffer : heap_ptr;
    }
    
public:
    MyString() : length(0), cap(SSO_CAPACITY) {
        small_buffer[0] = '\0';
    }
    
    MyString(const char* s) {
        if (!s) {
            length = 0;
            cap = SSO_CAPACITY;
            small_buffer[0] = '\0';
            return;
        }
        
        length = strlen(s);
        if (length <= SSO_CAPACITY) {
            cap = SSO_CAPACITY;
            std::memcpy(small_buffer, s, length + 1);
        } else {
            cap = length;
            heap_ptr = new char[cap + 1];
            std::memcpy(heap_ptr, s, length + 1);
        }
    }
    
    MyString(const MyString& other) : length(other.length), cap(other.cap) {
        if (is_sso()) {
            std::memcpy(small_buffer, other.small_buffer, length + 1);
        } else {
            heap_ptr = new char[cap + 1];
            std::memcpy(heap_ptr, other.heap_ptr, length + 1);
        }
    }
    
    MyString(MyString&& other) noexcept : length(other.length), cap(other.cap) {
        if (is_sso()) {
            std::memcpy(small_buffer, other.small_buffer, length + 1);
        } else {
            heap_ptr = other.heap_ptr;
        }
        other.length = 0;
        other.cap = SSO_CAPACITY;
        other.small_buffer[0] = '\0';
    }
    
    MyString& operator=(MyString&& other) noexcept {
        if (this != &other) {
            if (!is_sso()) {
                delete[] heap_ptr;
            }
            
            length = other.length;
            cap = other.cap;
            
            if (is_sso()) {
                std::memcpy(small_buffer, other.small_buffer, length + 1);
            } else {
                heap_ptr = other.heap_ptr;
            }
            
            other.length = 0;
            other.cap = SSO_CAPACITY;
            other.small_buffer[0] = '\0';
        }
        return *this;
    }
    
    MyString& operator=(const MyString& other) {
        if (this != &other) {
            if (!is_sso()) {
                delete[] heap_ptr;
            }
            
            length = other.length;
            cap = other.cap;
            
            if (is_sso()) {
                std::memcpy(small_buffer, other.small_buffer, length + 1);
            } else {
                heap_ptr = new char[cap + 1];
                std::memcpy(heap_ptr, other.heap_ptr, length + 1);
            }
        }
        return *this;
    }
    
    ~MyString() {
        if (!is_sso()) {
            delete[] heap_ptr;
        }
    }
    
    const char* c_str() const {
        return data_ptr();
    }
    
    size_t size() const {
        return length;
    }
    
    size_t capacity() const {
        return cap;
    }
    
    void reserve(size_t new_capacity) {
        if (new_capacity <= cap) {
            return;
        }
        
        if (new_capacity <= SSO_CAPACITY) {
            return;
        }
        
        char* new_ptr = new char[new_capacity + 1];
        std::memcpy(new_ptr, data_ptr(), length + 1);
        
        if (!is_sso()) {
            delete[] heap_ptr;
        }
        
        heap_ptr = new_ptr;
        cap = new_capacity;
    }
    
    void resize(size_t new_size) {
        if (new_size == length) {
            return;
        }
        
        if (new_size <= SSO_CAPACITY) {
            if (!is_sso()) {
                char temp[16];
                std::memcpy(temp, heap_ptr, std::min(length, new_size) + 1);
                delete[] heap_ptr;
                std::memcpy(small_buffer, temp, std::min(length, new_size));
                cap = SSO_CAPACITY;
            }
            if (new_size > length) {
                std::memset(small_buffer + length, '\0', new_size - length);
            }
            small_buffer[new_size] = '\0';
        } else {
            if (new_size > cap) {
                reserve(new_size);
            }
            if (new_size > length) {
                std::memset(data_ptr() + length, '\0', new_size - length);
            }
            data_ptr()[new_size] = '\0';
        }
        length = new_size;
    }
    
    char& operator[](size_t index) {
        return data_ptr()[index];
    }
    
    const char& operator[](size_t index) const {
        return data_ptr()[index];
    }
    
    MyString operator+(const MyString& rhs) const {
        MyString result;
        size_t new_len = length + rhs.length;
        
        if (new_len <= SSO_CAPACITY) {
            result.cap = SSO_CAPACITY;
            std::memcpy(result.small_buffer, data_ptr(), length);
            std::memcpy(result.small_buffer + length, rhs.data_ptr(), rhs.length);
            result.small_buffer[new_len] = '\0';
        } else {
            result.cap = new_len;
            result.heap_ptr = new char[result.cap + 1];
            std::memcpy(result.heap_ptr, data_ptr(), length);
            std::memcpy(result.heap_ptr + length, rhs.data_ptr(), rhs.length);
            result.heap_ptr[new_len] = '\0';
        }
        result.length = new_len;
        return result;
    }
    
    void append(const char* str) {
        if (!str) return;
        
        size_t str_len = strlen(str);
        size_t new_len = length + str_len;
        
        if (new_len <= SSO_CAPACITY) {
            std::memcpy(data_ptr() + length, str, str_len + 1);
        } else {
            if (new_len > cap) {
                size_t new_cap = std::max(new_len, cap * 2);
                reserve(new_cap);
            }
            std::memcpy(data_ptr() + length, str, str_len + 1);
        }
        length = new_len;
    }
    
    const char& at(size_t pos) const {
        if (pos >= length) {
            throw std::out_of_range("Index out of range");
        }
        return data_ptr()[pos];
    }
    
    char& at(size_t pos) {
        if (pos >= length) {
            throw std::out_of_range("Index out of range");
        }
        return data_ptr()[pos];
    }
    
    class const_iterator;
    
    class iterator {
    private:
        char* ptr;
        friend class MyString;
        friend class const_iterator;
        
        iterator(char* p) : ptr(p) {}
        
    public:
        iterator& operator++() {
            ++ptr;
            return *this;
        }
        
        iterator operator++(int) {
            iterator temp = *this;
            ++ptr;
            return temp;
        }
        
        iterator& operator--() {
            --ptr;
            return *this;
        }
        
        iterator operator--(int) {
            iterator temp = *this;
            --ptr;
            return temp;
        }
        
        char& operator*() const {
            return *ptr;
        }
        
        bool operator==(const iterator& other) const {
            return ptr == other.ptr;
        }
        
        bool operator!=(const iterator& other) const {
            return ptr != other.ptr;
        }
        
        bool operator==(const const_iterator& other) const {
            return ptr == other.ptr;
        }
        
        bool operator!=(const const_iterator& other) const {
            return ptr != other.ptr;
        }
    };
    
    class const_iterator {
    private:
        const char* ptr;
        friend class MyString;
        friend class iterator;
        
        const_iterator(const char* p) : ptr(p) {}
        
    public:
        const_iterator& operator++() {
            ++ptr;
            return *this;
        }
        
        const_iterator operator++(int) {
            const_iterator temp = *this;
            ++ptr;
            return temp;
        }
        
        const_iterator& operator--() {
            --ptr;
            return *this;
        }
        
        const_iterator operator--(int) {
            const_iterator temp = *this;
            --ptr;
            return temp;
        }
        
        const char& operator*() const {
            return *ptr;
        }
        
        bool operator==(const const_iterator& other) const {
            return ptr == other.ptr;
        }
        
        bool operator!=(const const_iterator& other) const {
            return ptr != other.ptr;
        }
        
        bool operator==(const iterator& other) const {
            return ptr == other.ptr;
        }
        
        bool operator!=(const iterator& other) const {
            return ptr != other.ptr;
        }
    };
    
public:
    iterator begin() {
        return iterator(data_ptr());
    }
    
    iterator end() {
        return iterator(data_ptr() + length);
    }
    
    const_iterator cbegin() const {
        return const_iterator(data_ptr());
    }
    
    const_iterator cend() const {
        return const_iterator(data_ptr() + length);
    }
};

#endif
