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

class WaitfreeReferenceCounter {
        std::atomic<ull> counter = 1;
        const ull zero_flag = (1ll << 63);
        const ull help_flag = (1ll << 62);
public:
        bool increment() {
                return ((counter.fetch_add(1) & zero_flag) == 0);
        }
        bool decrement() {
                if (counter.fetch_sub(1) == 1) {
                        ull expected = 0;
                        if (counter.compare_exchange_strong(expected, zero_flag)) {
                                return true;
                        } else if ((expected & help_flag) &&
                        (counter.exchange(zero_flag) & help_flag)) {
                                return true;
                        }
                }
                return false;
        }
        ull get_counter() {
                ull cur_counter = counter.load();
                if (cur_counter == 0 && counter.compare_exchange_strong(cur_counter, zero_flag | help_flag)) {
                        return 0;
                }
                if (cur_counter & zero_flag) {
                        return 0;
                }
                return cur_counter;
        }
};

class basic_string {
        size_t size;
        char  *c_str;
        WaitfreeReferenceCounter *counter;
public:
		basic_string(const char *other) {
			size = sizeof(other);
			c_str = new char[size];
			memcpy(c_str, other, size);
			counter = new WaitfreeReferenceCounter();
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
                        counter = new WaitfreeReferenceCounter();
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
	a[1] = 'q';
	std::cout << b.data() << std::endl;
	std::cout << a.data() << std::endl;
}
