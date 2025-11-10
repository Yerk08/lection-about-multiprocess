#include <iostream>
#include <cstdlib>
#include <cstring>
#include <atomic>

using ull = unsigned long long;
class ReferenceCounter {
        std::atomic<ull> counter = 1;
public:
        bool increment() {
                ull cur_counter = counter.load();
                while (cur_counter != 0 &&
        !counter.compare_exchange_weak(cur_counter, cur_counter + 1)) {
                        cur_counter = counter.load();
                }
                return cur_counter != 0;
        }
        bool decrement() {
                return counter.fetch_sub(1) == 1;
        }
        ull get_counter() {
                return counter.load();
        }
};

class basic_string {
        size_t size;
        char  *c_str;
        ReferenceCounter *counter;
public:
		basic_string(const char *other) {
			size = sizeof(other);
			c_str = new char[size];
			memcpy(c_str, other, size);
			counter = new ReferenceCounter();
		}
		size_t get_size() {
			return size - 1;
		}
		char* data() {
			return c_str;
		}
        basic_string(const basic_string &other) {
                if (!other.counter->increment())
                    throw std::invalid_argument("");
                size = other.size;
                c_str = other.c_str;
                counter = other.counter;
        }
        char& operator[](size_t index) {
                if (counter->get_counter() > 1) {
                        char *new_c_str = new char[size];
                        memcpy(new_c_str, c_str, size);
                        if (counter->decrement()) {
							delete c_str;
                        }
                        c_str = new_c_str;
                        counter = new ReferenceCounter();
                }
                return c_str[index];
        }
        ~basic_string() {
                if (counter->decrement()) {
                        delete c_str;
                        delete counter;
                }
        }
};

int main() {
	basic_string a = "asd";
	basic_string b = a;
	//a[1] = 'q';
	std::cout << b.data() << std::endl;
	std::cout << a.data() << std::endl;
}
