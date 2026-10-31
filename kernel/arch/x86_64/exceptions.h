
#ifndef CHIQUIOS_X64_EXCEPTIONS_H
#define CHIQUIOS_X64_EXCEPTIONS_H

#include "../../core/types.h"

void exceptions_initialize(void);
void exception_handler(u64 vector, u64 error_code);

#endif
