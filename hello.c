// hello.c

#include <stdio.h>
#include <string.h>
#include "wren.h"

void simpleWrite(WrenVM* vm, const char* text)
{
    fputs(text, stderr);
}

char* simpleLoadModule(WrenVM* vm, const char* moduleName)
{
    // path <- moduleName + ".wren"
    size_t moduleNameLength = strlen(moduleName);
    char* path = (char*)malloc(moduleNameLength + 6);
    memcpy(path, moduleName, moduleNameLength);
    memcpy(path + moduleNameLength, ".wren", 5);
    path[moduleNameLength + 5] = '\0';

    // Open the file.
    FILE* f = fopen(path, "rb");
    free(path);
    if (f == NULL) {
	return NULL;
    }

    // Get the file length.
    fseek(f, 0, SEEK_END);
    size_t fileLength = ftell(f);
    rewind(f);

    // Read the file.
    char* fileContent = (char*)malloc(fileLength + 1);
    size_t bytesRead = fread(fileContent, sizeof(char), fileLength, f);
    fclose(f);
    if (fileLength != bytesRead) {
	free(fileContent);
	return NULL;
    }
    fileContent[fileLength] = '\0';

    return fileContent;
}

// This isn't necessary for a Hello World, but try making deliberate
// mistakes in hello.wren or in the 2nd parameter for wrenInterpret().
void reportError(WrenErrorType type, const char* module, int line,
    const char* message)
{
    switch (type) {
    case WREN_ERROR_COMPILE:
	fprintf(stderr, "compile time error: %s:%d:%s\n",
	    module, line, message);
	break;
    case WREN_ERROR_RUNTIME:
	fprintf(stderr, "runtime error: %s\n", message);
	break;
    case WREN_ERROR_STACK_TRACE:
	fprintf(stderr, "stack trace: %s:%d:%s\n", module, line, message);
	break;
    }
}

int main(int argc, const char* argv[])
{
    WrenConfiguration config;
    wrenInitConfiguration(&config);
    config.loadModuleFn = simpleLoadModule;
    config.writeFn = simpleWrite;
    config.errorFn = reportError;
    WrenVM* vm = wrenNewVM(&config);

    WrenInterpretResult result = wrenInterpret(vm,
	"System.print(\"I am running in a VM!\")\n"
	"import \"hello\" for Hello\n"
        "var h = Hello.new()\n"
	"h.hello()\n"
    );

    // Print the result.
    fputs("wrenInterpret(): ", stdout);
    switch (result) {
    case WREN_RESULT_SUCCESS:
	printf("success\n");
	break;
    case WREN_RESULT_COMPILE_ERROR:
	printf("compile error\n");
	break;
    case WREN_RESULT_RUNTIME_ERROR:
	printf("runtime error\n");
	break;
    }

    wrenFreeVM(vm);
    return 0;
}
