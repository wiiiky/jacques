#include <jmod/jmod.h>
#include <stdio.h>

void init(void)
{
    j_mod_log(J_LOG_LEVEL_INFO,"hello world!");
}

void config_init(void)
{
    j_mod_log(J_LOG_LEVEL_WARNING,"config init");
}

void config_summary(void)
{
    j_mod_log(J_LOG_LEVEL_DEBUG,"config summary");
}

void config_load(const char *scope,JList *sargs,const char *directive,JList *dargs)
{
    j_mod_log(J_LOG_LEVEL_WARNING,"%s/%s",scope?scope:"NULL",directive);
}

static char *directives[]={"Hello","World",NULL};

JModuleConfigHandler config_handler={
    directives,
    NULL,
    config_init,
    config_load,
    config_summary
};

JModule module_struct={
    "hello",
    &config_handler,
    init,
    NULL
};
