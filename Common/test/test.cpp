#include <cassert>


int main()
{
    extern int do_hash_db_test();
    extern int do_bitops_test();

    assert(do_hash_db_test() == 0);
    assert(do_bitops_test() == 0);

    return 0;
}
