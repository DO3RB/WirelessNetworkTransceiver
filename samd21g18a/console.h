#pragma once

#ifndef CONSOLE_H
#define CONSOLE_H

typedef enum : unsigned { RUN, ARG, DAT } console_cmd_t;

typedef struct { char *str; console_cmd_t typ; void(*ptr)(); unsigned dat; } console_t;

#define CONSOLE_CMD(N,T,P,D) console_t _console_##N [[gnu::used,gnu::section(".console")]] = {#N,T,(void(*)())&P,D};

#define CONSOLE_RUN(N,P)   CONSOLE_CMD(N,RUN,P,0)
#define CONSOLE_ARG(N,P)   CONSOLE_CMD(N,ARG,P,0)
#define CONSOLE_DAT(N,P,D) CONSOLE_CMD(N,DAT,P,D)

void console_loop(void);

#endif // CONSOLE_H
