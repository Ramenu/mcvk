#!/bin/python

from os import listdir, walk
from os.path import isfile, splitext, join
from subprocess import run
from re import findall, match

# Tested using: include-what-you-use 0.19 based on clang version 15.0.7

class Color:
    BLACK = "\033[0;30m"
    RED = "\033[0;31m"
    GREEN = "\033[0;32m"
    BROWN = "\033[0;33m"
    BLUE = "\033[0;34m"
    PURPLE = "\033[0;35m"
    CYAN = "\033[0;36m"
    LIGHT_GRAY = "\033[0;37m"
    DARK_GRAY = "\033[1;30m"
    LIGHT_RED = "\033[1;31m"
    LIGHT_GREEN = "\033[1;32m"
    YELLOW = "\033[1;33m"
    LIGHT_BLUE = "\033[1;34m"
    LIGHT_PURPLE = "\033[1;35m"
    LIGHT_CYAN = "\033[1;36m"
    LIGHT_WHITE = "\033[1;37m"
    BOLD = "\033[1m"
    FAINT = "\033[2m"
    ITALIC = "\033[3m"
    UNDERLINE = "\033[4m"
    ENDC = "\033[0m"

SOURCE_DIR = "./src"
INCLUDE_DIR = "./include"
HEADER_EXTENSIONS = [".hpp", ".h", ".hxx", ".hh"]

def hasCorrectIncludes(fileName : str, pathNoRelative : str):
    for subdirs, _, files in walk(INCLUDE_DIR):
        for file in files:
            if splitext(file)[0] == splitext(fileName)[0]:
                headerPathNoRelative = join(subdirs, file)[2:]
                return (f"({pathNoRelative} has correct #includes/fwd-decls)" in output and \
                        f"({headerPathNoRelative} has correct #includes/fwd-decls)" in output, headerPathNoRelative)
    return (f"({pathNoRelative} has correct #includes/fwd-decls)" in output, _)


for path in listdir(SOURCE_DIR):
    if path.endswith('.cpp'):
        fullPath = SOURCE_DIR + '/' + path
        output = run(["include-what-you-use", "-std=c++20", f"-I{INCLUDE_DIR}", fullPath], capture_output=True).stderr.decode('utf-8')

        pathNoRelative = fullPath[2:]
        correct = hasCorrectIncludes(path, pathNoRelative) # returns pair with bool and header path (optional)
        if correct[0]:
            print(f"{Color.GREEN}{pathNoRelative} and {correct[1]} has correct #includes/fwd-decls ✔{Color.ENDC}")
        else:
            output = output.replace("#include", Color.RED + "#include" + Color.ENDC)

            extractedIncludes = findall(r"\".*\"", output)
            for include in extractedIncludes:
                output = output.replace(include, Color.BROWN + include + Color.ENDC)

            extractedIncludes = findall(r"<.*>", output)
            for include in extractedIncludes:
                output = output.replace(include, Color.BROWN + include + Color.ENDC)

            declarations = findall(r"namespace\s\w+ {|class\s\w+;|struct\s\w+;", output)
            for declaration in declarations:
                re = match(r"(namespace|class|struct)\s(\w+)", declaration)
                storageType = re.group(1)
                identifier = re.group(2)
                output = output.replace(f"{storageType} {identifier}", f"{Color.BLUE}{storageType} {Color.PURPLE}{identifier}{Color.ENDC}")
            
            extractedComments = findall(r"//.*\n", output)
            for comment in extractedComments:
                output = output.replace(comment, Color.DARK_GRAY + comment + Color.ENDC)

            addLineSuggestions = findall(r"(?s)((?:\w|\/|\.)+ should add these lines:.*?)The full include-list", output)
            for suggestion in addLineSuggestions:
                output = output.replace(suggestion, "")

            print(output)
            exit(1)
