#include "platform.h"
#include "logger.h"

#include <cstdio>
#include <stdexcept>

/// Helper for passing size static strings as function args.
/// For a function: `f(char*,size_t)` use `f(SIZED("hello"))`.
/// Expands to `f("hello",6)`.
#define SIZED(str) str, sizeof(str)

#define L (aq_logger)
#define LOG(...) L(0, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define ERR(...) L(1, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define EXPECT(e, ...)                                                         \
    do {                                                                       \
        if (!(e)) {                                                            \
            char buf[1 << 8] = { 0 };                                          \
            ERR(__VA_ARGS__);                                                  \
            snprintf(buf, sizeof(buf) - 1, __VA_ARGS__);                       \
            throw std::runtime_error(buf);                                     \
        }                                                                      \
    } while (0)
#define CHECK(e) EXPECT(e, "Expression evaluated as false: %s", #e)

void
reporter(int is_error,
         const char* file,
         int line,
         const char* function,
         const char* msg)
{
    fprintf(is_error ? stderr : stdout,
            "%s%s(%d) - %s: %s\n",
            is_error ? "ERROR " : "",
            file,
            line,
            function,
            msg);
}

int
main(int argc, char** argv)
{
    logger_set_reporter(reporter);

    // precondition for test

    struct file file;
    const char filename[] = "does-not-exist";

    remove(filename);

    EXPECT(
      0 == file_exists(SIZED(filename)),
      "The file \"%s\" must not already be present at the start of this test.",
      filename);

    EXPECT(file_create(&file, SIZED(filename)),
           "Expected the first creation of \"%s\" to succeed.",
           filename);
    file_close(&file);

    EXPECT(file_create(&file, SIZED(filename)),
           "Expected creation of \"%s\" to succeed. It was previously created, "
           "then closed, so it exists on the file system",
           filename);
    EXPECT(!file_create(&file, SIZED(filename)),
           "Expected creation of \"%s\" to fail. It hasn't been closed yet.",
           filename);

    return 0;
}