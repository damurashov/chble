#!/bin/bash

replaceinfiles.py '*' '#include "config.h"' '#include "config.h"'
replaceinfiles.py '*' '#include "wchble.h"' '#include "wchble.h"'

