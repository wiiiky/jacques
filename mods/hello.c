#include <jmod/jmod.h>
#include <stdio.h>

void init(void)
{
    printf("hello\n");
}

JModule module_struct={
    "hello",
    init
};
