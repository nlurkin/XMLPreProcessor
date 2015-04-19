#!/usr/bin/env python

import xmlpp
import sys

if __name__=='__main__':
    currVersion = [int(e) for e in xmlpp.__version__.split(".")]

    print "Current versions %s.%s.%s" % tuple(currVersion)
    
    change = False
    if(len(sys.argv)>1):
        if sys.argv[1]=="major":
            currVersion[0] = currVersion[0]+1
            currVersion[1] = 0
            currVersion[2] = 0
            change = True
        if sys.argv[1]=="minor":
            currVersion[1] = currVersion[1]+1
            currVersion[2] = 0
            change = True
        if sys.argv[1]=="patch":
            currVersion[2] = currVersion[1]+1
            change = True
    else:
        print "Please specify which field to update: major, minor, patch"

    print "New version is %s.%s.%s" % tuple(currVersion)
    if change==False:
        sys.exit(0)


    
    data = None
    with open("xmlpp.py") as fd:
        data = fd.readlines()
        
        if data:
            for i, line in enumerate(data):
                if "__version__" in line:
                    data[i] = '__version__ = "%s.%s.%s\n' % tuple(currVersion)
            with open("xmlpp.py", 'w') as f:
                f.writelines(data)
            
    with open("XMLConf/XMLConfVersion.h") as fd:
        data = fd.readlines()
        
        if data:
            for i, line in enumerate(data):
                if "XMLCONF_VERSION" in line:
                    data[i] = '#define XMLCONF_VERSION %s.%s.%s\n' % tuple(currVersion)
            with open("XMLConf/XMLConfVersion.h", 'w') as f:
                f.writelines(data)
    