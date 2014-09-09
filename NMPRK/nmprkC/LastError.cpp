#include "stdafx.h"

#include "LastError.h"

int lastError;

void SiSetLastError(int e) { lastError = e; }
