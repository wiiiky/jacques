#include <jmod/jmod.h>
#include <stdio.h>

void init(void)
{
    j_mod_log(J_LOG_LEVEL_INFO,"hello world!");
}

JModule module_struct={
    "hello",
    init,
    NULL,
    NULL
};
