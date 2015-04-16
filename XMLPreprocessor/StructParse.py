'''
Created on 30 Mar 2015

@author: Nicolas
'''
import re

fileContent = """#include "{FileBase}Proxy.h"
#include "XMLConfWriter.h"
#include "XMLConfParser.h"

void apply_{struct}({struct} *ptr, char* fileName){{
    XMLConfParser parser;
    parser.readFile(fileName);
    
{setters}}}

/*void test_{struct}({struct} *ptr){{
{testers}}}
*/
void create_{struct}({struct} *ptr, char* fileName){{
    XMLConfWriter writer;
    writer.createDocument("{struct}");
    
{creators}
    writer.writeDocument(fileName);
}}
"""

fileHeader = """#include "{FileBase}.h"

#ifndef {capFileBase}PROXY_H_
#define {capFileBase}PROXY_H_

#ifdef __cplusplus
extern "C" {{
#endif
void apply_{struct}({struct} *ptr, char* fileName);
//void test_{struct}({struct} *ptr);
void create_{struct}({struct} *ptr, char* fileName);

#ifdef __cplusplus
}}
#endif

#endif /* {capFileBase}PROXY_H_ */

"""

class struct():
    def __init__(self):
        self.name = ""
        self.typedef = ""
        self.fields = []
    
    def add(self, vartype, name):
        self.fields.append((vartype, name))
        
    def printFields(self):
        text = ""
        for (k,v) in self.fields:
            text = text + "\t" + k + " " + v + ";\n"
        return text
    
    def __repr__(self):
        if self.typedef=="":
            return "struct " + self.name + " {\n" + self.printFields() + "};"
        else:
            return "typedef struct " + self.name + " {\n" + self.printFields() + "} " + self.typedef +";"

def parseStruct(structLines):
    s = struct()
    m = re.findall("(?:typedef )?struct (.*) {(.*)}\s*(.*?)\s*;", "".join(structLines), re.MULTILINE | re.DOTALL)
    if m:
        s.name = m[0][0]
        s.typedef = m[0][2]
    
    for l in m[0][1].split("\n"):
        l = l.strip()
        if l == "":
            continue
        arr = l.split()
        s.add(" ".join(arr[:-1]), arr[-1][:-1])
    return s
    
def parse(fileName):
    structArr = list()
    structLines = None
    with open(fileName) as fd:
        for line in fd:
            if line.startswith('#'):
                continue
            if "struct" in line:
                if structLines:
                    structArr.append(structLines)
                structLines = [line]
            else:
                structLines.append(line)
    
    if structLines:
        structArr.append(structLines)
        
    structDict ={}
    for el in structArr:
        s = parseStruct(el)
        structDict[s.name] = s
        if not s.typedef == "":
            structDict[s.typedef] = s
        
    return structDict

def writeCFile(mainStruct, sDict, dirPath, baseName):
    with open(dirPath +"/" + baseName +"Proxy.cc", "w") as fd:
        fd.write(fileContent.format(struct=mainStruct.typedef,
        					FileBase=baseName,
                            setters=writeStructFields(mainStruct, sDict, mainStruct.typedef+".", "ptr->"),
                            testers=writeStructTest(mainStruct, sDict, mainStruct.typedef+".", "ptr->"),
                            creators=writeStructCreate(mainStruct, sDict, mainStruct.typedef+".", "ptr->")))
    with open(dirPath + "/" + baseName +"Proxy.h", "w") as fd:
        fd.write(fileHeader.format(struct=mainStruct.typedef, 
							       FileBase=baseName,
                                   capFileBase=baseName.upper()))


def writeStructFields(mainStruct, sDict, prefixString, prefixPointer, index=-1):
    setters = ""
    
    stringBasePath = prefixString
    ptrBasePath = prefixPointer

    for (vType,vName) in mainStruct.fields:
        vName,arrSize = isArray(vName)
        stringPath = stringBasePath + vName
        pointerPath = ptrBasePath + vName
        
        if isBasicType(vType):
            if arrSize==0:
                setters += "\tparser." + getFunctionType(vType) + \
                '("' + stringPath + '",' + pointerPath + ");\n"
            else:
                for i in range(0,arrSize):
                    setters += "\tparser." +getFunctionType(vType) + \
                    '("' + stringPath + '[' + str(i) + ']",' + \
                    pointerPath + "[" + str(i) + "]);\n"
        else:
            if vType in sDict:
                if arrSize==0:
                    setters += writeStructFields(sDict[vType], sDict, stringPath+".", pointerPath+".")
                else:
                    for i in range(0,arrSize):
                        setters += writeStructFields(sDict[vType], sDict, 
                                                          stringPath+"[%i]." % i, 
                                                          pointerPath+"[%i]." % i, i)
    return setters

def writeStructTest(mainStruct, sDict, prefixString, prefixPointer, index=-1):
    tester = ""
    
    stringBasePath = prefixString
    ptrBasePath = prefixPointer

    for (vType,vName) in mainStruct.fields:
        vName,arrSize = isArray(vName)
        stringPath = stringBasePath + vName
        pointerPath = ptrBasePath + vName
        
        if isBasicType(vType):
            if arrSize==0:
                tester += '\tif(!exists("' + stringPath + '")) printf("'+ \
                pointerPath+ ' was not found in XML file");\n'
            else:
                for i in range(0,arrSize):
                    tester += '\tif(!exists("' + stringPath + '_' + str(i) + '_")) printf("'+\
                    pointerPath+'[' + str(i) + '] was not found in XML file");\n'
        else:
            if vType in sDict:
                if arrSize==0:
                    tester += writeStructTest(sDict[vType], sDict, stringPath+".", pointerPath+".")
                else:
                    for i in range(0,arrSize):
                        tester += writeStructTest(sDict[vType], sDict, 
                                                          stringPath+"_%i_." % i, 
                                                          pointerPath+"[%i]." % i, i)
    return tester

def writeStructCreate(mainStruct, sDict, prefixString, prefixPointer, index=-1):
    creator = ""
    
    stringBasePath = prefixString
    ptrBasePath = prefixPointer

    for (vType,vName) in mainStruct.fields:
        vName,arrSize = isArray(vName)
        stringPath = stringBasePath + vName
        pointerPath = ptrBasePath + vName
        
        if isBasicType(vType):
            if arrSize==0:
                creator += '\twriter.' + putFunctionType(vType) + '("' + stringPath + '",' + pointerPath + ');\n'
            else:
                for i in range(0,arrSize):
                    creator += '\twriter.' + putFunctionType(vType) + '("' + stringPath + '[' + str(i) + ']",' + \
                    pointerPath+'[' + str(i) + ']);\n'
        else:
            if vType in sDict:
                if arrSize==0:
                    creator += writeStructCreate(sDict[vType], sDict, stringPath+".", pointerPath+".")
                else:
                    for i in range(0,arrSize):
                        creator += writeStructCreate(sDict[vType], sDict, 
                                                          stringPath+"[%i]." % i, 
                                                          pointerPath+"[%i]." % i, i)
    return creator

def isBasicType(vtype):
    if vtype=="int":
        return True
    elif vtype=="unsigned int":
        return True
    elif vtype=="xmlchar":
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
    
