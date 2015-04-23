#!/usr/bin/env python

import versioning
import sys

if __name__=='__main__':
    versioning.startbump("xmlpp.py " + " ".join(sys.argv[1:]))
    versioning.startbump("XMLConf/XMLConfVersion.h " + " ".join(sys.argv[1:]))
    sys.exit(0)
