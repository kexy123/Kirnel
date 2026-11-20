#include "master.h"
#include "structure.h"
#include "vga/out.h"

TSSObject init_tss()
{
    TSSObject object = create_tss();
    print_ln(object.Selector);
    print_uintx((unsigned long)object.TSS);
    print_newl();

    load_tss(object);

    return object;
}
