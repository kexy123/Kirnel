#include "master.h"
#include "structure.h"

TSSObject init_tss()
{
    TSSObject object = create_tss();
    load_tss(object);

    return object;
}
