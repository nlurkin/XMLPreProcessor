#!/usr/bin/python

'''
Created on 30 Mar 2015

@author: Nicolas
'''

from XMLPreprocessor import StructParse 
import sys
import os
from subprocess import call
from argparse import ArgumentParser, RawDescriptionHelpFormatter
from __builtin__ import bool

__version__ = "1.1.0"
__descr__ = ("""
Script for XML proxy
""")

def preProcessFile(filePath):
    # Verify the file exist and is a file
    if not os.path.isfile(filePath):
        print "Error: " + filePath + " is not a file or cannot be found"
        sys.exit(-1)
    
    # Extract file name (and without extension), directory name
    fileName = os.path.basename(filePath)
    baseName = os.path.splitext(fileName)[0]
    dirName = os.path.dirname(filePath)
    if not os.path.isabs(dirName):
        dirName = dirName + os.getcwd()
        
        preProcessedFile = dirName + "/" + baseName + ".pre"
        
    #Run the C preprocessor on the header file and produce a preprocessed file in the same folder
    cmd = ["gcc", "-x", "c++", "-E", filePath, "-o", preProcessedFile, "-I"+os.path.dirname(__file__)+"/XMLConf", "-DXMLPREPROCESSOR"]
    print "C preprocessor command: " + " ".join(cmd)
    call(cmd)
    
    return (preProcessedFile, dirName, baseName)

def prepareProxy(args):	
    '''
    Create the proxy functions for the structure given in the header file
    '''
    filePath = args.file
    topStruct = args.struct
    
    # Use the C preprocessor to preprocess the file
    # This allows to get rid of comments and replace known defines
    (preProcessedFile, dirName, baseName) = preProcessFile(filePath) 
    
    # Parse the file
    # Get in return a dictionary of all the structures found (correctly understood)
    # and a dictionary of typedef giving the corresponding structure name
    (struct, tdef) = StructParse.parse(preProcessedFile)
    
    if len(struct)==1: # Only a single structure was found in the file: use it as top structure
        StructParse.writeCFile(struct.values()[0], struct, tdef, dirName, baseName)
    elif not topStruct is None: # More than 1 structure found and the top structure was specified
        if topStruct in struct: # Specified top struct is found in struct dictionary
            StructParse.writeCFile(struct[topStruct], struct, tdef, dirName, baseName)
        elif topStruct in tdef: # Specified top struct is found in typedef dictionary 
            StructParse.writeCFile(struct[tdef[topStruct]], struct, tdef, dirName, baseName)
        else: # specified top structure not found - Don't know what to do: abort
            print "The specified top structure (" + topStruct + ") is not found"
    else: # No top structure specified - Don't know what to do: abort
        print "Error: More than 1 struct in file. Please specify which is the top one."
    
    os.remove(preProcessedFile)

def test(args):
    '''
    Test the parsing of the header file.
    It should print something equivalent to the input header file
    '''
    filePath = args.file
    (preProcessedFile, dirName, baseName) = preProcessFile(filePath)
    
    (struct, tdef) = StructParse.parse(preProcessedFile)

    print "List of typedef:"
    print "----------------"
    for el in tdef: 
        print "typedef " + tdef[el] + " " + el + ";"
    
    print ""
    print "Parsed structures:"
    print "-----------------"
    for el in struct:
        print struct[el]
        print "=================="
        
    print ""
    
    os.remove(preProcessedFile)

def printHelp(args):
    args.parserRef.print_help()
    
def parseArgs():
    program_version_message = "Version %s." % __version__
    program_short_description = __descr__
    
    parser = ArgumentParser(description=program_short_description, formatter_class=RawDescriptionHelpFormatter)
    parser.add_argument('-V', '--version', action='version', version=program_version_message)
    parser.set_defaults(parserRef=parser)
    parser.add_argument('file', type=str, help="Header file containing the structure")
    parser.add_argument('-s', '--struct', type=str, help="Top structure")
    parser.add_argument('-t', '--test', action='store_true', help="Test the parsing of the structure")
    
    # Process arguments
    args = parser.parse_args()
    
    if args.test:
        test(args)
    else:
        prepareProxy(args)

if __name__ == '__main__':
    
    parseArgs()
    sys.exit(0)
    topStruct = ""
    if len(sys.argv)<2:
        print "Please provide exactly one header file containing the configuration"
        print "structure and the name of the top structure (if more than 1 struct"
        print "in the file)"
        sys.exit(-1)
    
    if sys.argv[1]=="test":
        doTests()
    else:
        filePath = sys.argv[1]
        if len(sys.argv)>2:
            topStruct = sys.argv[2]
        prepareProxy()
