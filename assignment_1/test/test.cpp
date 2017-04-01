#include <stdint.h>
#include <iostream>
#include <memory>
#include <boost/coroutine2/all.hpp>

typedef boost::coroutines2::coroutine<std::pair<uint16_t, uint16_t>> coro_t;

void pair_sequence(coro_t::push_type& yield)
{
    uint16_t i = 0;
    uint16_t j = 0;
    for (;;) {
        for (;;) {
            yield(std::make_pair(i, j));
            if (++j == 0)
                break;
        }
        if (++i == 0)
            break;
    }
}

int main()
{
    coro_t::pull_type seq(boost::coroutines2::fixedsize_stack(),
                          pair_sequence);
    for (auto pair : seq) {
        //std::cout << pair.first << "-" << pair.second <<std::endl;
    }
    //while (seq) {
    //    print_pair(seq.get());
    //    seq();
    //}
}