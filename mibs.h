// ------------------------------------------------------------------------------------------------------------
// mibs.h base library
// ------------------------------------------------------------------------------------------------------------
// 
// +mdoc #repo https://gitlab.com/kamkow1/mibs
//
// +mdoc #intro <h3>mibs.h - Mini Build System and toolset for C</h3>
// +mdoc #intro <h5>Getting started</h5>
// +mdoc #intro <br />
// +mdoc #intro To get started, you first need to download mibs.h
// +mdoc #intro <pre><code class="bash">wget https://gitlab.com/kamkow1/mibs/-/raw/master/mibs.h?ref_type=heads</code></pre><br />
// +mdoc #intro Now you can start writing your first build script!<br /><br /><br />
// +mdoc #intro Let's create a simple hello world program<br />
// +mdoc #intro <pre><code language="c">
// +mdoc #intro // main.c
// +mdoc #intro #include "stdio.h"
// +mdoc #intro 
// +mdoc #intro int main(void)
// +mdoc #intro {
// +mdoc #intro     printf("Hello world!!\n");
// +mdoc #intro     return 0;
// +mdoc #intro }
// +mdoc #intro </pre></code><br /><br />
// +mdoc #intro 
// +mdoc #intro Here's our build script<br />
// +mdoc #intro <pre><code language="c">
// +mdoc #intro // mibs.c
// +mdoc #intro #define MIBS_IMPL
// +mdoc #intro #include "mibs.h"
// +mdoc #intro 
// +mdoc #intro // We need an allocator to make use of mibs
// +mdoc #intro Mibs_Default_Allocator alloc = mibs_make_default_allocator();
// +mdoc #intro 
// +mdoc #intro bool build_main(void)
// +mdoc #intro {
// +mdoc #intro     Mibs_Cmd cmd = {0};
// +mdoc #intro     // We don't really have to deinitialize any memory, because our build script is a job-style program
// +mdoc #intro     mdefer { mibs_da_deinit(&alloc, &cmd); }
// +mdoc #intro 
// +mdoc #intro     // output executable name
// +mdoc #intro     #define OUT "main"
// +mdoc #intro 
// +mdoc #intro     // our source files (we can later on add more with #define SOURCES "main.c", "new-file.c")
// +mdoc #intro     #define SOURCES "main.c"
// +mdoc #intro 
// +mdoc #intro     // run GCC synchronously to compile our program and return a boolean indicating success or failure.
// +mdoc #intro     // any errors will be printed to standard output
// +mdoc #intro     mibs_cmd_append(&alloc, &cmd, "gcc", "-o", OUT, SOURCES);
// +mdoc #intro 
// +mdoc #intro     return mibs_run_cmd(&alloc, &cmd, MIBS_CMD_SYNC, NULL).ok;
// +mdoc #intro }
// +mdoc #intro 
// +mdoc #intro int main(int argc, char ** argv)
// +mdoc #intro {
// +mdoc #intro     // If GCC has failed, exit with 1
// +mdoc #intro     if (!build_main()) return 1;
// +mdoc #intro 
// +mdoc #intro     return 0;
// +mdoc #intro }
// +mdoc #intro 
// +mdoc #intro </code></pre><br /><br />
// +mdoc #intro 
// +mdoc #intro Now we can compile and run our build script and the program<br />
// +mdoc #intro <pre><code>cc -o mibs mibs.c</code></pre>
// +mdoc #intro <pre><code>./mibs</code></pre><br /><br />
// +mdoc #intro 
// +mdoc #intro Now our program "main" should be available
// +mdoc #intro <pre><code>./main</code></pre><br /><br />
// +mdoc #intro 
// +mdoc #intro If we make any changes to "mibs.c", we have to recompile mibs each time.<br />
// +mdoc #intro Sounds pretty tedious and defeats the point of mibs being a build system. <br />
// +mdoc #intro We can modify our build script a little do make mibs <i><b>recompile itself ( !!1!1! )</b></i>.<br /><br />
// +mdoc #intro 
// +mdoc #intro Here's our modified build script that recompiles itself upon detecting a change in the source file<br />
// +mdoc #intro <pre><code language="c">
// +mdoc #intro // mibs.c
// +mdoc #intro #define MIBS_IMPL
// +mdoc #intro #include "mibs.h"
// +mdoc #intro 
// +mdoc #intro Mibs_Default_Allocator alloc = mibs_make_default_allocator();
// +mdoc #intro 
// +mdoc #intro bool build_main(void)
// +mdoc #intro {
// +mdoc #intro     Mibs_Cmd cmd = {0};
// +mdoc #intro     mdefer { mibs_da_deinit(&alloc, &cmd); }
// +mdoc #intro 
// +mdoc #intro     #define OUT "main"
// +mdoc #intro 
// +mdoc #intro     #define SOURCES "main.c"
// +mdoc #intro 
// +mdoc #intro     mibs_cmd_append(&alloc, &cmd, "gcc", "-o", OUT, SOURCES);
// +mdoc #intro 
// +mdoc #intro     return mibs_run_cmd(&alloc, &cmd, MIBS_CMD_SYNC, NULL).ok;
// +mdoc #intro }
// +mdoc #intro 
// +mdoc #intro int main(int argc, char ** argv)
// +mdoc #intro {
// +mdoc #intro     // If mibs detects a change in "mibs.c", it will recompile itself and invoke the new version
// +mdoc #intro     mibs_rebuild(&alloc, argc, argv);
// +mdoc #intro 
// +mdoc #intro     if (!build_main()) return 1;
// +mdoc #intro 
// +mdoc #intro     return 0;
// +mdoc #intro }
// +mdoc #intro 
// +mdoc #intro </code></pre><br /><br />
// +mdoc #intro 
// +mdoc #intro Bootstrap the build system<br />
// +mdoc #intro <pre><code>cc -o mibs mibs.c</code></pre><br /><br />
// +mdoc #intro If we change "mibs.c", we don't have to call GCC again, we just let mibs do it's job
// +mdoc #intro <pre><code>./mibs</code></pre><br />
// +mdoc #intro 
// +mdoc #intro <h3>What is mibs exaclty?</h3>
// +mdoc #intro Mibs is a build system, as shown in the example above, but also a collection of useful functions and macros that make C much more ergonomic.<br />
// +mdoc #intro Mibs is also a toolset which currently consists of:
// +mdoc #intro <ul>
// +mdoc #intro <li><a class="link-info" href="https://gitlab.com/kamkow1/mibs/-/raw/master/Mimage.h?ref_type=heads">MImage.h</a> - <a class="link-info" href="https://raw.githubusercontent.com/nothings/stb/master/stb_image.h">stb_image.h</a> and <a class="link-info" href="https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h">stb_image_write.h</a> packaged together as one header, modified to support mibs allocators</li>
// +mdoc #intro <li><a class="link-info" href="https://gitlab.com/kamkow1/mibs/-/raw/master/c2sh.c?ref_type=heads">c2sh.c</a> - bundler that can package a library into a single header</li>
// +mdoc #intro <li><a class="link-info" href="https://gitlab.com/kamkow1/mibs/-/raw/master/mdg.c?ref_type=heads">mdg.c</a> along with <a class="link-info" href="https://gitlab.com/kamkow1/mibs/-/raw/master/mdg.h?ref_type=heads">mdg.h</a> - Metadata/Reflection generator for C (uses libclang)</li>
// +mdoc #intro <li><a class="link-info" href="https://gitlab.com/kamkow1/mibs/-/raw/master/mdoc.c?ref_type=heads">mdoc.c</a> - Minimal documentation generator for C</li>
// +mdoc #intro </ul>
// +mdoc #intro The premise of those tools is that they're small, consise, easy to extend/modify and embed within your project.<br /><br />
// +mdoc #intro 
// +mdoc #intro <h3>What's the point?</h3>
// +mdoc #intro There are so many build systems out there - Make, CMake, Ninja, Bazel and Meson just to name a few.<br />
// +mdoc #intro One common issue that I've ran into while using these tools is that they lack a proper programming language. <br />
// +mdoc #intro A proper programming language makes things, such as compile-time source generation, file processing,<br />
// +mdoc #intro resource embedding and more, much easier. Alternatively, I could use a bash script to compile my projects,<br />
// +mdoc #intro but bash as a programming language is just so goddamn awful. And so I've decided that my C projects will also use C as a build system,<br>
// +mdoc #intro which makes things easier by a whole lot.<br />
// +mdoc #intro - kamkow1<br/><br />
// +mdoc #intro 
// +mdoc #intro <h3>Special thanks</h3>
// +mdoc #intro I'd like to thank Kacper Lukstaedt (<a class="link-info" href="https://github.com/FluffyLuke">FluffyLuke</a>) for support during the development of the mibs toolset ;)<br />
// +mdoc #intro and <a class="link-info" href="https://twitch.tv/Tsoding">Tsoding</a> for his highly educational C-related streams
// +mdoc #intro 

#ifndef MIBS_H_
#define MIBS_H_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>
#include <stddef.h>
#include <inttypes.h>
#include <sys/stat.h>

#if defined(unix) || defined(__unix) || defined(__unix__)
#   define MIBS_PLATFORM_UNIX
#   include <unistd.h>
#   include <dirent.h>
#   include <fts.h>
#   include <sys/types.h>
#   include <sys/wait.h>
#   include <sys/mman.h>
#   define MIBS_NL "\n"
#elif defined(WIN32) || defined(WIN64) || defined(_WIN32)
#   define MIBS_PLATFORM_WINDOWS
#   define WIN32_LEAN_AND_MEAN
#   define _WINUSER_
#   define _IMM_
#   define _WINCON_
#   define NOGDI
#   define NOUSER
#   include <windows.h>
#   undef near
#   undef far
#   include <io.h>
#   include <wingdi.h>
#   include <Shlobj.h>
#   include <tchar.h>
#   define MIBS_NL "\r\n"
#endif

// +mdoc #macro MIBS_CC
// +mdoc #note  The default C compiler Mibs will use to recompile itself. On windows it's suggested to use #define "C:\MinGW\bin\gcc.exe"
#ifndef MIBS_CC
#   define MIBS_CC "cc"
#endif

// Pragma helpers
#if defined(__GNUC__) || defined(__clang__)
#   define MIBS_DO_PRAGMA(X) _Pragma(#X)
#   define MIBS_DISABLE_WARNING(warning) MIBS_DO_PRAGMA(GCC diagnostic ignored #warning)
#   define MIBS_DISABLE_WARNING_PUSH MIBS_DO_PRAGMA("GCC diagnostic push")
#   define MIBS_DISABLE_WARNING_POP MIBS_DO_PRAGMA("GCC diagnostic pop")
#endif

#define MIBS_ASSERT  assert
// +mdoc #macro MIBS_MALLOC
// +mdoc #note  Mibs' underlying allocator's malloc()
// +mdoc #usage #define MIBS_ASSERT my_assert
#define MIBS_MALLOC  malloc
// +mdoc #macro MIBS_REALLOC
// +mdoc #note  Mibs' underlying allocator's realloc()
// +mdoc #usage #define MIBS_MALLOC my_allocator_malloc
#define MIBS_REALLOC realloc
// +mdoc #macro MIBS_FREE
// +mdoc #note  Mibs' underlying allocator's free()
// +mdoc #usage #define MIBS_FREE my_allocator_free
#define MIBS_FREE    free
// +mdoc #macro MIBS_LOG_STREAM
// +mdoc #note  Default output log stream, can be a regular file
// +mdoc #usage #define MIBS_LOG_STREAM stderr
#define MIBS_LOG_STREAM stdout

// gets a piece of C code as text (literally)
#define mibs_text(x) #x

// +mdoc #macro         Mibs_Result
// +mdoc #param         TValue/_
// +mdoc #param_note    Decides the type of value that can be retrieved via ".value" field if ".ok" is true
#define Mibs_Result(TValue) struct { bool ok; TValue value; }

// alignment helpers
#define mibs_align_by(N, M) (M * ((N+M-1)/M))
#define mibs_align_by_power(N, M) \
    ({ \
        int tmp = 1; \
        while(tmp < N) tmp *= M; \
        tmp; \
    })

// types for allocator handlers

// +mdoc #macro         Mibs_Allocator_Alloc_Func
// +mdoc #note          Generate alloc function pointer declaration for underlying allocator type
// +mdoc #param         T/_
// +mdoc #param_note    Underlying allocator type
// +mdoc #param         _name/_
#define Mibs_Allocator_Alloc_Func(T, _name) void* (*_name)(struct Mibs_Allocator_##T *self, size_t size)
// +mdoc #macro         Mibs_Allocator_Free_Func
// +mdoc #note          Generate free function pointer declaration for underlying allocator type
// +mdoc #param         T/_
// +mdoc #param_note    Underlying allocator type
// +mdoc #param         _name/_
#define Mibs_Allocator_Free_Func(T, _name) void (*_name)(struct Mibs_Allocator_##T *self, void* mem)
// +mdoc #macro         Mibs_Allocator_Realloc_Func
// +mdoc #note          Generate realloc function pointer declaration for underlying allocator type
// +mdoc #param         T/_
// +mdoc #param_note    Underlying allocator type
// +mdoc #param         _name/_
#define Mibs_Allocator_Realloc_Func(T, _name) void* (*_name)(struct Mibs_Allocator_##T *self, void* mem, size_t prev_size, size_t new_size)

// +mdoc #macro         Mibs_Allocator_Base
// +mdoc #note          Generates a base for Mibs_Allocator(TUnderlying)
// +mdoc #param         TUnderlying/_
// +mdoc #param_note    Underlying allocator type which is used to generate base functions (alloc, realloc, free)
#define Mibs_Allocator_Base(TUnderlying) \
    struct Mibs_Allocator_Base_##TUnderlying { \
        Mibs_Allocator_Alloc_Func(TUnderlying, alloc_func); \
        Mibs_Allocator_Free_Func(TUnderlying, free_func); \
        Mibs_Allocator_Realloc_Func(TUnderlying, realloc_func); \
        TUnderlying *self; \
    }

// +mdoc #macro         Mibs_Allocator
// +mdoc #note          Generates a full allocator type
// +mdoc #param         TUnderlying/_
// +mdoc #param_note    User-provided underlying allocator type
// +mdoc #usage typedef Mibs_Allocator(My_Allocator_Underlying) My_Allocator;
#define Mibs_Allocator(TUnderlying) \
    struct Mibs_Allocator_##TUnderlying { \
        Mibs_Allocator_Base(TUnderlying) base; \
        TUnderlying underlying; \
    }

typedef void *(*Mibs_Simple_Alloc)(void *, size_t);
typedef void (*Mibs_Simple_Free)(void *, void *);
typedef void *(*Mibs_Simple_Realloc)(void *, void *, size_t, size_t);

// +mdoc #struct        Mibs_Allocator_Simple_Base
// +mdoc #note          Base of the simplified allocator
// +mdoc #struct_field  alloc_func/Mibs_Simple_Alloc
// +mdoc #struct_field  free_func/Mibs_Simple_Free
// +mdoc #struct_field  realloc_func/Mibs_Simple_Realloc
// +mdoc #struct_field  self/void*
// +mdoc #struct_field_note Pointer to the original allocator
typedef struct {
        Mibs_Simple_Alloc alloc_func;
        Mibs_Simple_Free free_func;
        Mibs_Simple_Realloc realloc_func;
        void *self;
} Mibs_Allocator_Simple_Base;

// +mdoc #struct        Mibs_Allocator_Simple
// +mdoc #note The simple allocator is just to hide the real allocator and pass it into functions (look at map_t implementation)
// +mdoc #struct_field  base/Mibs_Allocator_Simple_Base
typedef struct {
    Mibs_Allocator_Simple_Base base;
} Mibs_Allocator_Simple;

// +mdoc #macro         MIBS_ALLOC_SIMPLE
// +mdoc #note          Convert any allocator into a simplified Mibs_Allocator(T)-compliant interface
// +mdoc #param         X/_
// +mdoc #param_note    A pointer to any allocator type
// +mdoc #usage Mibs_Default_Allocator alloc = mibs_make_default_allocator();
// +mdoc #usage Mibs_Allocator_Simple simplified = MIBS_ALLOC_SIMPLE(&alloc);
// +mdoc #usage some_function(&simplified);
#define MIBS_ALLOC_SIMPLE(X) ({ \
    Mibs_Allocator_Simple *_x = (Mibs_Allocator_Simple*)(X); \
    _x->base.self = X; \
    _x; \
})

typedef Mibs_Allocator(int) Mibs_Default_Allocator;

// +mdoc #macro         mibs_make_default_allocator
// +mdoc #funclike
// +mdoc #return_note   Returns a Mibs_Default_Allocator instance
#define mibs_make_default_allocator() \
    ((Mibs_Default_Allocator) { \
        .base = { \
            .self = 0, \
            .alloc_func = mibs_default_alloc, \
            .free_func = mibs_default_free, \
            .realloc_func = mibs_default_realloc \
        } \
    })

// +mdoc #function      mibs_default_alloc
// +mdoc #note          Calls to MIBS_MALLOC(), which is malloc() by default
// +mdoc #return        void*
// +mdoc #param         default_alloc/Mibs_Default_Allocator*
// +mdoc #param         size/size_t
// +mdoc #param_note    Size to be allocated
void* mibs_default_alloc(Mibs_Default_Allocator* default_alloc, size_t size);
// +mdoc #function      mibs_default_free
// +mdoc #note          Calls to MIBS_FREE, which is free() by default
// +mdoc #return        void
// +mdoc #param         default_alloc/Mibs_Default_Allocator*
// +mdoc #param         mem/void*
// +mdoc #param_note    Memory to be freed
void mibs_default_free(Mibs_Default_Allocator* default_alloc, void* mem);
// +mdoc #function      mibs_default_realloc
// +mdoc #note          Calls to MIBS_REALLOC, which is realloc() by default
// +mdoc #return        void*
// +mdoc #return_note   Pointer to resized memory
// +mdoc #param         default_alloc/Mibs_Default_Allocator*
// +mdoc #param         mem/void*
// +mdoc #param_note    Memory to be resized
// +mdoc #param         prev_size/size_t
// +mdoc #param_note    Size of memory before resizing; IGNORED
// +mdoc #param         new_size/size_t
// +mdoc #param_note    Desired size of memory
void* mibs_default_realloc(Mibs_Default_Allocator* default_alloc, void* mem, size_t prev_size, size_t new_size);

// +mdoc #struct        Mibs_Arena_Allocator_Underlying
// +mdoc #note          Underlying structure behind the arena allocator
// +mdoc #struct_field  buffer/void*
// +mdoc #struct_field_note Allocated buffer, which can be shared between processes and is freed all at once
// +mdoc #struct_field  end/void*
// +mdoc #struct_field_note A cursor within the buffer
// +mdoc #struct_field  capacity/size_t
// +mdoc #struct_field_note Max size which the arena can hold
typedef struct {
    void *buffer;
    void *end;
    size_t capacity;
} Mibs_Arena_Allocator_Underlying;

typedef Mibs_Allocator(Mibs_Arena_Allocator_Underlying) Mibs_Arena_Allocator;

// +mdoc #macro         mibs_make_arena_allocator
// +mdoc #note          Create a Mibs_Arena_Allocator instance
// +mdoc #note          NOTICE: Allocated memory has MAP_ANONYMOUS and MAP_SHARED flags enabled!
// +mdoc #note          If you wish to change this behaviour, modify your copy of mibs.h directly
// +mdoc #param         _capacity/_
// +mdoc #param_note    Max capacity the arena can use
// +mdoc #usage Mibs_Arena_Allocator alloc = mibs_make_arena_allocator(1024*8); // 8KiB
// +mdoc #usage 
// +mdoc #usage typedef Mibs_Da(int) Ints;
// +mdoc #usage 
// +mdoc #usage Ints ints = {0};
// +mdoc #usage mdefer { mibs_da_deinit(&alloc, &ints); } // This statement is now useless
// +mdoc #usage 
// +mdoc #usage while(1) {
// +mdoc #usage     mibs_da_append(&alloc, &ints, 1);
// +mdoc #usage     mibs_da_append(&alloc, &ints, 2);
// +mdoc #usage     mibs_da_append(&alloc, &ints, 3);
// +mdoc #usage     mibs_da_append(&alloc, &ints, 4);
// +mdoc #usage     
// +mdoc #usage     mibs_arena_reset(&alloc); // reset the buffer
// +mdoc #usage }
// +mdoc #usage 
// +mdoc #usage mibs_arena_cleanup(&alloc); // free memory all at once
#define mibs_make_arena_allocator(_capacity) (Mibs_Arena_Allocator){ \
    .base = { .alloc_func = mibs_arena_alloc, .free_func = mibs_arena_free, .realloc_func = mibs_arena_realloc }, \
    .underlying = { .buffer = 0, .end = 0, .capacity = _capacity } \
}

// +mdoc #function      mibs_arena_alloc
// +mdoc #note          Allocate using an arena
// +mdoc #return        void*
// +mdoc #return_note   pointer to memory within the buffer
// +mdoc #param         alloc/Mibs_Arena_Allocator*
// +mdoc #param         size/size_t
// +mdoc #param_note    size to allocate
void *mibs_arena_alloc(Mibs_Arena_Allocator *alloc, size_t size);
// +mdoc #function      mibs_arena_free
// +mdoc #note          Doesn't do anything, because an arena cannot free memory arbitrarly
// +mdoc #return        void
// +mdoc #param         alloc/Mibs_Arena_Allocator*
// +mdoc #param         mem/void*
void mibs_arena_free(Mibs_Arena_Allocator *alloc, void *mem);
// +mdoc #function      mibs_arena_realloc
// +mdoc #note          Resize using an arena
// +mdoc #note          This function won't actually resize.
// +mdoc #note          It will just allocate a new segmnet of specified size, copy old memory and return the pointer
// +mdoc #return        void*
// +mdoc #return_note   pointer to resized memory
// +mdoc #param         alloc/Mibs_Arena_Allocator*
// +mdoc #param         mem/void*
// +mdoc #param_note    Memory to resize
// +mdoc #param         psz/size_t
// +mdoc #param_note    Size of previous/current allocation
// +mdoc #param         nsz/size_t
// +mdoc #param_note    Desired size after resizing
void *mibs_arena_realloc(Mibs_Arena_Allocator *alloc, void *mem, size_t psz, size_t nsz);
// +mdoc #function      mibs_arena_cleanup
// +mdoc #note          Free the entire buffer all at once
// +mdoc #return        void
// +mdoc #param         alloc/Mibs_Arena_Allocator*
void mibs_arena_cleanup(Mibs_Arena_Allocator *alloc);
// +mdoc #function      mibs_arena_reset
// +mdoc #note          Reset the buffer to point to the beginning
// +mdoc #return        void
// +mdoc #param         alloc/Mibs_Arena_Allocator*
void mibs_arena_reset(Mibs_Arena_Allocator *alloc);

// Allocation helper functions
// these functions are simple wrappers for calling an allocator's handlers

// +mdoc #macro         mibs_alloc
// +mdoc #note          Allocate memory using any Mibs_Allocator(T)-compliant allocator
// +mdoc #return_note   Returns a pointer to allocated memory
// +mdoc #param         alloc/_
// +mdoc #param_note    pointer to a structure that complies to Mibs_Allocator(T) interface
// +mdoc #param         size/_
// +mdco #param_note    Size to be allocated
// +mdoc #usage Mibs_Default_Allocator alloc = mibs_make_default_allocator();
// +mdoc #usage char* buf = mibs_alloc(&alloc, 1024);
#define mibs_alloc(alloc, size) ((alloc)->base.alloc_func((alloc)->base.self != 0 ? (alloc)->base.self : (void*)(alloc), size))
// +mdoc #macro         mibs_free
// +mdoc #note          Free memory using any Mibs_Allocator(T)-compliant allocator
// +mdoc #param         alloc/_
// +mdoc #param_note    pointer to a structure that complies to Mibs_Allocator(T) interface
// +mdoc #param         mem/_
// +mdoc #param_note    Memory to be freed
// +mdoc #usage Mibs_Default_Allocator alloc = mibs_make_default_allocator();
// +mdoc #usage char* buf = mibs_alloc(&alloc, 1024);
// +mdoc #usage mibs_free(&alloc, buf);
#define mibs_free(alloc, mem) ((alloc)->base.free_func((alloc)->base.self != 0 ? (alloc)->base.self : (void*)(alloc), mem))
// +mdoc #macro         mibs_realloc
// +mdoc #note          Resizes memory using any Mibs_Allocator(T)-compliant allocator
// +mdoc #param         alloc/_
// +mdoc #param_note    pointer to a structure that complies to Mibs_Allocator(T) interface
// +mdoc #param         mem/_
// +mdoc #param_note    Memory to be resized
// +mdoc #param         prevsz/_
// +mdoc #param_note    Size before reallocation
// +mdoc #param         newsz/_
// +mdoc #param_note    Desired size of reallocated memory
// +mdoc #usage Mibs_Default_Allocator alloc = mibs_make_default_allocator();
// +mdoc #usage char* buf = mibs_alloc(&alloc, 1024);
// +mdoc #usage buf = mibs_realoc(&alloc, buf, 1024, 2048);
// +mdoc #usage mibs_free(&alloc, buf);
#define mibs_realloc(alloc, mem, prevsz, newsz) ((alloc)->base.realloc_func((alloc)->base.self != 0 ? (void*)(alloc)->base.self : (alloc), mem, prevsz, newsz))


// ------------------------------------------------------------------------------------------------------------
// Runtime utilities
// ------------------------------------------------------------------------------------------------------------

#define MIBS_RTINIT   __attribute__((constructor))
#define MIBS_RTDEINIT __attribute__((destructor))

typedef Mibs_Arena_Allocator Mibs_Scratch_Allocator;

#ifndef MIBS_SCRATCH_ALLOC_SIZE
// +mdoc #macro         MIBS_SCRATCH_ALLOC_SIZE
// +mdoc #note          Default scratch allocator size - 1024*4
// +mdoc #usage #define MIBS_SCRATCH_ALLOC_SIZE 1024*8 // 8KiB
// +mdoc #usage #include "mibs.h"
#   define MIBS_SCRATCH_ALLOC_SIZE 1024*4
#endif

// +mdoc #var           mibs_scratch_alloc/Mibs_Scratch_Alloc
// +mdoc #note          Scratch allocator for small, temporary allocations
extern Mibs_Scratch_Allocator mibs_scratch_alloc;

// +mdoc #function      mibs_rt_init
// +mdoc #note          Called before main()
// +mdoc #return        void
MIBS_RTINIT   void mibs_rt_init(void);
// +mdoc #function      mibs_rt_deinit
// +mdoc #note          Called after main()
// +mdoc #return        void
MIBS_RTDEINIT void mibs_rt_deinit(void);

struct map_node_t;
typedef struct map_node_t map_node_t;

// +mdoc #struct        map_base_t
// +mdoc #note          Base of a type generated by map_t()
// +mdoc #struct_field  buckets/map_node_t**
// +mdoc #struct_field  nbuckets/unsigned
// +mdoc #struct_field  nnodes/unsigned
// +mdoc #struct_field  alloc/Mibs_Allocator_Simple*
typedef struct {
  map_node_t **buckets;
  unsigned nbuckets, nnodes;
  Mibs_Allocator_Simple *alloc;
} map_base_t;

// +mdoc #struct        map_iter_t
// +mdoc #note          Map iterator type
// +mdoc #struct_field  bucketidx/unsigned
// +mdoc #struct_field  node/map_node_t*
typedef struct {
  unsigned bucketidx;
  map_node_t *node;
} map_iter_t;

// +mdoc #macro         map_t
// +mdoc #note          Simple String -> T map implementation by <a class="link-info" href="https://github.com/rxi/">rxi</a>
// +mdoc #note          Modified by kamkow1 to support Mibs allocators
// +mdoc #param         T/_
// +mdoc #param_note    Underlying type, for eg. map_t(int)
// +mdoc #usage typedef map_t(int) Int_Map;
// +mdoc #usage 
// +mdoc #usage Mibs_Default_Allocator alloc = mibs_make_default_allocator();
// +mdoc #usage Int_Map int_map;
// +mdoc #usage map_init(&alloc, &int_map);
// +mdoc #usage mdefer { map_deinit(&int_map); }
// +mdoc #usage 
// +mdoc #usage map_set(&int_map, "Hello", 1234);
// +mdoc #usage int* x = map_get(&int_map, "Hello");
// +mdoc #usage if (x != NULL) {
// +mdoc #usage     printf("Hello = %d\n", *x);
// +mdoc #usage }
#define map_t(T)\
  struct { map_base_t base; T *ref; T tmp; }

// +mdoc #macro         map_init
// +mdoc #note          Initializes a map with an allocator
// +mdoc #param         a/_
// +mdoc #param_note    Pointer to an allocator
// +mdoc #param         m/_
// +mdoc #param_note    pointer to a map_t(T) instance
#define map_init(a, m)\
do { \
  memset(m, 0, sizeof(*(m))); \
  (m)->base.alloc = MIBS_ALLOC_SIMPLE(a); \
} while(0)

// +mdoc #macro         map_deinit
// +mdoc #note          Deinitialize a map with a previously assigned allocator
// +mdoc #param         m/_
// +mdoc #param_note    pointer to a map(T) instance
#define map_deinit(m)\
  map_deinit_(&(m)->base)

// +mdoc #macro         map_get
// +mdoc #note          Retrieve an element from map_t using a string key
// +mdoc #return_note   Pointer to the requested element. If it's missing, NULL will be returned
// +mdoc #param         m/_
// +mdoc #param_note    Pointer to a map_t(T) instance
// +mdoc #param         key/_
// +mdoc #param_note    string that represents a key within the map
#define map_get(m, key)\
  ( (m)->ref = map_get_(&(m)->base, key) )

// +mdoc #macro         map_set
// +mdoc #note          Places an element in map_t using a string key
// +mdoc #note          Will override an existing element
// +mdoc #param         m/_
// +mdoc #param_note    Pointer to a map_t(T) instance
// +mdoc #param         key/_
// +mdoc #param_note    string key
// +mdoc #param         value/_
// +mdoc #param_note    value to be set within the map
#define map_set(m, key, value)\
  ( (m)->tmp = (value),\
    map_set_(&(m)->base, key, &(m)->tmp, sizeof((m)->tmp)) )

// +mdoc #macro         map_remove
// +mdoc #note          Removes an element from map_t using a key
// +mdoc #note          If the key is not present, operation has no effect
// +mdoc #param         m/_
// +mdoc #param_note    Pointer to a map_t(T) instance
// +mdoc #param         key/_
// +mdoc #param_note    string key
#define map_remove(m, key)\
  map_remove_(&(m)->base, key)

// +mdoc #macro         map_iter
// +mdoc #note          Returns a map_iter_t instance, which is used to iterate a map with map_next()
// +mdoc #return_note   map_iter_t instance
// +mdoc #param         m/_
// +mdoc #param_note    Pointer to a map_t(T) instance
// +mdoc #usage const char *key;
// +mdoc #usage map_iter_t iter = map_iter(&m);
// +mdoc #usage 
// +mdoc #usage while ((key = map_next(&m, &iter))) {
// +mdoc #usage   printf("%s -> %d", key, *map_get(&m, key));
// +mdoc #usage }
#define map_iter(m)\
  map_iter_()

// +mdoc #macro         map_next
// +mdoc #return_note   string key
// +mdoc #param         m/_
// +mdoc #param_note    Pointer to a map_t(T) instance
// +mdoc #param         iter/_
// +mdoc #param_note    pointer to map_iter_t instance
#define map_next(m, iter)\
  map_next_(&(m)->base, iter)

void map_deinit_(map_base_t *m);
void *map_get_(map_base_t *m, const char *key);
int map_set_(map_base_t *m, const char *key, void *value, int vsize);
void map_remove_(map_base_t *m, const char *key);
map_iter_t map_iter_(void);
const char *map_next_(map_base_t *m, map_iter_t *iter);

struct map_node_t {
  unsigned hash;
  void *value;
  map_node_t *next;
  /* char key[]; */
  /* char value[]; */
};

#define mibs_is_unsigned(x) (x >= 0 && ~x >= 0)
#define mibs_is_unsigned_type(T) ((T)0 - 1 > 0)
#define mibs_is_signed(x) (!mibs_is_unsigned((x)))
#define mibs_is_signed_type(T) (!mibs_is_unsigned_type((T)))

#define mibs_array_len(a) (sizeof(a)/sizeof(a[0]))
#define mibs_array_get(a, i) (MIBS_ASSERT(i < mibs_array_len(a)), a[i])

// +mdoc #macro         Mibs_Da
// +mdoc #note          Generic type-safe dynamic array
// +mdoc #param         T/_
// +mdoc #param_note    Type that will be stored within the array
// +mdoc #usage typedef Mibs_Da(int) ints;
// +mdoc #usage 
// +mdoc #usage Mibs_Default_Allocator alloc = mibs_make_default_allocator();
// +mdoc #usage 
// +mdoc #usage ints is = {0};
// +mdoc #usage mdefer { mibs_da_deinit(&alloc, &is); }
// +mdoc #usage mibs_da_append(&alloc, &is, 1);
// +mdoc #usage mibs_da_append(&alloc, &is, 2);
// +mdoc #usage mibs_da_append(&alloc, &is, 3);
// +mdoc #usage 
// +mdoc #usage for (size_t i = 0; i < is.count; i++) {
// +mdoc #usage     printf("int %d\n", is.items[i]);
// +mdoc #usage }
#define Mibs_Da(T) struct { T* items; size_t count; size_t cap; }

#define mibs_da_get(da, i) (MIBS_ASSERT((i) < (da)->count), (da)->items[i])

// +mdoc #macro         mibs_da_deinit
// +mdoc #note          Free all items and set count to zero
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         da/_
// +mdoc #param_note    pointer to Mibs_Da(T) instance
#define mibs_da_deinit(alloc, da) \
    do { \
    	(da)->count = 0; \
        mibs_free((alloc), (da)->items); \
    } while(0)

// +mdoc #macro         mibs_da_append
// +mdoc #note          Append to a Mibs_Da(T) instance
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         da/_
// +mdoc #param_note    pointer to Mibs_Da(T) instance
// +mdoc #param         item/_
// +mdoc #param_note    Item to be appended
#define mibs_da_append(alloc, da, item) \
    do { \
        if ((da)->count == 0) { \
            (da)->items = mibs_alloc((alloc), sizeof((item))); \
            (da)->cap = 1; \
        } else { \
        	if ((da)->count == (da)->cap) { \
                size_t old_cap = (da)->cap * sizeof((item)); \
	        	(da)->cap *= 2; \
		        (da)->items = mibs_realloc((alloc), (da)->items, old_cap, (da)->cap * sizeof((item))); \
        	} \
        } \
    	(da)->items[(da)->count++] = item; \
    } while(0)

// +mdoc #macro         mibs_da_append_array
// +mdoc #note          Append an entire C array to a Mibs_Da(T) instance
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         da/_
// +mdoc #param_note    pointer to Mibs_Da(T) instance
// +mdoc #param         array/_
// +mdoc #param_note    array to append
#define mibs_da_append_array(alloc, da, array) \
    do { \
        for (size_t i = 0; i < mibs_array_len((array)); i++) { \
            mibs_da_append((alloc), (da), mibs_array_get(array, i)); \
        } \
    } while(0)

// +mdoc #macro         mibs_da_append_many
// +mdoc #note          Append multiple items at callsite to a Mibs_Da(T) instance
// +mdoc #variadic
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         da/_
// +mdoc #param_note    pointer to Mibs_Da(T) instance
// +mdoc #param         T/_
// +mdoc #param_note    type of appended items
#define mibs_da_append_many(alloc, da, T, ...) \
    mibs_da_append_array((alloc), (da), ((T[]){ __VA_ARGS__ }))

// +mdoc #macro         mibs_da_concat
// +mdoc #note          Concatinate two Mibs_Da(T) instances into one
// +mdoc #return_note   Mibs_Da(T) instance
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         T/_
// +mdoc #param_note    Type of the two dynamic arrays
// +mdoc #param         da1/_
// +mdoc #param         da2/_
#define mibs_da_concat(alloc, T, da1, da2) \
    ({ \
        Mibs_Da(T) result = {0}; \
        for (size_t i = 0; i < (da1)->count; i++) mibs_da_append((alloc), (da1), mibs_da_get((da1), i)); \
        for (size_t i = 0; i < (da2)->count; i++) mibs_da_append((alloc), (da2), mibs_da_get((da2), i)); \
        result; \
    })

// +mdoc #macro         mibs_da_concat_str_items
// +mdoc #note          Concatinate a Mibs_Da(T) instance into a string using a delimiter character
// +mdoc #return_note   Pointer to string; USER OWNS THE MEMORY
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         da/_
// +mdoc #param_note    Must hold character data
// +mdoc #param         delim/_
// +mdoc #param_note    Single character used to concatinate the array, for eg. ','
#define mibs_da_concat_str_items(alloc, da, delim) \
    ({ \
        size_t size = 0; \
        for (size_t i = 0; i < (da)->count; i++) size += strlen((mibs_da_get((da), i)))+1; \
        char* buf = (char*)mibs_alloc((alloc), size); \
        memset(buf, 0, size); \
        size_t offset = 0; \
        for (size_t i = 0; i < (da)->count; i++) { \
            const char* str = mibs_da_get((da), i); \
            memcpy(buf+offset, str, strlen(str)); \
            offset += strlen(str); \
            char d[2]; \
            memset(d, 0, sizeof(d)); \
            d[0] = delim; \
            d[1] = '\0'; \
            if (i < (da)->count - 1) { \
                memcpy(buf+offset, d, sizeof(d)/sizeof(d[0])); \
                offset += 1; \
            } \
        } \
        buf; \
    })

// +mdoc #macro         mibs_da_clone
// +mdoc #note          Clone a Mibs_Da(T) instance
// +mdoc #return_note   Cloned Mibs_Da(T); USER OWNS THE MEMORY
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         T/_
// +mdoc #param_note    Type of items stored in the array
// +mdoc #param         da/_
// +mdoc #param_note    Pointer to Mibs_Da(T) instance
#define mibs_da_clone(alloc, T, da) \
    ({ \
        T new_da = {0}; \
        for (size_t i = 0; i < (da)->count; i++) mibs_da_append((alloc), &new_da, mibs_da_get((da), i)); \
        new_da; \
    })

// +mdoc #macro         mibs_da_remove
// +mdoc #note          Remove an element from Mibs_Da(T) using an index
// +mdoc #note          WARNING: This macro will shift the indicies of the array
// +mdoc #param         da/_
// +mdoc #param_note    Pointer to Mibs_Da(T) instance
// +mdoc #param         index/_
// +mdoc #param_note    Index of element to remove
#define mibs_da_remove(da, index) \
    do { \
        for (size_t j = index; j < (da)->count; j++) { \
            (da)->items[j] = (da)->items[j+1]; \
        } \
        (da)->count -= 1; \
    } while(0)

#define mibs_copy_string(dest, src) (strcpy(dest, src));

// Da derivative types
typedef Mibs_Da(char) Mibs_DString;
typedef Mibs_Da(char) Mibs_String_Builder;
typedef Mibs_Da(Mibs_DString) Mibs_DString_Builder;
typedef Mibs_Da(const char*) Mibs_String_Array;
typedef Mibs_Da(void*) Mibs_Ptrs;

// +mdoc #macro         mibs_free_ptrs_a
// +mdoc #note          Free pointers using an allocator
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         ptrs/_
// +mdoc #param_note    Pointer to a Mibs_Ptrs instance
#define mibs_free_ptrs_a(alloc, ptrs) \
{ \
    for (size_t i = 0; i < (ptrs)->count; i++) { \
        mibs_free((alloc), (ptrs)->items[i]); \
    } \
}

// +mdoc #macro         mibs_free_ptrs
// +mdoc #note          Free pointers by calling free()
// +mdoc #param         ptrs/_
// +mdoc #param_note    Pointer to a Mibs_Ptrs instance
#define mibs_free_ptrs(ptrs) \
{ \
    for (size_t i = 0; i < (ptrs)->count; i++) { \
        free((ptrs)->items[i]); \
    } \
}

// +mdoc #macro         mibs_sb_append_cstr
// +mdoc #note          Append a C string to a Mibs_String_Builder instance
// +mdoc #note          Will not include the '\0' byte!
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         sb/_
// +mdoc #param_note    Pointer to a Mibs_String_Builder instance
// +mdoc #param         s/_
// +mdoc #param_note    C String
#define mibs_sb_append_cstr(alloc, sb, s) \
do { \
    const char* c = (char*)s; \
    while(*c) mibs_da_append((alloc), (sb), *c++); \
} while(0)

// +mdoc #macro         mibs_sb_append_char
// +mdoc #note          Append a single character to a Mibs_String_Builder instance
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         sb/_
// +mdoc #param_note    Pointer to a Mibs_String_Builder instance
// +mdoc #param         c/_
// +mdoc #param_note    Character
#define mibs_sb_append_char(alloc, sb, c) mibs_da_append((alloc), (sb), (c))
// +mdoc #macro         mibs_sb_append_null
// +mdoc #note          Append a '\0' character to a Mibs_String_Builder instance
// +mdoc #note          Use this after finishing building your string
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         sb/_
// +mdoc #param_note    Pointer to a Mibs_String_Builder instance
#define mibs_sb_append_null(alloc, sb) mibs_da_append((alloc), (sb), '\0')

// +mdoc #macro         mibs_split_cstr
// +mdoc #note          Splits a C String into a Mibs_String_Array instance
// +mdoc #return_note   Mibs_String_Array instance; USER OWNS THE MEMORY
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         s/_
// +mdoc #param_note    C String
// +mdoc #param         delims/_
// +mdoc #param_note    List of delimiters represented by a string, for eg. ",;/"
#define mibs_split_cstr(alloc, s, delims) \
({ \
    Mibs_String_Array sa = {0}; \
    char* ch = strtok((char*)(s), (char*)(delims)); \
    while (ch != NULL) { \
        mibs_da_append((alloc), &sa, ch); \
        ch = strtok(NULL, (char*)(delims)); \
    } \
    sa; \
})

// +mdoc #macro         mibs_ds_append_null
// +mdoc #note          Append '\0' to a Mibs_DString instance
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         ds/_
// +mdoc #param_note    Pointer to Mibs_DString instance
#define mibs_ds_append_null(alloc, ds) mibs_da_append((alloc), (ds), '\0')

// +mdoc #macro         mibs_join_sa
// +mdoc #note          Join a Mibs_String_Array instance into a Mibs_DString instance
// +mdoc #return_note   Mibs_DString instance; USER OWNS THE MEMORY
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         sa/_
// +mdoc #param_note    Pointer to a Mibs_String_Array instance
// +mdoc #param         delim/_
// +mdoc #param_note    Delimiter character
#define mibs_join_sa(alloc, sa, delim) \
({ \
    Mibs_DString ds = {0}; \
    for (size_t i = 0; i < (sa)->count; i++) { \
        const char* c = (sa)->items[i]; \
        while(*c) mibs_da_append((alloc), &ds, *c++); \
        if (i < sa->count - 1) { \
            mibs_da_append((alloc), &ds, (delim)); \
        } \
    } \
    mibs_ds_append_null((alloc), &ds); \
    ds; \
})

// +mdoc #macro         mibs_ds_from_cstr
// +mdoc #note          Creates a Mibs_DString instance from a C String
// +mdoc #return_note   Mibs_DString instance; USER OWNS THE MEMORY
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         s/_
// +mdoc #param_note    C String
#define mibs_ds_from_cstr(alloc, s) \
({ \
    Mibs_DString ds = {0}; \
    const char* c = (char*)(s); \
    while(*c) mibs_da_append((alloc), &ds, *c++); \
    mibs_ds_append_null((alloc), &ds); \
    ds; \
})

// NOTE: caller has to free the result buffer
// +mdoc #macro         mibs_string_replace
// +mdoc #note          Replace a string in a C String with another C String
// +mdoc #return_note   Pointer to new C String; USER OWNS THE MEMORY
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         orig/_
// +mdoc #param_note    Original string
// +mdoc #param         rep/_
// +mdoc #param_note    Part of the string to be replaced
// +mdoc #param         with/_
// +mdoc #param_note    String to replace with
#define mibs_string_replace(alloc, orig, rep, with) \
({ \
    char* result = 0; \
    char* ins = 0; \
    char* tmp = 0; \
    size_t len_rep = 0; \
    size_t len_with = 0; \
    size_t len_front = 0; \
    size_t count; \
    \
    if (!(orig) || !(rep)) { \
        result = 0; \
    } else { \
        len_rep = strlen((rep)); \
        if (len_rep == 0) { \
            result = 0; \
        } else { \
            if (!(with)) { \
                with = ""; \
            } \
            len_with = strlen((with)); \
            \
            ins = (orig); \
            for (count = 0; tmp = strstr((ins), (rep)); ++count) { \
                ins = tmp + len_rep; \
            } \
            tmp = result = mibs_alloc((alloc), strlen((orig))+(len_with-len_rep)*count+1); \
            while(count--) { \
                ins = strstr((orig), (rep)); \
                len_front = ins-(orig); \
                tmp = strncpy(tmp, orig, len_front) + len_front; \
                tmp = strcpy(tmp, with) + len_with; \
                orig += len_front + len_rep; \
            } \
            strcpy(tmp, orig); \
        } \
    } \
    \
    result; \
})

// +mdoc #macro         mibs_format
// +mdoc #note          Format a string in-place, using a scratch allocator
// +mdoc #variadic
// +mdoc #param         fmt/_
// +mdoc #param_note    Format string
#define mibs_format(fmt, ...) \
({ \
    ssize_t bufsz = snprintf(NULL, 0, fmt, ##__VA_ARGS__); \
    char* buf = mibs_alloc(&mibs_scratch_alloc, bufsz+1); \
    snprintf(buf, bufsz+1, fmt, ##__VA_ARGS__); \
    buf; \
})

// +mdoc #macro         mibs_copy_string_in_place
// +mdoc #note          Copy a C String in-place
// +mdoc #param         s/_
// +mdoc #param_note    C String
#define mibs_copy_string_in_place(s) mibs_format("%s", s)

// +mdoc #enum          Mibs_Log_Lvl
// +mdoc #note          Sets the logging level for a mibs_log() call
// +mdoc #enum_item     MIBS_LL_INFO
// +mdoc #enum_item     MIBS_LL_WARNING
// +mdoc #enum_item     MIBS_LL_ERROR
// +mdoc #enum_item     MIBS_LL_DEBUG
typedef enum {
    MIBS_LL_INFO,
    MIBS_LL_WARNING,
    MIBS_LL_ERROR,
    MIBS_LL_DEBUG,
} Mibs_Log_Lvl;

// +mdoc #macro         mibs_log
// +mdoc #note          Simple and pretty logging utility for Mibs
// +mdoc #variadic
// +mdoc #param         lvl/_
// +mdoc #param_note    Log level - MIBS_LL_INFO, MIBS_LL_WARNING, MIBS_LL_ERROR, MIBS_LL_DEBUG
// +mdoc #param         fmt/_
// +mdoc #param_note    Format String
#define mibs_log(lvl, fmt, ...) fprintf(MIBS_LOG_STREAM, "(%s): " fmt, mibs_array_get(mibs_log_lvl_prefixes, lvl), ##__VA_ARGS__)
extern const char* mibs_log_lvl_prefixes[4];

#if defined(MIBS_PLATFORM_UNIX)
// +mdoc #macro         MIBS_INVALID_PROCESS
// +mdoc #note          Invalid process ( -1 )
// +mdoc #note          Defined if MIBS_PLATFORM_UNIX is defined
#   define MIBS_INVALID_PROCESS -1
typedef int Mibs_Process;
#elif defined(MIBS_PLATFORM_WINDOWS)
// +mdoc #macro         MIBS_INVALID_PROCESS
// +mdoc #note          Invalid process ( INVALID_HANDLE_VALUE )
// +mdoc #note          Defined if MIBS_PLATFORM_WINDOWS is defined
#   define MIBS_INVALID_PROCESS INVALID_HANDLE_VALUE
typedef HANDLE Mibs_Process;
#else
#   error "Unimplemented platform"
#endif

#ifdef MIBS_PLATFORM_WINDOWS
#   define UNIX_TIME_START 0x019DB1DED53E8000
#   define TICKS_PER_SEC   10000000

// +mdoc #function      windows_system_time_to_unix
// +mdoc #note          Convert Windows' FILETIME into an unix timestamp
// +mdoc #note          Declared if MIBS_PLATFORM_WINDOWS is defined
// +mdoc #return        int64_t
// +mdoc #return_note   Unix timestamp
// +mdoc #param         ft/FILETIME
// +mdoc #param_note    <a class="link-info" href="https://learn.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-filetime">FILETIME docs</a>
int64_t windows_system_time_to_unix(FILETIME ft);
// +mdoc #function      windows_get_file_last_write
// +mdoc #note          Check when a file was last written to
// +mdoc #note          Declared if MIBS_PLATFORM_WINDOWS is defined
// +mdoc #return        int64_t
// +mdoc #return_note   Unix timestamp
// +mdoc #param         hFile/HANDLE
// +mdoc #param_note    Handle to the file
int64_t windows_get_file_last_write(HANDLE hFile);
// +mdoc #macro         windows_GetLastError_as_ds
// +mdoc #note          Convert a GetLastError() result into a Mibs_DString instance
// +mdoc #note          Defined if MIBS_PLATFORM_WINDOWS is defined
// +mdoc #return_note   A Mibs_DString instance; USER OWNS THE MEMORY
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
#define windows_GetLastError_as_ds(alloc) \
({ \
    Mibs_DString ds = {0}; \
    DWORD err = GetLastError(); \
    if (err != 0) { \
        \
        LPSTR message_buffer = NULL; \
        FormatMessageA( \
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, \
            NULL, \
            err, \
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), \
            (LPSTR)&message_buffer, \
            0, NULL \
        ); \
        ds = mibs_ds_from_cstr((alloc), message_buffer); \
        LocalFree(message_buffer); \
    } \
    ds; \
})
#endif

// +mdoc #macro         MIBS_CMD_SYNC
// +mdoc #note          Run a Mibs_Cmd instance synchronously
#define MIBS_CMD_SYNC  1
// +mdoc #macro         MIBS_CMD_ASYNC
// +mdoc #note          Run a Mibs_Cmd instance asynchronously
#define MIBS_CMD_ASYNC 0

typedef Mibs_Result(Mibs_Process) Mibs_Cmd_Result;

// +mdoc #macro         make_bad_cmd_result
// +mdoc #funclike
// +mdoc #note          Creates an invalid Mibs_Cmd_Result instance
// +mdoc #note          This macro is for internal usage
// +mdoc #return_note   Mibs_Cmd_Result instance
#define make_bad_cmd_result() ((Mibs_Cmd_Result){ .ok = false, .value = MIBS_INVALID_PROCESS })

// +mdoc #macro         mibs_log_cmd
// +mdoc #note          Log nicely a Mibs_Cmd instance
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         log_lvl/_
// +mdoc #param_note    Log level; See Mibs_Log_Lvl enum
// +mdoc #param         cmd/_
// +mdoc #param_note    Pointer to a Mibs_Cmd instance
// +mdoc #param         cwd/_
// +mdoc #param_note    A C String representing the current working directory
#define mibs_log_cmd(alloc, log_lvl, cmd, cwd) \
    do { \
        Mibs_String_Builder sb = {0}; \
        char* str = mibs_da_concat_str_items((alloc), (cmd), ' '); \
        mibs_sb_append_cstr((alloc), &sb, "command "); \
        mibs_sb_append_char((alloc), &sb, '\''); \
        mibs_sb_append_cstr((alloc), &sb, str); \
        mibs_sb_append_char((alloc), &sb, '\''); \
        mibs_sb_append_cstr((alloc), &sb, " in "); \
        mibs_sb_append_char((alloc), &sb, '\''); \
        mibs_sb_append_cstr((alloc), &sb, cwd); \
        mibs_sb_append_char((alloc), &sb, '\''); \
        mibs_sb_append_null((alloc), &sb); \
        mibs_log(log_lvl, "%s" MIBS_NL, sb.items); \
        mibs_free((alloc), str); \
        mibs_da_deinit((alloc), &sb); \
    } while(0)

typedef Mibs_Da(const char*) Mibs_Cmd;

// +mdoc #macro         _windows_run_cmd_cleanup
// +mdoc #note          Clean up after mibs_run_cmd
// +mdoc #note          Used only if MIBS_PLATFORM_WINDOWS is defined
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
#define _windows_run_cmd_cleanup(alloc) \
    do { \
        for (size_t i = 0; i < tmp.count; i++) { \
            mibs_free((alloc), (void*)tmp.items[i]); \
        } \
        mibs_da_deinit((alloc), &tmp); \
        mibs_da_deinit((alloc), &ds); \
        mibs_free((alloc), commandline); \
    } while(0)

#if defined(MIBS_PLATFORM_UNIX)
// +mdoc #macro         mibs_run_cmd
// +mdoc #note          Execute a command constructed via a Mibs_Cmd instance
// +mdoc #note          Defined if MIBS_PLATFORM_UNIX is defined
// +mdoc #return_note   Mibs_Cmd_Result instance
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointet to allocator
// +mdoc #param         cmd/_
// +mdoc #param_note    Pointer to Mibs_Cmd instance
// +mdoc #param         sync/_
// +mdoc #param_note    MIBS_CMD_SYNC or MIBS_CMD_ASYNC
// +mdoc #param         cwd/_
// +mdoc #param_note    Current working directory for the command; Pass NULL or 0 to run in the current working directory of the program
#define mibs_run_cmd(alloc, cmd, sync, cwd) \
({ \
    Mibs_Process pid = fork(); \
    Mibs_Cmd_Result result = (Mibs_Cmd_Result){ .ok = true, .value = pid };  \
    if (pid < 0) { \
        mibs_log(MIBS_LL_ERROR, "cannot fork" MIBS_NL); \
        result = make_bad_cmd_result(); \
    } else if (pid == 0) { \
        for (size_t i = 0; i < (cmd)->count; i++) { \
            if (mibs_compare_cstr((cmd)->items[i], "")) { \
                mibs_da_remove((cmd), i); \
            } \
        } \
        Mibs_Cmd null_cmd = mibs_da_clone((alloc), Mibs_Cmd, (cmd)); \
        mibs_da_append((alloc), &null_cmd, (char*)NULL); \
        \
        char cwdbuf[PATH_MAX]; \
        getcwd(cwdbuf, sizeof(cwdbuf)); \
        \
        if ((cwd) != NULL) chdir((cwd)); \
        mibs_log_cmd((alloc), MIBS_LL_INFO, (cmd), (cwd) != NULL ? (cwd) : cwdbuf); \
        \
        int execvp_status = execvp((cmd)->items[0], (char* const*)null_cmd.items); \
        if (execvp_status < 0) { \
            result = make_bad_cmd_result(); \
        } \
    } else { \
        if (sync) { \
            while(true) { \
                int status; \
                if (waitpid(pid, &status, 0) < 0) { \
                    mibs_log(MIBS_LL_ERROR, "could not wait for command (%d): %s"MIBS_NL, pid, strerror(errno)); \
                    result = make_bad_cmd_result(); \
                    break; \
                } \
                \
                if (WIFSIGNALED(status)) { \
                    mibs_log(MIBS_LL_ERROR, "command was terminated by %s" MIBS_NL, strsignal(WTERMSIG(status))); \
                    result = make_bad_cmd_result(); \
                    break; \
                } \
                if (WIFEXITED(status)) { \
                    int ec = WEXITSTATUS(status); \
                    if (ec != 0) { \
                        mibs_log(MIBS_LL_ERROR, "failed to run command (%d)" MIBS_NL, ec); \
                        result = make_bad_cmd_result(); \
                    } \
                    break; \
                } \
            } \
        } \
    } \
    result; \
})
#elif defined(MIBS_PLATFORM_WINDOWS)
// +mdoc #macro         mibs_run_cmd
// +mdoc #note          Execute a command constructed via a Mibs_Cmd instance
// +mdoc #note          Defined if MIBS_PLATFORM_WINDOWS is defined
// +mdoc #return_note   Mibs_Cmd_Result instance
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointet to allocator
// +mdoc #param         cmd/_
// +mdoc #param_note    Pointer to Mibs_Cmd instance
// +mdoc #param         sync/_
// +mdoc #param_note    MIBS_CMD_SYNC or MIBS_CMD_ASYNC
// +mdoc #param         cwd/_
// +mdoc #param_note    Current working directory for the command; Pass NULL or 0 to run in the current working directory of the program
#define mibs_run_cmd(alloc, cmd, sync, cwd) \
({ \
    Mibs_Cmd_Result result = {0}; \
    STARTUPINFO si; \
    ZeroMemory(&si, sizeof(si)); \
    si.cb = sizeof(si); \
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE); \
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE); \
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE); \
    si.dwFlags |= STARTF_USESTDHANDLES; \
    \
    PROCESS_INFORMATION pi; \
    ZeroMemory(&pi, sizeof(pi)); \
    \
    if (cwd != NULL) { \
        for (size_t i = 0; ((char*)cwd)[i] != '\0'; i++) { \
            if (((char*)cwd)[i] == '/') ((char*)cwd)[i] = '\\'; \
        } \
    } \
    \
    Mibs_Cmd tmp = {0}; \
    \
    /* TODO: figure out why 0 makes a difference */  \
    for (size_t i = (cmd)->count > 1 ? 0 : 0; i < (cmd)->count; i++) { \
        char* s = (char*)mibs_alloc((alloc), strlen((cmd)->items[i])); \
        mibs_copy_string(s, (cmd)->items[i]); \
        mibs_cmd_append((alloc), &tmp, s); \
    } \
    \
    char* commandline = NULL; \
    if ((cmd)->count > 1) { commandline = mibs_da_concat_str_items((alloc), &tmp, ' '); } \
    \
    char cwdbuf[MAX_PATH]; \
    ZeroMemory(&cwdbuf, sizeof(cwdbuf)); \
    GetCurrentDirectory(MAX_PATH, cwdbuf); \
    mibs_log_cmd((alloc), MIBS_LL_INFO, (cmd), (cwd) != NULL ? (cwd) : cwdbuf); \
    BOOL ok = CreateProcess((LPCSTR)(cmd)->items[0], (LPSTR)commandline, NULL, NULL, TRUE, 0, NULL, (LPCSTR)(cwd), &si, &pi); \
    \
    if (ok) { \
        if (sync) { \
            if (WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_FAILED) { \
                Mibs_DString ds = windows_GetLastError_as_ds((alloc)); \
                mibs_log(MIBS_LL_ERROR, "could not wait for command: %s"MIBS_NL, ds.items); \
                _windows_run_cmd_cleanup((alloc)); \
                result = make_bad_cmd_result(); \
            } else { \
                DWORD exitCode; \
                if (!GetExitCodeProcess(pi.hProcess, &exitCode)) { \
                    Mibs_DString ds = windows_GetLastError_as_ds((alloc)); \
                    mibs_log(MIBS_LL_ERROR, "could not get process error code: %sMIBS_NL", ds.items); \
                    _windows_run_cmd_cleanup((alloc)); \
                    result = make_bad_cmd_result(); \
                } else if (exitCode != 0) { \
                    Mibs_DString ds = windows_GetLastError_as_ds((alloc)); \
                    mibs_log(MIBS_LL_ERROR, "Failed to run command: %s"MIBS_NL, ds.items); \
                    _windows_run_cmd_cleanup((alloc)); \
                    result = make_bad_cmd_result(); \
                } else {\
                    CloseHandle(pi.hProcess); \
                    result = (Mibs_Cmd_Result){ .ok = true, .value = MIBS_INVALID_PROCESS }; \
                } \
            } \
        } else { \
            CloseHandle(pi.hThread); \
            result = (Mibs_Cmd_Result){ .ok = true, .value = pi.hProcess }; \
        } \
    } else { \
        Mibs_DString ds = windows_GetLastError_as_ds((alloc)); \
        mibs_log(MIBS_LL_ERROR, "Failed to create the process: %s"MIBS_NL, ds.items); \
        _windows_run_cmd_cleanup((alloc)); \
        result = make_bad_cmd_result(); \
    } \
    result; \
})
#else
#   error "Unimplemented platform"
#endif
// +mdoc #macro         mibs_cmd_append
// +mdoc #note          Append multiple strings into a Mibs_Cmd instance
// +mdoc #note          Works only for compile-time strings. If you wish to append a string constructed at runtime, just use mibs_da_append()
// +mdoc #variadic
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         cmd/_
// +mdoc #param_note    Pointet to Mibs_Cmd instance
#define mibs_cmd_append(alloc, cmd, ...) \
    mibs_da_append_many((alloc), (cmd), const char*, ##__VA_ARGS__)

#if defined(MIBS_PLATFORM_UNIX)
// +mdoc #macro         MIBS_PATH_SEP
// +mdoc #note          Path separator /
// +mdoc #note          Defined if MIBS_PLATFORM_UNIX is defined
#define MIBS_PATH_SEP "/"
#elif defined(MIBS_PLATFORM_WINDOWS)
// +mdoc #macro         MIBS_PATH_SEP
// +mdoc #note          Path separator \
// +mdoc #note          Defined if MIBS_PLATFORM_WINDOWS is defined
#define MIBS_PATH_SEP "\\"
#else
#error "Unimplemented Platform"
#endif

// +mdoc #enum          Mibs_Fs_Entry_Kind
// +mdoc #note          Kind of an entry encountered while opening a directory
// +mdoc #enum_item     MIBS_FSEK_DIR
// +mdoc #enum_item     MIBS_FSEK_FILE
// +mdoc #enum_item     MIBS_FSEK_LINK
// +mdoc #enum_item     MIBS_FSEK_UNKNOWN
typedef enum {
    MIBS_FSEK_DIR,
    MIBS_FSEK_FILE,
    MIBS_FSEK_LINK,
    MIBS_FSEK_UNKNOWN,
} Mibs_Fs_Entry_Kind;
// +mdoc #macro         mibs_fsek_to_string
// +mdoc #note          Converts a Mibs_Fs_Entry_Kind into a string
// +mdoc #return_note   Human-readable version to Mibs_Fs_Entry_Kind
// +mdoc #param         fsek/_
// +mdoc #param_note    Mibs_Fs_Entry_Kind element
#define mibs_fsek_to_string(fsek) (mibs_fs_entry_kind_names[(fsek)])

// +mdoc #struct        Mibs_Fs_Entry
// +mdoc #note          A File system entry
// +mdoc #struct_field  path/const char*
// +mdoc #struct_field_note has to be freed
// +mdoc #struct_field  kind/Mibs_Fs_Entry_Kind
typedef struct {
    const char* path; // has to be freed
    Mibs_Fs_Entry_Kind kind;
} Mibs_Fs_Entry;

typedef Mibs_Da(Mibs_Fs_Entry) Mibs_Fs_Entries;

typedef Mibs_Result(Mibs_Fs_Entries) Mibs_Open_Dir_Result;

// +mdoc #macro         mibs_fs_entries_free
// +mdoc #note          Free all data allocated by a Mibs_Fs_Entires instance
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         entries/_
// +mdoc #param_note    Pointer to Mibs_Fs_Entires instance
#define mibs_fs_entries_free(alloc, entries) \
do { \
    for (size_t i = 0; i < (entries)->count; i++) { \
        Mibs_Fs_Entry ent = (entries)->items[i]; \
        mibs_free((alloc), (void*)ent.path); \
    } \
    mibs_da_deinit((alloc), entries); \
} while(0)

// TODO: Add example usage to docs
#if defined(MIBS_PLATFORM_UNIX)
// +mdoc #macro         mibs_open_dir
// +mdoc #note          Open a directory
// +mdoc #note          Defined if MIBS_PLATFORM_UNIX is defined
// +mdoc #return_note   Returns a Mibs_Open_Dir_Result
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         dir_path/_
// +mdoc #param_note    C String that represents a path to a directory
#define mibs_open_dir(alloc, dir_path) \
({ \
    Mibs_Open_Dir_Result result = (Mibs_Open_Dir_Result){.ok = true, .value = {0}}; \
    DIR* dir = NULL; \
    if ((dir = opendir(dir_path)) == NULL) { \
        result.ok = false; \
        mibs_log(MIBS_LL_ERROR, "cannot open directory %s: %n"MIBS_NL, dir_path, strerror(errno)); \
    } else { \
        struct dirent* entry = NULL; \
        while((entry = readdir(dir)) != NULL) { \
            Mibs_Fs_Entry_Kind kind = 0; \
            switch(entry->d_type) { \
            case DT_DIR:     kind = MIBS_FSEK_DIR;     break; \
            case DT_REG:     kind = MIBS_FSEK_FILE;    break; \
            case DT_LNK:     kind = MIBS_FSEK_LINK;    break; \
            case DT_UNKNOWN: kind = MIBS_FSEK_UNKNOWN; break; \
            } \
            \
            char* full_name = (char*)mibs_alloc((alloc), PATH_MAX); \
            memset(full_name, 0, PATH_MAX); \
            sprintf(full_name, "%s%c%s", dir_path, MIBS_PATH_SEP[0], entry->d_name); \
            Mibs_Fs_Entry fs_entry = (Mibs_Fs_Entry){ \
                .path = full_name, \
                .kind = kind, \
            }; \
            mibs_da_append((alloc), &result.value, fs_entry); \
        } \
        closedir(dir); \
    } \
    result; \
})

#elif defined(MIBS_PLATFORM_WINDOWS)
// +mdoc #macro         mibs_open_dir
// +mdoc #note          Open a directory
// +mdoc #note          Defined if MIBS_PLATFORM_WINDOWS is defined
// +mdoc #return_note   Returns a Mibs_Open_Dir_Result
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         dir_path/_
// +mdoc #param_note    C String that represents a path to a directory

// https://stackoverflow.com/questions/41404711/how-to-list-files-in-a-directory-using-the-windows-api
// https://learn.microsoft.com/en-us/windows/win32/fileio/file-attribute-constants
// https://learn.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-win32_find_dataa
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findnextfilea
#define mibs_open_dir(alloc, dir_path) \
({ \
    Mibs_Open_Dir_Result result = (Mibs_Open_Dir_Result){.ok = true, .value = {0}}; \
    Mibs_String_Builder tmp_path_sb = {0}; \
    mibs_sb_append_cstr((alloc), &tmp_path_sb, dir_path); \
    mibs_sb_append_cstr((alloc), &tmp_path_sb, "\\*"); \
    \
    WIN32_FIND_DATAA findData; \
    HANDLE hFind = INVALID_HANDLE_VALUE; \
    \
    hFind = FindFirstFileA(tmp_path_sb.items, &findData); \
    if (hFind == INVALID_HANDLE_VALUE) { \
        result.ok = false; \
        mibs_log(MIBS_LL_ERROR, "cannot open directory %s: %n"MIBS_NL, dir_path, strerror(errno)); \
    } else { \
        while(FindNextFileA(hFind, &findData) != 0) { \
            char* full_name = (char*)mibs_alloc((alloc), MAX_PATH); \
            memset(full_name, 0, MAX_PATH); \
            sprintf(full_name, "%s%c%s", dir_path, MIBS_PATH_SEP[0], findData.cFileName); \
            \
            Mibs_Fs_Entry_Kind kind = 0; \
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { \
                kind = MIBS_FSEK_DIR; \
            } else if (findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) { \
                kind = MIBS_FSEK_LINK; \
            } else { \
                kind = MIBS_FSEK_FILE; \
            } \
            \
            Mibs_Fs_Entry fs_entry = (Mibs_Fs_Entry){ \
                .path = full_name, \
                .kind = kind, \
            }; \
            mibs_da_append((alloc), &result.value, fs_entry); \
        } \
        FindClose(hFind); \
    } \
    result; \
})

#else
#error "Unimplemented Platform"
#endif

// +mdoc #macro         mibs_copy_file
// +mdoc #note          Copy a file from src to dest
// +mdoc #return_note   a boolean result; true if operation succeeded, false if didn't
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         src/_
// +mdoc #param_note    File to copy
// +mdoc #param         dest/_
// +mdoc #param_note    Destination
// +mdoc #usage Mibs_Default_Allocator alloc = mibs_make_default_allocator();
// +mdoc #usage 
// +mdoc #usage if (mibs_copy_file(&alloc, "test.c", "test1.c")) {
// +mdoc #usage     printf("OK\n");
// +mdoc #usage }
#define mibs_copy_file(alloc, src, dest) \
({ \
    bool result; \
    FILE* r_stream = fopen(src, "rb"); \
    if (r_stream == NULL) { \
        mibs_log(MIBS_LL_ERROR, "could not open file %s: %s"MIBS_NL, src, strerror(errno)); \
        result = false; \
        goto mibs_copy_file_end; \
    } \
    \
    FILE* w_stream = fopen(dest, "wb"); \
    if (r_stream == NULL) { \
        mibs_log(MIBS_LL_ERROR, "could not open file %s: %s"MIBS_NL, dest, strerror(errno)); \
        result = false; \
        goto mibs_copy_file_end; \
    } \
    \
    fseek(r_stream, 0L, SEEK_END); \
    size_t file_size = ftell(r_stream); \
    fseek(r_stream, 0L, SEEK_SET); \
    \
    char* buffer = (char*)mibs_alloc((alloc), file_size); \
    \
    fread(buffer, file_size, 1, r_stream); \
    fwrite(buffer, file_size, 1, w_stream); \
    \
    mibs_free((alloc), buffer); \
    result = true; \
mibs_copy_file_end: \
    result; \
})

// +mdoc #function      mibs_fs_rename
// +mdoc #note          Rename a specified path
// +mdoc #return        bool
// +mdoc #return_note   boolean that denotes if the operation has succeeded or not
// +mdoc #param         oldpath/const char*
// +mdoc #param         newpath/const char*
// +mdoc #usage if(mibs_fs_rename("some_file.c", "new_file.c")) {
// +mdoc #usage     printf("RENAMED\n");
// +mdoc #usage }
bool mibs_fs_rename(const char* oldpath, const char *newpath);
// +mdoc #function      mibs_get_file_ext
// +mdoc #note          Retrieve a file extension string from a file path
// +mdoc #return        const char*
// +mdoc #return_note   Pointer to file extension
// +mdoc #param         file/const char*
// +mdoc #param_note    String with a file path
const char* mibs_get_file_ext(const char* file);
// +mdoc #function      mibs_create_file
// +mdoc #note          Creates an empty file with specified path
// +mdoc #return        bool
// +mdoc #return_note   boolean that denotes if the operation has succeeded or not
// +mdoc #param         path/const char*
// +mdoc #param_note    path to file
bool mibs_create_file(const char* path);
// +mdoc #function      mibs_file_exists
// +mdoc #note          Check if provided path exists
// +mdoc #return        bool
// +mdoc #return_note   true if exists, false if not
// +mdoc #param         path/const char*
// +mdoc #param_note    Path to check
bool mibs_file_exists(const char* path);
// +mdoc #function      mibs_clear_file
// +mdoc #note          Clear/reset content of a provided file
// +mdoc #return        bool
// +mdoc #return_note   boolean that denotes if the operation has succeeded or not
// +mdoc #param         path/const char*
// +mdoc #param_note    path to file
bool mibs_clear_file(const char* path);
// +mdoc #function      mibs_create_folder_rec
// +mdoc #note          Create a folder from a specified path recursively
// +mdoc #return        bool
// +mdoc #return_note   boolean that denotes if the operation has succeeded or not
// +mdoc #param         path/const char*
// +mdoc #param_note    Path of folder
bool mibs_create_folder_rec(const char* path);
// +mdoc #function      _mibs_remove_folder_rec
// +mdoc #note          Remove folder recursively; Made for internal usage
// +mdoc #return        bool
// +mdoc #return_note   boolean that denotes if the operation has succeeded or not
// +mdoc #param         path/const char*
// +mdoc #param_note    Path of folder
// +mdoc #param         print_info/bool
// +mdoc #param_note    Print path information if true; Used if MIBS_PLATFORM_WINDOWS is defined
bool _mibs_remove_folder_rec(const char* path, bool print_info);
// +mdoc #function      mibs_remove_file
// +mdoc #note          Remove a file at specified path
// +mdoc #return        bool
// +mdoc #return_note   boolean that denotes if the operation has succeeded or not
// +mdoc #param         path/const char*
// +mdoc #param_note    Path of file
bool mibs_remove_file(const char* path);

// +mdoc #macro         mibs_remove_folder_rec
// +mdoc #note          Remove folder recursively
// +mdoc #note          This is a wrapper for _mibs_remove_folder_rec, because C doesn't have default parameters
// +mdoc #return_note   boolean that denotes if the operation has succeeded or not
// +mdoc #param         path/_
// +mdoc #param_note    Path of folder
#define mibs_remove_folder_rec(path) _mibs_remove_folder_rec(path, true)

// +mdoc #macro         mibs_path_exists
// +mdoc #note          Check if provided path exists
// +mdoc #note          Simple wrapper for mibs_file_exists with a more meaningful name
// +mdoc #return_note   true if exists, false if not
// +mdoc #param         _path/_
// +mdoc #param_note    Path to check
#define mibs_path_exists(_path) mibs_file_exists(_path)

#define MIBS_LINE_LEN 0xff
typedef Mibs_Result(Mibs_String_Builder) Mibs_Read_File_Result;

// +mdoc #macro         mibs_read_file
// +mdoc #note          Read a file into a Mibs_String_Builder
// +mdoc #return_note   Returns a Mibs_Read_File_Result instance; USER OWNS THE MEMORY
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         file_path/_
// +mdoc #param_note    Path to file
// +mdoc #usage Mibs_Default_Allocator alloc = mibs_make_default_allocator();
// +mdoc #usage 
// +mdoc #usage Mibs_Read_File_Result rfr = mibs_read_file(&alloc, "test.c");
// +mdoc #usage if (!rfr.ok) { exit(1); }
// +mdoc #usage 
// +mdoc #usage Mibs_String_Builder content = rfr.value
// +mdoc #usage mdefer { mibs_da_deinit(&alloc, &content); }
// +mdoc #usage 
// +mdoc #usage mibs_log(MIBS_LL_INFO, "File content:"MIBS_NL);
// +mdoc #usage printf("%s"MIBS_NL, content.items);
#define mibs_read_file(alloc, file_path) \
    ({ \
        Mibs_Read_File_Result result = {0}; \
        FILE* fp; \
        fp = fopen(file_path, "r"); \
        if (fp == NULL) { \
            mibs_log(MIBS_LL_ERROR, "Failed to read file: %s"MIBS_NL, file_path); \
            result.ok = false; \
        } else { \
            Mibs_String_Builder sb = {0}; \
            char buffer[MIBS_LINE_LEN]; \
            memset(buffer, 0, sizeof(buffer)); \
            while(fgets(buffer, MIBS_LINE_LEN, fp)) { \
                mibs_sb_append_cstr((alloc), &sb, buffer); \
            } \
            mibs_sb_append_null((alloc), &sb); \
            fclose(fp); \
            result.ok = true; \
            result.value = sb; \
        } \
        result; \
    })

// +mdoc #macro         Mibs_RF_Result_Bin
// +mdoc #note          Readable wrapper for creating a Mibs_Result(T) instance
// +mdoc #param         T/_
// +mdoc #param_note    Type passed into Mibs_Result(T)
// +mdoc #usage typedef Mibs_RF_Result_Bin( My_Struct_Type ) My_Struct_Type_Bin_Result
#define Mibs_RF_Result_Bin(T) Mibs_Result(T)

// +mdoc #macro         mibs_read_file_bin
// +mdoc #note          Read a binary file to a Mibs_RF_Result_Bin(T) instance
// +mdoc #return_note   Mibs_RF_Result_Bin(T) instance
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         T/_
// +mdoc #param_note    Result type which contains a Mibs_Da(T)
// +mdoc #param         S/_
// +mdoc #param_note    Type/structure which the file contains
// +mdoc #param         file_path/_
// +mdoc #param_note    Path to file
// +mdoc #usage Mibs_Default_Allocator alloc = mibs_make_default_allocator();
// +mdoc #usage 
// +mdoc #usage typedef struct { int a; float b; } My_Struct;
// +mdoc #usage typedef Mibs_Da(My_Struct) My_Structs;
// +mdoc #usage typedef Mibs_RF_Result_Bin(My_Structs) My_Structs_Result_Bin;   
// +mdoc #usage 
// +mdoc #usage My_Structs_Result_Bin msrb = mibs_read_file_bin(&alloc, My_Structs_Result_Bin, My_Struct, "my_structs.bin");
// +mdoc #usage if (!msrb.ok) { exit(1); }
// +mdoc #usage 
// +mdoc #usage My_Structs my_structs = msrb.value;
// +mdoc #usage mdefer { mibs_da_deinit(&alloc, &my_structs); }
// +mdoc #usage 
// +mdoc #usage // do whatever you'd like with my_structs
#define mibs_read_file_bin(alloc, T, S, file_path) \
    ({ \
        T result = {0}; \
        FILE* fp = fopen(file_path, "rb"); \
        if (fp == NULL) { \
            mibs_log(MIBS_LL_ERROR, "Failed to read file: %s"MIBS_NL, file_path); \
            result.ok = false; \
        } else { \
            struct stat sb; \
            if (stat(file_path, &sb) != -1) { \
                result.ok = true; \
                size_t file_size = sb.st_size; \
                for (size_t i = 0; i < file_size/sizeof(S); i++) { \
                    fseek(fp, sizeof(S)*i, SEEK_SET); \
                    S s; \
                    fread(&s, sizeof(S), 1, fp); \
                    mibs_da_append((alloc), &result.value, s); \
                } \
            } \
            fclose(fp); \
        } \
        result; \
    })

// +mdoc #function      mibs_needs_rebuild
// +mdoc #note          Check against the executable path whether a file needs to be rebuilt 
// +mdoc #return        int
// +mdoc #return_note   true if a file needs to be rebuilt, else false
// +mdoc #param         out/const char*
// +mdoc #param_note    path of the executable
// +mdoc #param         in/const char*
// +mdoc #param_note    file to check
int mibs_needs_rebuild(const char* out, const char* in);

// +mdoc #macro         mibs_needs_rebuild_many
// +mdoc #note          call mibs_needs_rebuild for many files
// +mdoc #variadic
// +mdoc #return_note   true if one of the files needs to be rebuilt, else false
// +mdoc #param         bin/_
// +mdoc #param_note    path of the executable
#define mibs_needs_rebuild_many(bin, ...) \
    ({ \
        bool result = false; \
        const char* files[] = { __VA_ARGS__ }; \
        size_t len = sizeof(files)/sizeof(files[0]); \
        for (size_t i = 0; i < len; i++) { \
            if (mibs_needs_rebuild(files[i], bin)) { \
                result = true; \
            } \
        } \
        result; \
    })

#if defined(MIBS_PLATFORM_UNIX)
// +mdoc #macro         _mibs_rebuild_old_name
// +mdoc #note          Create an path for the previous version of the build script executable
// +mdoc #note          THIS MACRO IS FOR INTERNAL USAGE
// +mdoc #note          example: ./mibs -> ./mibs_old
// +mdoc #note          Defined if MIBS_PLATFORM_UNIX is defined
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         bin/_
// +mdoc #param_note    build script executable path
#define _mibs_rebuild_old_name(alloc, bin) \
    do { \
        mibs_sb_append_cstr((alloc), &sb, (bin)); \
        mibs_sb_append_cstr((alloc), &sb, "_old"); \
        mibs_sb_append_null((alloc), &sb); \
    } while(0)
#endif

// +mdoc #macro         MIBS_REBUILD_ARGS_PRE
// +mdoc #note          List of compiler arguments PREPENDED to the argument list, which is passed to the compiler
// +mdoc #note          Those arguments are used when the build script executable rebuilds itself
// +mdoc #usage #define MIBS_REBUILD_ARGS_PRE "-ggdb", "-O3"
// +mdoc #usage #define MIBS_IMPL
// +mdoc #usage #include "mibs.h"
// +mdoc #usage // We still need to provide -ggdb and -O3 when we bootstrap the build script,
// +mdoc #usage // but after that these arguments will be passed on to the compiler automatically.
// +mdoc #usage // The arguments will apprear in the compiler's commandline like so: cc -ggdb -O3 -o mibs mibs.c
#ifndef MIBS_REBUILD_ARGS_PRE
#   define MIBS_REBUILD_ARGS_PRE ""
#endif

// +mdoc #macro         MIBS_REBUILD_ARGS_POST
// +mdoc #note          List of compiler arguments APPENDED to the argument list, which is passed to the compiler
// +mdoc #note          Those arguments are used when the build script executable rebuilds itself
// +mdoc #usage #define MIBS_REBUILD_ARGS_POST "-lm", "-ldl"
// +mdoc #usage #define MIBS_IMPL
// +mdoc #usage #include "mibs.h"
// +mdoc #usage // We still need to provide -lm and -ldl when we bootstrap the build script,
// +mdoc #usage // but after that these arguments will be passed on to the compiler automatically.
// +mdoc #usage // The arguments will apprear in the compiler's commandline like so: cc -o mibs mibs.c -lm -ldl
#ifndef MIBS_REBUILD_ARGS_POST
#   define MIBS_REBUILD_ARGS_POST ""
#endif

#if defined(MIBS_PLATFORM_UNIX)
// +mdoc #macro         mibs_rebuild
// +mdoc #note          Makes a program/build script rebuild itself upon detecting changes in source without having the user manually compile
// +mdoc #note          WARNING: This feature is Unix exclusive and will not work on Windows
// +mdoc #note          You can still call this macro on Windows, but it will have no effect
// +mdoc #param         alloc/_
// +mdoc #param_note    Pointer to allocator
// +mdoc #param         argc/_
// +mdoc #param_note    Commandline argument count
// +mdoc #param         argv/_
// +mdoc #param_note    Array of commandline arguments
// +mdoc #usage // Bootstrap: cc -o mibs mibs.c
// +mdoc #usage // Run: ./mibs
// +mdoc #usage // Output: HELLO WORLD
// +mdoc #usage 
// +mdoc #usage // Change "HELLO WORLD" to "HELLO MIBS"
// +mdoc #usage // Rerun (will automatically pickup changes and rebuild): ./mibs
// +mdoc #usage // Output: HELLO MIBS
// +mdoc #usage     
// +mdoc #usage     
// +mdoc #usage #define MIBS_IMPL
// +mdoc #usage #include "mibs.h"
// +mdoc #usage 
// +mdoc #usage int main(int argc, char ** argv) {
// +mdoc #usage     
// +mdoc #usage     Mibs_Default_Allocator alloc = mibs_make_default_allocator();
// +mdoc #usage     mibs_rebuild(&alloc, argc, argv);
// +mdoc #usage     
// +mdoc #usage     printf("HELLO WORLD"MIBS_NL);
// +mdoc #usage     
// +mdoc #usage     return 0;
// +mdoc #usage }
#define mibs_rebuild(alloc, argc, argv) \
    do { \
        const char* bin = argv[0]; \
        if (mibs_needs_rebuild(bin, __FILE__)) { \
            Mibs_String_Builder sb = {0}; \
            _mibs_rebuild_old_name((alloc), bin); \
            if (!mibs_fs_rename(bin, sb.items)) exit(1); \
            Mibs_Cmd build_cmd = {0}; \
            mibs_da_append((alloc), &build_cmd, MIBS_CC); \
            mibs_cmd_append((alloc), &build_cmd, MIBS_REBUILD_ARGS_PRE); \
            mibs_da_append_many((alloc), &build_cmd, const char*, "-o", bin); \
            mibs_da_append((alloc), &build_cmd, __FILE__); \
            mibs_cmd_append((alloc), &build_cmd, MIBS_REBUILD_ARGS_POST); \
            Mibs_Cmd_Result build_result = mibs_run_cmd((alloc), &build_cmd, MIBS_CMD_SYNC, NULL); \
            mibs_da_deinit((alloc), &build_cmd); \
            if (!build_result.ok) { \
                mibs_log(MIBS_LL_ERROR, "mibs failed to recompile itself"MIBS_NL); \
                mibs_fs_rename(sb.items, bin); \
                exit(1); \
            } \
            mibs_da_deinit((alloc), &sb); \
            Mibs_Cmd run_cmd = {0}; \
            for (size_t i = 0; i < (size_t)argc; i++) mibs_da_append((alloc), &run_cmd, argv[i]); \
            Mibs_Cmd_Result cmd_result = mibs_run_cmd((alloc), &run_cmd, MIBS_CMD_SYNC, NULL); \
            if (!cmd_result.ok) exit(1); \
            mibs_da_deinit((alloc), &run_cmd); \
            exit(0); \
        } \
    } while(0)
#elif defined(MIBS_PLATFORM_WINDOWS)
#   ifndef MIBS_DEBUG
#       define mibs_rebuild(alloc, argc, argv)
#   else
#       define mibs_rebuild(alloc, argc, argv) mibs_log(MIBS_LL_DEBUG, "mibs_rebuild() is disabled on Windows"MIBS_NL)
#   endif
#else
#   error "Unknown platform"
#endif

// DEFER
//https://github.com/cmhood/c-defer/blob/master/defer.h
// +mdoc #macro         mdefer
// +mdoc #note          Golang-style defer for C. Useful for easy memory and resource management
// +mdoc #usage // Memory management
// +mdoc #usage int *ptr = malloc(sizeof(int));
// +mdoc #usage mdefer {
// +mdoc #usage     free(ptr);
// +mdoc #usage     mibs_log(MIBS_LL_INFO, "Called free(ptr)!"MIBS_NL);
// +mdoc #usage }
// +mdoc #usage 
// +mdoc #usage *ptr = 20;
// +mdoc #usage mibs_log(MIBS_LL_INFO, "*ptr = %d"MIBS_NL, *ptr);
// +mdoc #usage 
// +mdoc #usage // Resource management
// +mdoc #usage 
// +mdoc #usage FILE *f = fopen("file.txt", "r");
// +mdoc #usage if (!f) exit(1);
// +mdoc #usage mdefer { fclose(f); }
// +mdoc #usage 
// +mdoc #usage // Do something with the file ...
// +mdoc #usage 
#define mdefer mdefer__2(__COUNTER__)
#define mdefer__2(X) mdefer__3(X)
#define mdefer__3(X) mdefer__4(defer__id##X)
#define mdefer__4(ID) auto void ID##func(char (*)[]); __attribute__((cleanup(ID##func))) char ID##var[0]; void ID##func(char (*ID##param)[])

// Commandline argument parser
typedef enum {
    MIBS_AT_INT,
    MIBS_AT_STRING,
    MIBS_AT_FLOAT,
} Mibs_Arg_Type;
extern const char* arg_type_strings[3];

typedef union {
    int i;
    const char* s;
    float f;
} Mibs_Arg_Value;

typedef struct {
    const char id;
    const char* description;
    Mibs_Arg_Type expeced_type;
    Mibs_Arg_Type type;
    Mibs_Arg_Value value;
} Mibs_Arg;

Mibs_Arg *mibs_get_arg(Mibs_Arg *args, const char c);
bool mibs_is_integer(const char* x);
bool mibs_is_float(const char* x);
bool mibs_has_fp_dot(const char* x);
void mibs_print_usage(Mibs_Arg *args);
void mibs_parse_args(int argc, char ** argv, Mibs_Arg *args);

#ifdef MIBS_IMPL

const char* mibs_log_lvl_prefixes[] = {"info", "warning", "error", "debug"};

// Mibs allocators

void* mibs_default_alloc(Mibs_Default_Allocator* default_alloc, size_t size) { return MIBS_MALLOC(size); }
void mibs_default_free(Mibs_Default_Allocator* default_alloc, void* mem) { MIBS_FREE(mem); }
void* mibs_default_realloc(Mibs_Default_Allocator* default_alloc, void* mem, size_t prev_size, size_t new_size)
{
    return MIBS_REALLOC(mem, new_size);
}

void *mibs_arena_alloc(Mibs_Arena_Allocator *alloc, size_t size)
{
    if (alloc->underlying.buffer == 0) {
        int prot = PROT_READ | PROT_WRITE;
        int vis = MAP_SHARED | MAP_ANONYMOUS;
        alloc->underlying.buffer = mmap(0, alloc->underlying.capacity, prot, vis, -1, 0);
        alloc->underlying.end = alloc->underlying.buffer;
    }
    void *p = alloc->underlying.end;
    alloc->underlying.end += size;
    return p;
}

void mibs_arena_free(Mibs_Arena_Allocator *alloc, void *mem)
{
}

void *mibs_arena_realloc(Mibs_Arena_Allocator *alloc, void *mem, size_t psz, size_t nsz)
{
    void *new = mibs_arena_alloc(alloc, nsz);
    memcpy(new, mem, psz);
    return new;
}

void mibs_arena_cleanup(Mibs_Arena_Allocator *alloc)
{
    munmap(alloc->underlying.buffer, alloc->underlying.capacity);
}

void mibs_arena_reset(Mibs_Arena_Allocator *alloc)
{
    alloc->underlying.end = alloc->underlying.buffer;
}


Mibs_Scratch_Allocator mibs_scratch_alloc = mibs_make_arena_allocator(MIBS_SCRATCH_ALLOC_SIZE);

MIBS_RTINIT void mibs_rt_init(void)
{

}

MIBS_RTDEINIT void mibs_rt_deinit(void)
{
    mibs_arena_cleanup(&mibs_scratch_alloc);
}


// Map implementation: https://github.com/rxi/map
// Modified by kamkow1 to support mibs allocators

unsigned map_hash(const char *str) {
  unsigned hash = 5381;
  while (*str) {
    hash = ((hash << 5) + hash) ^ *str++;
  }
  return hash;
}


map_node_t *map_newnode(Mibs_Allocator_Simple *alloc, const char *key, void *value, int vsize) {
  map_node_t *node;
  int ksize = strlen(key) + 1;
  int voffset = ksize + ((sizeof(void*) - ksize) % sizeof(void*));
  node = (map_node_t*)mibs_alloc(alloc, sizeof(*node) + voffset + vsize);
  if (!node) return NULL;
  memcpy(node + 1, key, ksize);
  node->hash = map_hash(key);
  node->value = ((char*) (node + 1)) + voffset;
  memcpy(node->value, value, vsize);
  return node;
}


int map_bucketidx(map_base_t *m, unsigned hash) {
  /* If the implementation is changed to allow a non-power-of-2 bucket count,
   * the line below should be changed to use mod instead of AND */
  return hash & (m->nbuckets - 1);
}


void map_addnode(map_base_t *m, map_node_t *node) {
  int n = map_bucketidx(m, node->hash);
  node->next = m->buckets[n];
  m->buckets[n] = node;
}


int map_resize(map_base_t *m, int nbuckets) {
  map_node_t *nodes, *node, *next;
  map_node_t **buckets;
  int i; 
  /* Chain all nodes together */
  nodes = NULL;
  i = m->nbuckets;
  while (i--) {
    node = (m->buckets)[i];
    while (node) {
      next = node->next;
      node->next = nodes;
      nodes = node;
      node = next;
    }
  }
  /* Reset buckets */
  int prev_size = sizeof(*m->buckets)*m->nbuckets;
  int new_size = sizeof(*m->buckets)*nbuckets;
  buckets = (map_node_t**)mibs_realloc(m->alloc, m->buckets, prev_size, new_size);
  if (buckets != NULL) {
    m->buckets = buckets;
    m->nbuckets = nbuckets;
  }
  if (m->buckets) {
    memset(m->buckets, 0, sizeof(*m->buckets) * m->nbuckets);
    /* Re-add nodes to buckets */
    node = nodes;
    while (node) {
      next = node->next;
      map_addnode(m, node);
      node = next;
    }
  }
  /* Return error code if realloc() failed */
  return (buckets == NULL) ? -1 : 0;
}


map_node_t **map_getref(map_base_t *m, const char *key) {
  unsigned hash = map_hash(key);
  map_node_t **next;
  if (m->nbuckets > 0) {
    next = &m->buckets[map_bucketidx(m, hash)];
    while (*next) {
      if ((*next)->hash == hash && !strcmp((char*) (*next + 1), key)) {
        return next;
      }
      next = &(*next)->next;
    }
  }
  return NULL;
}


void map_deinit_(map_base_t *m) {
  map_node_t *next, *node;
  int i;
  i = m->nbuckets;
  while (i--) {
    node = m->buckets[i];
    while (node) {
      next = node->next;
      mibs_free(m->alloc, node);
      node = next;
    }
  }
  mibs_free(m->alloc, m->buckets);
}


void *map_get_(map_base_t *m, const char *key) {
  map_node_t **next = map_getref(m, key);
  return next ? (*next)->value : NULL;
}


int map_set_(map_base_t *m, const char *key, void *value, int vsize) {
  int n, err;
  map_node_t **next, *node;
  /* Find & replace existing node */
  next = map_getref(m, key);
  if (next) {
    memcpy((*next)->value, value, vsize);
    return 0;
  }
  /* Add new node */
  node = map_newnode(m->alloc, key, value, vsize);
  if (node == NULL) goto fail;
  if (m->nnodes >= m->nbuckets) {
    n = (m->nbuckets > 0) ? (m->nbuckets << 1) : 1;
    err = map_resize(m, n);
    if (err) goto fail;
  }
  map_addnode(m, node);
  m->nnodes++;
  return 0;
  fail:
  if (node) mibs_free(m->alloc, node);
  return -1;
}


void map_remove_(map_base_t *m, const char *key) {
  map_node_t *node;
  map_node_t **next = map_getref(m, key);
  if (next) {
    node = *next;
    *next = (*next)->next;
    mibs_free(m->alloc, node);
    m->nnodes--;
  }
}


map_iter_t map_iter_(void) {
  map_iter_t iter;
  iter.bucketidx = -1;
  iter.node = NULL;
  return iter;
}


const char *map_next_(map_base_t *m, map_iter_t *iter) {
  if (iter->node) {
    iter->node = iter->node->next;
    if (iter->node == NULL) goto nextBucket;
  } else {
    nextBucket:
    do {
      if (++iter->bucketidx >= m->nbuckets) {
        return NULL;
      }
      iter->node = m->buckets[iter->bucketidx];
    } while (iter->node == NULL);
  }
  return (char*) (iter->node + 1);
}

bool mibs_fs_rename(const char* oldpath, const char *newpath)
{
    mibs_log(MIBS_LL_INFO, "renaming from \"%s\" to \"%s\""MIBS_NL, oldpath, newpath);
#if defined(MIBS_PLATFORM_UNIX)
    return rename(oldpath, newpath) == 0;
#elif defined(MIBS_PLATFORM_WINDOWS)
    int flags = MOVEFILE_DELAY_UNTIL_REBOOT;
    return MoveFileEx(oldpath, newpath, flags);
#else
#   error "Unknown platform"
#endif
    return true;
}

const char* mibs_get_file_ext(const char* file)
{
    const char* ext = strrchr(file, '.');
    if (!ext) return NULL;
    return ext+1;
}

bool mibs_create_file(const char* path)
{
#if defined(MIBS_PLATFORM_UNIX)
    return mknod(path, S_IFREG | 0666, 0) != 0;
#elif defined(MIBS_PLATFORM_WINDOWS)
    HANDLE hFile = CreateFileA(
        path,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    return hFile != INVALID_HANDLE_VALUE;
#else
#   error "Unknown platform"
#endif
}

bool mibs_file_exists(const char* path)
{
#if defined(MIBS_PLATFORM_UNIX)
    return access(path, F_OK) == 0;
#elif defined(MIBS_PLATFORM_WINDOWS)
    DWORD dwAttrib = GetFileAttributes(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES);
#else
#   error "Unknown platform"
#endif
}

bool mibs_clear_file(const char* path)
{
#if defined(MIBS_PLATFORM_UNIX)
    return fclose(fopen(path, "w")) == 0;
#elif defined(MIBS_PLATFORM_WINDOWS)
    HANDLE hFile = CreateFile(path, GENERIC_WRITE | TRUNCATE_EXISTING, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return false;
    SetFilePointer(hFile, 0, 0, FILE_BEGIN);
    bool res = SetEndOfFile(hFile);
    CloseHandle(hFile);
    return res;
#else
#   error "Unknown platform"
#endif
}

#if defined(MIBS_PLATFORM_UNIX)
bool mibs_create_folder_rec2(const char* path)
{
    char* sep = strchr(path, '/');
    if (sep != NULL) {
        *sep = 0;
        if (!mibs_create_folder_rec(path)) return false;
        *sep = '/';
    }
    if (mkdir(path, 0777) && errno != EEXIST) {
        mibs_log(MIBS_LL_ERROR, "Error while trying to create %s"MIBS_NL, path);
        return false;
    }
    return true;
}
#endif
bool mibs_create_folder_rec(const char* path)
{
#if defined(MIBS_PLATFORM_UNIX)
    mibs_log(MIBS_LL_INFO, "Creating directory %s"MIBS_NL, path);
    bool ok = false;
    char* sep = strrchr(path, '/');
    if (sep) {
        char* path_dup = strdup(path);
        path_dup[sep-path] = 0;
        ok = mibs_create_folder_rec2(path_dup);
        free(path_dup);
    }
    return ok;
#elif defined(MIBS_PLATFORM_WINDOWS)
    TCHAR buffer[4096] = TEXT("");
    TCHAR **lppPart = {NULL};
    GetFullPathName(path, 4096, buffer, lppPart);

    mibs_log(MIBS_LL_INFO, "Creating directory %s"MIBS_NL, buffer);
    int err = SHCreateDirectoryExA(NULL, buffer, NULL);
    return err == ERROR_SUCCESS || err == ERROR_ALREADY_EXISTS;
#else
#   error "Unknown platform"
#endif
}

bool _mibs_remove_folder_rec(const char* dir, bool print_info)
{
#if defined(MIBS_PLATFORM_UNIX)
//  https://stackoverflow.com/questions/2256945/removing-a-non-empty-directory-programmatically-in-c-or-c
    mibs_log(MIBS_LL_INFO, "Removing directory %s"MIBS_NL, dir);
    int ret = 1;
    FTS *ftsp = NULL;
    FTSENT *curr;

    char *files[] = { (char *) dir, NULL };

    ftsp = fts_open(files, FTS_NOCHDIR | FTS_PHYSICAL | FTS_XDEV, NULL);
    if (!ftsp) {
        mibs_log(MIBS_LL_ERROR, "fts_open failed: %s"MIBS_NL, strerror(errno));
        ret = 0;
        goto finish;
    }

    while ((curr = fts_read(ftsp))) {
        switch (curr->fts_info) {
        case FTS_NS:
        case FTS_DNR:
        case FTS_ERR:
            mibs_log(MIBS_LL_ERROR, "fts_read failed %s: %s"MIBS_NL, curr->fts_accpath, strerror(curr->fts_errno));
            break;

        case FTS_DC:
        case FTS_DOT:
        case FTS_NSOK:
            break;
        case FTS_D:
            break;

        case FTS_DP:
        case FTS_F:
        case FTS_SL:
        case FTS_SLNONE:
        case FTS_DEFAULT:
            if (remove(curr->fts_accpath) < 0) {
                mibs_log(MIBS_LL_ERROR, "Failed to remove %s: %s"MIBS_NL, curr->fts_path, strerror(curr->fts_errno));
                ret = 0;
            }
            break;
        }
    }

finish:
    if (ftsp) {
        fts_close(ftsp);
    }

    return ret;
#elif defined(MIBS_PLATFORM_WINDOWS)
    TCHAR buffer[MAX_PATH] = TEXT("");
    TCHAR **lppPart = {NULL};
    GetFullPathName(dir, MAX_PATH, buffer, lppPart);
    if (print_info) mibs_log(MIBS_LL_INFO, "Removing directory %s"MIBS_NL, buffer);

    TCHAR old_dir[MAX_PATH];
    HANDLE hFind;
    WIN32_FIND_DATA find;
    LPTSTR pch;

    GetCurrentDirectory(MAX_PATH, old_dir);
    if (!SetCurrentDirectory(buffer)) {
        return 0;
    }

    hFind = FindFirstFile(TEXT("*"), &find);
    if(hFind == INVALID_HANDLE_VALUE) {
        mibs_log(MIBS_LL_ERROR, "Failed to open %s"MIBS_NL, find.cFileName);
        return false;
    }
    do {
        pch = find.cFileName;
        if (pch[0] == TEXT('.') &&
            (pch[1] == 0 || (pch[1] == TEXT('.') && pch[2] == 0))) {
            continue;
        }
        if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            _mibs_remove_folder_rec(find.cFileName, false);
        } else {
            SetFileAttributes(find.cFileName, FILE_ATTRIBUTE_NORMAL);
            if (!DeleteFile(find.cFileName)) {
                mibs_log(MIBS_LL_ERROR, "DeleteFile: Could not delete %s"MIBS_NL, find.cFileName);
                return false;
            }
        }
    } while(FindNextFile(hFind, &find));
    FindClose(hFind);

    SetCurrentDirectory(old_dir);
    SetFileAttributes(buffer, FILE_ATTRIBUTE_DIRECTORY);

    return RemoveDirectory(buffer);
#else
#   error "Unknown platform"
#endif
}

bool mibs_remove_file(const char* path)
{
    return remove(path) == 0;
}

static const char* mibs_fs_entry_kind_names[] = {
    [MIBS_FSEK_DIR]     = "Directory",
    [MIBS_FSEK_FILE]    = "File",
    [MIBS_FSEK_LINK]    = "Link",
    [MIBS_FSEK_UNKNOWN] = "Unknown"
};

// https://learn.microsoft.com/en-us/windows/win32/sysinfo/retrieving-the-last-write-time
#ifdef MIBS_PLATFORM_WINDOWS

#define UNIX_TIME_START 0x019DB1DED53E8000
#define TICKS_PER_SEC   10000000

int64_t windows_system_time_to_unix(FILETIME ft)
{
    LARGE_INTEGER li;
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    return (li.QuadPart - UNIX_TIME_START) / TICKS_PER_SEC;
}

int64_t windows_get_file_last_write(HANDLE hFile)
{
    FILETIME ftWrite;
    if (!GetFileTime(hFile, NULL, NULL, &ftWrite)) return -1;
    return windows_system_time_to_unix(ftWrite);
}

#endif

// https://learn.microsoft.com/en-us/windows/win32/procthread/creating-processes
int mibs_needs_rebuild(const char* out, const char* in)
{
#if defined(MIBS_PLATFORM_UNIX)
    struct stat statbuf;
    char real_out[PATH_MAX];
    realpath(out, real_out);
    if (stat(real_out, &statbuf) < 0) {
        if (errno == ENOENT) return 1;
        // mibs_log(MIBS_LL_ERROR, "could not stat %s: %s"MIBS_NL, real_out, strerror(errno));
        return -1;
    }
    int out_time = statbuf.st_mtime;
    char real_in[PATH_MAX];
    realpath(in, real_in);
    if (stat(in, &statbuf) < 0) {
        // mibs_log(MIBS_LL_ERROR, "could not stat %s: %s"MIBS_NL, real_in, strerror(errno));
        return -1;
    }
    int in_time = statbuf.st_mtime;
    return in_time > out_time;
#elif defined(MIBS_PLATFORM_WINDOWS)
    int flags = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    HANDLE hFileOut = CreateFile(out, GENERIC_READ, flags, NULL, OPEN_EXISTING, 0, NULL);
    if (hFileOut == INVALID_HANDLE_VALUE) return 1;
    int64_t out_time = windows_get_file_last_write(hFileOut);
    if (out_time == -1) return -1;

    HANDLE hFileIn = CreateFile(in, GENERIC_READ, flags, NULL, OPEN_EXISTING, 0, NULL);
    if (hFileIn == INVALID_HANDLE_VALUE) return -1;
    int64_t in_time = windows_get_file_last_write(hFileIn);
    if (in_time == -1) return -1;

    return in_time > out_time;
#else
#   error "Unimplemented platform"
#endif
}

bool mibs_compare_cstr(const char* lhs, const char* rhs)
{ 
    return strcmp(lhs, rhs) == 0;
}

const char* mibs_arg_type_strings[3] = {"int", "string", "float"};

Mibs_Arg *mibs_get_arg(Mibs_Arg *args, const char c)
{
    Mibs_Arg *arg = 0;
    size_t i = 0;
    while (args[i].id != 0) {
        if (args[i].id == c) {
            arg = &args[i];
        }
        i++;
    }
    return arg;
}

bool mibs_is_integer(const char* x) { int _x; return sscanf(x, "%d", &_x) == 1; }
bool mibs_is_float(const char* x)   { int _x; return sscanf(x, "%f", &_x) == 1; }
bool mibs_has_fp_dot(const char* x) { const char* x2 = x; while(*x2) { if (*x2++ == '.') return 1; } return 0; }

void mibs_print_usage(Mibs_Arg *args)
{
    size_t i = 0;
    while(args[i].id != 0) {
        Mibs_Arg *arg = &args[i];
        printf("Usage:"MIBS_NL);
        printf("    -%c: %s; Expects %s"MIBS_NL, arg->id, arg->description, mibs_arg_type_strings[arg->expeced_type]);
        i++;
    }
}

void mibs_parse_args(int argc, char ** argv, Mibs_Arg *args)
{
    while((argc > 1) && (argv[1][0] == '-')) {
        Mibs_Arg *arg = 0;
        size_t i = 0;
        while(args[i].id != 0) {
            if (args[i].id == argv[1][1]) {
                arg = &args[i];
            }
            i++;
        }

        if (arg == 0) {
            mibs_log(MIBS_LL_ERROR, "Bad argument %s"MIBS_NL, argv[1]);
            exit(1);
        }

        const char* value = &argv[1][2];
        if (mibs_is_integer(value) && !mibs_has_fp_dot(value)) {
            arg->type = MIBS_AT_INT;
            arg->value.i = atoi(value);
        } else if (mibs_is_float(value) && mibs_has_fp_dot(value)) {
            arg->type = MIBS_AT_FLOAT;
            arg->value.f = atof(value);
        } else {
            arg->type = MIBS_AT_STRING;
            arg->value.s = mibs_copy_string_in_place(value);
        }

        if (arg->type != arg->expeced_type) {
            mibs_log(MIBS_LL_ERROR, "-%c expected %s, but got %s"MIBS_NL, arg->id,
                    mibs_arg_type_strings[arg->expeced_type], mibs_arg_type_strings[arg->type]);
            mibs_print_usage(args);
            exit(1);
        }

        ++argv;
        --argc;
    }
}

#endif // MIBS_IMPL

#endif // MIBS_H_
