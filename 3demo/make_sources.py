import os
import make_sources_config

configuration = "output"

def generatePath(subdir, fname):
    return os.path.join(subdir,fname)

def getSources(baseDir, subDir):
	path = os.path.join(baseDir, subDir)
	print "Adding source directory: {0}".format(path)
	dirList = os.listdir(path)

	l = []
	for fileName in dirList:
		if fileName.endswith(".cpp") or fileName.endswith(".c") or fileName.endswith(".S"):
			l.append(generatePath(subDir, fileName))
	l.sort()
	return l

def makeSources(infoArray):
	sources = []
	for infoDict in infoArray:
		base = ""
		output = ""
		files = []
		if "base" in infoDict:
			base = infoDict["base"]
		if "output" in infoDict:
			output = infoDict["output"]
		if "dirs" in infoDict:
			dirs = infoDict["dirs"]
			for curDir in dirs:
				files += getSources(base, curDir)
		if "files" in infoDict:
			files += infoDict["files"]
		
		for f in files:
			fullSourcePath = os.path.join(base,f)
			if not os.path.isfile(fullSourcePath):
				raise Exception("File not found:"+fullSourcePath);
			fnoext = os.path.splitext(f)[0]
			sources.append(
				(fullSourcePath,
				os.path.join(configuration, os.path.join(output, fnoext+".o")),
				os.path.join(configuration, os.path.join(output, fnoext+".d"))
				)
			)

	return sources

def createDirectories(sources):
	"""	Create directories to files in sources list"""
	for s in sources:
		f = s[1] #object file
		d = os.path.dirname(f)
		if os.path.isdir(d):
			continue
		print "Create:"+d
		os.makedirs(d)
	pass
    
def main():
	sources = makeSources(make_sources_config.sources)
   
	makeFile = open("sources.mk", "w")
	makeFile.write("################################################################################\n")
	makeFile.write("# Automatically-generated file. Do not edit!\n")
	makeFile.write("################################################################################\n")
	makeFile.write("\n\n")
	makeFile.write("DEPS += \\\n")
	for s in sources:
		makeFile.write("\t"+s[2]+" \\\n")
	makeFile.write("\n\n")

	makeFile.write("OBJS += \\\n")
	for s in sources:
		makeFile.write("\t"+s[1]+" \\\n")
	makeFile.write("\n\n")

	for s in sources:
		ext = os.path.splitext(s[0])[1]
		if ext=='.S':
			makeFile.write(s[1]+": " + s[0] + "\n")
		else:
			#makeFile.write(s[1]+": " + s[0] + " " + s[2] + "\n")
			makeFile.write(s[1]+": " + s[0] + "\n")

		makeFile.write("\t@echo 'Building target: "+os.path.split(s[0])[1]+"'\n")
		if ext=='.cpp':
			makeFile.write('\t@$(CPP) $(CPP_FLAGS) -o "$@" "$<"\n\n')
		elif ext=='.c':
			makeFile.write('\t@$(CC) $(C_FLAGS) -o "$@" "$<"\n\n')
		elif ext=='.S':
			makeFile.write('\t@$(CC) $(ASM_FLAGS) -o "$@" "$<"\n\n')
		else:
			raise Exception("File extension not recognized:"+s[0]);

	makeFile.write("\n\n")
	makeFile.close()

	createDirectories(sources)
    
	print "all done."
    
main()
