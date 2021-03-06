'''
Created on 30 Mar 2015

@author: Nicolas
'''
import re

fileContent = """#include "%(FileBase)sProxy.h"
#include "XMLConfWriter.h"
#include "XMLConfParser.h"
#include <iostream>

static XMLConfParser gParser;
static std::string gLastFatalError;

const char* xml_getLastFatalError_%(struct)s(){ return gLastFatalError.c_str(); };

int xml_read_file_%(struct)s(const char* fileName){
    try {
    gParser.readFile(fileName);
    }
    catch (std::runtime_error& ex) {
        std::cout << "Fatal error: " << ex.what() << std::endl;
        gLastFatalError = ex.what();
        gParser.getLastError()->clear();
        return -1;
    }
    gParser.getLastError()->clear();
    return 0;
}

int xml_apply_%(struct)s(%(structtype)s%(struct)s *ptr){
    if(!gParser.isDocumentInitialised()) return -1;
    gParser.resetReadSuccess();
    try {
%(setters)s
    }
    catch (std::runtime_error& ex) {
        std::cout << "Fatal error: " << ex.what() << std::endl;
        gLastFatalError = ex.what();
        gParser.getLastError()->clear();
        return -1;
    }
    gParser.getLastError()->clear();

    return gParser.getReadSuccess();
}

int xml_test_%(struct)s(){
    if(!gParser.isDocumentInitialised()) return -1;
    gParser.resetReadSuccess();
    gParser.startCheckAdditional();
    try {
%(testers)s
    }
    catch (std::runtime_error& ex) {
        std::cout << "Fatal error: " << ex.what() << std::endl;
        gLastFatalError = ex.what();
        gParser.getLastError()->clear();
        return -1;
    }
    gParser.getLastError()->clear();

    std::cout << "XML tags without struct correspondance: " << std::endl;
    gParser.printAdditional();
    return gParser.getReadSuccess();
}

void* xml_compare_get_address_from_string(%(structtype)s%(struct)s *ptr, std::string diff){
    if(!gParser.isDocumentInitialised()) return NULL;
    if(diff.compare("")==0) return NULL;
%(comparers)s
    else return NULL;
}

void* xml_start_compare_%(struct)s(%(structtype)s%(struct)s *ptr){
    if(!gParser.isDocumentInitialised()) return NULL;
    std::string diff = gParser.getFirstDiff();

    return xml_compare_get_address_from_string(ptr, diff);
}

void* xml_next_compare_%(struct)s(%(structtype)s%(struct)s *ptr){
    if(!gParser.isDocumentInitialised()) return NULL;
    std::string diff = gParser.getNextDiff();

    return xml_compare_get_address_from_string(ptr, diff);
}

int xml_create_%(struct)s(%(structtype)s%(struct)s *ptr, const char* fileName){
    XMLConfWriter writer;
    try {
    writer.createDocument("%(struct)s");
    
%(creators)s
    return writer.writeDocument(fileName);
    
    }
    catch (std::runtime_error& ex) {
        std::cout << "Fatal error: " << ex.what() << std::endl;
        gLastFatalError = ex.what();
        writer.getLastError()->clear();
        return -1;
    }
    writer.getLastError()->clear();
    return 0;
}
"""

fileHeader = """#include "%(FileBase)s.h"

#ifndef %(capFileBase)sPROXY_H_
#define %(capFileBase)sPROXY_H_

#ifdef __cplusplus
#include <string>
extern "C" {
#endif
int xml_read_file_%(struct)s(const char* fileName);
int xml_apply_%(struct)s(%(structtype)s%(struct)s *ptr);
int xml_test_%(struct)s();
void* xml_start_compare_%(struct)s(%(structtype)s%(struct)s *ptr);
void* xml_next_compare_%(struct)s(%(structtype)s%(struct)s *ptr);
int xml_create_%(struct)s(%(structtype)s%(struct)s *ptr, const char* fileName);
const char* xml_getLastFatalError_%(struct)s();
#ifdef __cplusplus
}
#endif

#endif /* %(capFileBase)sPROXY_H_ */

"""

class struct(object):
    def __init__(self):
        self.name = ""
        self.typedef = None
        self.fields = []
    
    def add(self, vartype, name):
        self.fields.append((vartype, name))
        
    def printFields(self):
        text = ""
        for (k,v) in self.fields:
            text = text + "\t" + k + " => " + v + ";\n"
        return text
    
    def __repr__(self):
        if self.typedef is None:
            return "struct " + self.name + " {\n" + self.printFields() + "};"
        else:
            return "typedef struct " + self.name + " {\n" + self.printFields() + "} " + self.typedef +";"

def parseStruct(structLines):
    s = struct()
    re_compiled = re.compile("(?:typedef )?struct (.*)\s*{(.*)}\s*(.*?)\s*;", re.MULTILINE | re.DOTALL)
    m = re_compiled.findall("".join(structLines)) # regular expression to find struct name and eventual typedef
        
    if m:                                   # set them if found
        s.name = m[0][0].strip(" \n")
        tdef = m[0][2].strip(" \n")
        if tdef!="": 
            s.typedef = tdef 
    
        # Split the lines into individual statements with \n and ;
        listStatements = [x.split("\n") for x in m[0][1].split(";")]
        listStatements = [item for sublist in listStatements for item in sublist]
        for l in listStatements:
            l = l.strip()
            if l == "":                                 #Skip empty lines
                continue
            arr = l.split()                             # Split to get the variable type and names
            varNames = arr[-1].split(",")               # Split in case of several names
            for v in varNames:
                s.add(" ".join(arr[:-1]), v.strip(";")) # Add the variables
    return s
    
def parse(fileName):
    structArr = list()
    structLines = None
    inStruct = -1
    fd = open(fileName)
    try:
        for line in fd:
            if line.startswith('#'):                # Comment: skip line
                continue
            
            line = line.strip("\n")
            if "struct" in line and inStruct<=0:    # New struct definition found (struct not within a struct)
                inStruct += 1
                if structLines:                     # Skip when starting the for loop (structLines not yet initialised) 
                    structArr.append(structLines)
                structLines = [line]
                    
            else:                                   # Already in a struct: append line to the current struct
                if structLines:
                    structLines.append(line)
            
            #Count number of opening and closing brackets to understand when we exit the current struct definition 
            if "{" in line:
                inStruct += 1
            if "}" in line:
                inStruct -= 1
                if inStruct == 0:
                    inStruct = -1
    finally:
        fd.close()

    if structLines:                                 # Include last struct in array of struct
        structArr.append(structLines)
        
    structDict = {}
    typedefDict = {}
    
    for el in structArr:                            #Parse each struct individually and add them to the dictionary of struct
        s = parseStruct(el)
        structDict[s.name] = s
        if not s.typedef is None:                   # Also build the dictionary of typedef
           typedefDict[s.typedef] = s.name
        
    return (structDict,typedefDict)

def writeCFile(mainStruct, sDict, tdefDict, dirPath, baseName):
    if mainStruct.typedef is None:
        structName = mainStruct.name
        structType = "struct "
    else:
        structName = mainStruct.typedef
        structType = ""
    
    #Write source file
    fd = open(dirPath +"/" + baseName +"Proxy.cc", "w")
    try:
        fd.write(fileContent % {"structtype":structType,
                            "struct":structName,
                            "FileBase":baseName,
                            "setters":writeStructFields(mainStruct, sDict, tdefDict, structName+".", "ptr->"),
                            "testers":writeStructTest(mainStruct, sDict, tdefDict, structName+".", "ptr->"),
                            "creators":writeStructCreate(mainStruct, sDict, tdefDict, structName+".", "ptr->"),
                            "comparers":writeStructStartCompare(mainStruct, sDict, tdefDict, structName+".", "ptr->")})
    finally:
        fd.close()
        
    #Write header file
    fd = open(dirPath + "/" + baseName +"Proxy.h", "w")
    try:
        fd.write(fileHeader % {"structtype":structType,
                                   "struct":structName, 
                                   "FileBase":baseName,
                                   "capFileBase":baseName.upper()})
    finally:
        fd.close()


def writeStructFields(mainStruct, sDict, tdefDict, prefixString, prefixPointer, index=-1):
    '''
    Write the apply_ function for the specified structure
    '''
    setters = ""
    
    stringBasePath = prefixString
    ptrBasePath = prefixPointer

    for (vType,vName) in mainStruct.fields:                             # Loop over structure fields
        vName,arrSize = isArray(vName)                                  # Is it an array?
        stringPath = stringBasePath + vName                             # Build the path for the string: sname.a.b.c
        pointerPath = ptrBasePath + vName                               # Build the path for the pointer: ptr->a.b.c
        
        if isBasicType(vType):                                          # If basic type, we reached a final node
            if arrSize==0:                                              # call the get function
                setters += "\t" + wrapInCatchError("gParser", "gParser." + \
                        getFunctionType(vType) + '("' + stringPath + \
                        '",' + pointerPath + ")") + "\n"
            else:                                                       # call the get function for each element in the array
                for i in range(0,arrSize):
                    setters += "\t" + wrapInCatchError("gParser", "gParser." + \
                        getFunctionType(vType) + '("' + stringPath + \
                        '[' + str(i) + ']",' + pointerPath + "[" + \
                        str(i) + "])") + "\n"
        else:                                                           # Unkown or non-basic type (struct)
            if "struct" in vType:                                       # we don't care about the struct keyword
                vType = vType.replace("struct", "").strip()
            if vType in tdefDict:                                       # If found in the typedef, replace type with the struct name
             vType = tdefDict[vType]
            if vType in sDict:                                          # Find it in the dictionary
                if arrSize==0:                                          # Recurse
                    setters += writeStructFields(sDict[vType], sDict, tdefDict, stringPath+".", pointerPath+".")
                else:                                                   # Recurse for each element in the array
                    for i in range(0,arrSize):
                        setters += writeStructFields(sDict[vType], sDict, tdefDict, 
                                                          stringPath+"[%i]." % i, 
                                                          pointerPath+"[%i]." % i, i)
    return setters

def writeStructTest(mainStruct, sDict, tdefDict, prefixString, prefixPointer, index=-1):
    '''
    Write the test_ function for the specified structure
    '''

    tester = ""
    
    stringBasePath = prefixString
    ptrBasePath = prefixPointer

    for (vType,vName) in mainStruct.fields:                             # Loop over structure fields
        vName,arrSize = isArray(vName)                                  # Is it an array?
        stringPath = stringBasePath + vName                             # Build the path for the string: sname.a.b.c
        pointerPath = ptrBasePath + vName                               # Build the path for the pointer: ptr->a.b.c
        
        if isBasicType(vType):                                          # If basic type, we reached a final node
            if arrSize==0:                                              # call the get function
                tester += '\tgParser.pathExists("' + stringPath + \
                '");\n'
            else:
                for i in range(0,arrSize):                              # call the get function for each element in the array
                    tester += '\t!gParser.pathExists("' + stringPath + \
                    '[' + str(i) + ']");\n'
        else:                                                           # Unkown or non-basic type (struct)
            if "struct" in vType:                                       # we don't care about the struct keyword
                vType = vType.replace("struct", "").strip()
            if vType in tdefDict:                                       # If found in the typedef, replace type with the struct name
                vType = tdefDict[vType]
            if vType in sDict:                                          # Find it in the dictionary
                if arrSize==0:                                          # Recurse
                    tester += writeStructTest(sDict[vType], sDict, tdefDict, stringPath+".", pointerPath+".")
                else:
                    for i in range(0,arrSize):                          # Recurse for each element in the array
                        tester += writeStructTest(sDict[vType], sDict, tdefDict, 
                                                          stringPath+"[%i]." % i, 
                                                          pointerPath+"[%i]." % i, i)
    return tester

def writeStructCreate(mainStruct, sDict, tdefDict, prefixString, prefixPointer, index=-1):
    '''
    Write the create_ function for the specified structure
    '''
    creator = ""
    
    stringBasePath = prefixString
    ptrBasePath = prefixPointer

    for (vType,vName) in mainStruct.fields:                             # Loop over structure fields
        vName,arrSize = isArray(vName)                                  # Is it an array?
        stringPath = stringBasePath + vName                             # Build the path for the string: sname.a.b.c
        pointerPath = ptrBasePath + vName                               # Build the path for the pointer: ptr->a.b.c
        
        if isBasicType(vType):                                          # If basic type, we reached a final node
            if arrSize==0:                                              # call the get function
                creator += '\t' + wrapInCatchError("writer", 'writer.' + \
                        putFunctionType(vType) + '("' + stringPath + \
                        '",' + pointerPath + ')')+'\n'
            else:
                for i in range(0,arrSize):                              # call the get function for each element in the array
                    creator += '\t' + wrapInCatchError("writer", 'writer.' + \
                            putFunctionType(vType) + '("' + stringPath + \
                            '[' + str(i) + ']",' + pointerPath+'[' + \
                            str(i) + '])')+'\n'
        else:                                                           # Unkown or non-basic type (struct)
            if "struct" in vType:                                       # we don't care about the struct keyword
                vType = vType.replace("struct", "").strip()
            if vType in tdefDict:                                       # If found in the typedef, replace type with the struct name
                vType = tdefDict[vType]
            if vType in sDict:                                          # Find it in the dictionary
                if arrSize==0:                                          # Recurse
                    creator += writeStructCreate(sDict[vType], sDict, tdefDict, stringPath+".", pointerPath+".")
                else:
                    for i in range(0,arrSize):                          # Recurse for each element in the array
                        creator += writeStructCreate(sDict[vType], sDict, tdefDict, 
                                                          stringPath+"[%i]." % i, 
                                                          pointerPath+"[%i]." % i, i)
    return creator

def writeStructStartCompare(mainStruct, sDict, tdefDict, prefixString, prefixPointer, index=-1):
    '''
    Write the start_compare_ function for the specified structure
    '''

    tester = ""
    
    stringBasePath = prefixString
    ptrBasePath = prefixPointer

    for (vType,vName) in mainStruct.fields:                             # Loop over structure fields
        vName,arrSize = isArray(vName)                                  # Is it an array?
        stringPath = stringBasePath + vName                             # Build the path for the string: sname.a.b.c
        pointerPath = ptrBasePath + vName                               # Build the path for the pointer: ptr->a.b.c
        
        if isBasicType(vType):                                          # If basic type, we reached a final node
            if arrSize==0:                                              # call the get function
                tester += '\telse if(diff.compare("' + stringPath + \
                        '")==0) return &(' + pointerPath + ');\n'
            else:
                for i in range(0,arrSize):                              # call the get function for each element in the array
                    tester += '\telse if(diff.compare("' + stringPath + \
                    '[' + str(i) + ']")==0) return &(' + pointerPath + \
                    '[' + str(i) + ']);\n'
        else:                                                           # Unkown or non-basic type (struct)
            if "struct" in vType:                                       # we don't care about the struct keyword
                vType = vType.replace("struct", "").strip()
            if vType in tdefDict:                                       # If found in the typedef, replace type with the struct name
                vType = tdefDict[vType]
            if vType in sDict:                                          # Find it in the dictionary
                if arrSize==0:                                          # Recurse
                    tester += writeStructStartCompare(sDict[vType], sDict, tdefDict, stringPath+".", pointerPath+".")
                else:
                    for i in range(0,arrSize):                          # Recurse for each element in the array
                        tester += writeStructStartCompare(sDict[vType], sDict, tdefDict, 
                                                          stringPath+"[%i]." % i, 
                                                          pointerPath+"[%i]." % i, i)
    return tester

def wrapInCatchError(object, bool_statement):
    #return "if(!" + bool_statement + ") " + object + ".getLastError().stringStack();"
    return bool_statement + ";"

def isBasicType(vtype):
    '''
    Is the given type a basic type
    '''
    if vtype=="int":
        return True
    if vtype=="long int":
        return True
    elif vtype=="unsigned int":
        return True
    elif vtype=="xmlchar":
        return True
    elif vtype=="float":
        return True
    elif vtype=="double":
        return True
    elif vtype=="hexinteger":
        return True
    else:
        return False

def isArray(vName):
    m = re.findall("(.*?)\[(.*?)\].*", vName)
    if m:
        if m[0][1]=="XMLSTRING":
            return m[0][0],0
        return m[0][0],int(m[0][1])
    return vName,0

def putFunctionType(vType):
    if vType=="int":
        return "addPath"
    elif vType=="unsigned int":
        return "addPath"
    elif vType=="hexinteger":
        return "addPathAsHex"
    elif vType=="xmlchar":
        return "addPath"
    elif vType=="double":
        return "addPath"
    elif vType=="float":
        return "addPath"
    
def getFunctionType(vType):
    if vType=="int":
        return "getValue"
    elif vType=="unsigned int":
        return "getValue"
    elif vType=="hexinteger":
        return "getValue"
    elif vType=="xmlchar":
        return "getValue"
    elif vType=="double":
        return "getValue"
    elif vType=="float":
        return "getValue"
    
