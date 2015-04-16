#!/usr/bin/python

'''
Created on 30 Mar 2015

@author: Nicoas
'''
from XMLPreprocessor import StructParse 
import sys
import os
from subprocess import call

if __name__ == '__main__':
	
	topStruct = ""
	if len(sys.argv)<2:
		print "Please provide exactly one header file containing the configuration"
		print "structure and the name of the top structure (if more than 1 struct"
		print "in the file)"
		sys.exit(-1)
	
	filePath = sys.argv[1]
	if len(sys.argv)>2:
		topStruct = sys.argv[2]
	
	if not os.path.isfile(filePath):
		print filePath + " is not a file"
		sys.exit(-1)
	
	fileName = os.path.basename(filePath)
	baseName = os.path.splitext(fileName)[0]
	dirName = os.path.dirname(filePath)
	if not os.path.isabs(dirName):
		dirName = dirName + os.getcwd()
	
	preProcessedFile = dirName + "/" + baseName + ".pre"
	
	call(["gcc", "-x", "c++", "-E", filePath, "-o", preProcessedFile])
	d = StructParse.parse(preProcessedFile)

	if len(d)==1:
		StructParse.writeCFile(d.values()[0], d, dirName, baseName)
	elif topStruct!="":
		if topStruct in d:
			StructParse.writeCFile(d[topStruct], d, dirName, baseName)
		else:
			print "The specified top structure (" + topStruct + ") is not found"
	else:
		print "More than 1 struct in file. Please specify which is the top one."

	os.remove(preProcessedFile)
		
