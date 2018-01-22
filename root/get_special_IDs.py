#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys

# bypassed. create output file to keep make happy.
file = open('special.tmp', 'a')
file.write('')
file.close()
sys.exit(0)
