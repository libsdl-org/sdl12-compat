/*
    SDL_main.c, placed in the public domain by Sam Lantinga  4/13/98

    The WinMain function -- calls your program's main() function
*/

#include <stdio.h>
#include <stdlib.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#define DIR_SEPERATOR "/"

/* Include the SDL main definition header */
#include "SDL.h"
#include "SDL_main.h"

#ifdef main
#  undef main
#endif /* main */

/* The standard output files */
#define STDOUT_FILE	"stdout.txt"
#define STDERR_FILE	"stderr.txt"

/* Set a variable to tell if the stdio redirect has been enabled. */
static int stdioRedirectEnabled = 0;

static char stdoutPath[MAX_PATH];
static char stderrPath[MAX_PATH];

static void UnEscapeQuotes( char *arg )
{
	char *last = NULL;

	while( *arg ) {
		if( *arg == '"' && *last == '\\' ) {
			char *c_curr = arg;
			char *c_last = last;

			while( *c_curr ) {
				*c_last = *c_curr;
				c_last = c_curr;
				c_curr++;
			}
			*c_last = '\0';
		}
		last = arg;
		arg++;
	}
}

/* Parse a command line buffer into arguments */
static int ParseCommandLine(char *cmdline, char **argv)
{
	char *bufp;
	char *lastp = NULL;
	int argc, last_argc;

	argc = last_argc = 0;
	for ( bufp = cmdline; *bufp; ) {
		/* Skip leading whitespace */
		while ( isspace(*bufp) ) {
			++bufp;
		}
		/* Skip over argument */
		if ( *bufp == '"' ) {
			++bufp;
			if ( *bufp ) {
				if ( argv ) {
					argv[argc] = bufp;
				}
				++argc;
			}
			/* Skip over word */
			while ( *bufp && ( *bufp != '"' || (lastp && *lastp == '\\') ) ) {
				lastp = bufp;
				++bufp;
			}
		} else {
			if ( *bufp ) {
				if ( argv ) {
					argv[argc] = bufp;
				}
				++argc;
			}
			/* Skip over word */
			while ( *bufp && ! isspace(*bufp) ) {
				++bufp;
			}
		}
		if ( *bufp ) {
			if ( argv ) {
				*bufp = '\0';
			}
			++bufp;
		}

		/* Strip out \ from \" sequences */
		if( argv && last_argc != argc ) {
			UnEscapeQuotes( argv[last_argc] );	
		}
		last_argc = argc;	
	}
	if ( argv ) {
		argv[argc] = NULL;
	}
	return(argc);
}

/* Show an error message */
static void ShowError(const char *title, const char *message)
{
/* If USE_MESSAGEBOX is defined, you need to link with user32.lib */
#ifdef USE_MESSAGEBOX
	MessageBox(NULL, message, title, MB_ICONEXCLAMATION|MB_OK);
#else
	fprintf(stderr, "%s: %s\n", title, message);
#endif
}

/* Pop up an out of memory message, returns to Windows */
static BOOL OutOfMemory(void)
{
	ShowError("Fatal Error", "Out of memory - aborting");
	return FALSE;
}

/* SDL_Quit() shouldn't be used with atexit() directly because
   calling conventions may differ... */
static void cleanup(void)
{
	SDL_Quit();
}

/* Remove the output files if there was no output written */
static void cleanup_output(void) {
	FILE *file;
	int empty;

	/* Flush the output in case anything is queued */
	fclose(stdout);
	fclose(stderr);

	/* Without redirection we're done */
	if (!stdioRedirectEnabled) {
		return;
	}

	/* See if the files have any output in them */
	if ( stdoutPath[0] ) {
		file = fopen(stdoutPath, "rb");
		if ( file ) {
			empty = (fgetc(file) == EOF) ? 1 : 0;
			fclose(file);
			if ( empty ) {
				remove(stdoutPath);
			}
		}
	}
	if ( stderrPath[0] ) {
		file = fopen(stderrPath, "rb");
		if ( file ) {
			empty = (fgetc(file) == EOF) ? 1 : 0;
			fclose(file);
			if ( empty ) {
				remove(stderrPath);
			}
		}
	}
}

/* Redirect the output (stdout and stderr) to a file */
static void redirect_output(void)
{
	DWORD pathlen;
	char path[MAX_PATH];
	FILE *newfp;

	pathlen = GetModuleFileName(NULL, path, SDL_arraysize(path));
	while ( pathlen > 0 && path[pathlen] != '\\' ) {
		--pathlen;
	}
	path[pathlen] = '\0';

	SDL_strlcpy( stdoutPath, path, SDL_arraysize(stdoutPath) );
	SDL_strlcat( stdoutPath, DIR_SEPERATOR STDOUT_FILE, SDL_arraysize(stdoutPath) );

	/* Redirect standard input and standard output */
	newfp = freopen(stdoutPath, "w", stdout);

	if ( newfp == NULL ) {	/* This happens on NT */
#if !defined(stdout)
		stdout = fopen(stdoutPath, "w");
#else
		newfp = fopen(stdoutPath, "w");
		if ( newfp ) {
			*stdout = *newfp;
		}
#endif
	}

	SDL_strlcpy( stderrPath, path, SDL_arraysize(stderrPath) );
	SDL_strlcat( stderrPath, DIR_SEPERATOR STDERR_FILE, SDL_arraysize(stderrPath) );

	newfp = freopen(stderrPath, "w", stderr);
	if ( newfp == NULL ) {	/* This happens on NT */
#if !defined(stderr)
		stderr = fopen(stderrPath, "w");
#else
		newfp = fopen(stderrPath, "w");
		if ( newfp ) {
			*stderr = *newfp;
		}
#endif
	}

	setvbuf(stdout, NULL, _IOLBF, BUFSIZ);	/* Line buffered */
	setbuf(stderr, NULL);			/* No buffering */
	stdioRedirectEnabled = 1;
}

#if defined(_MSC_VER)
/* The VC++ compiler needs main defined */
#define console_main main
#endif

/* This is where execution begins [console apps] */
int console_main(int argc, char *argv[])
{
	size_t n;
	char *bufp, *appname;
	int status;

	/* Get the class name from argv[0] */
	appname = argv[0];
	if ( (bufp=SDL_strrchr(argv[0], '\\')) != NULL ) {
		appname = bufp+1;
	} else
	if ( (bufp=SDL_strrchr(argv[0], '/')) != NULL ) {
		appname = bufp+1;
	}

	if ( (bufp=SDL_strrchr(appname, '.')) == NULL )
		n = SDL_strlen(appname);
	else
		n = (bufp-appname);

	bufp = SDL_stack_alloc(char, n+1);
	if ( bufp == NULL ) {
		return OutOfMemory();
	}
	SDL_strlcpy(bufp, appname, n+1);
	appname = bufp;

	/* Load SDL dynamic link library */
	if ( SDL_Init(SDL_INIT_NOPARACHUTE) < 0 ) {
		ShowError("WinMain() error", SDL_GetError());
		return(FALSE);
	}
	atexit(cleanup_output);
	atexit(cleanup);

	/* Sam:
	   We still need to pass in the application handle so that
	   DirectInput will initialize properly when SDL_RegisterApp()
	   is called later in the video initialization.
	 */
	SDL_SetModuleHandle(GetModuleHandle(NULL));

	/* Run the application main() code */
	status = SDL_main(argc, argv);

	/* Exit cleanly, calling atexit() functions */
	exit(status);

	/* Hush little compiler, don't you cry... */
	return 0;
}

/* This is where execution begins [windowed apps] */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
	HMODULE handle;
	char **argv;
	int argc;
	char *cmdline;
	char *env_str;
	char *bufp;
	size_t nLen;

	/* Start up DDHELP.EXE before opening any files, so DDHELP doesn't
	   keep them open.  This is a hack.. hopefully it will be fixed 
	   someday.  DDHELP.EXE starts up the first time DDRAW.DLL is loaded.
	 */
	handle = LoadLibrary(TEXT("DDRAW.DLL"));
	if ( handle != NULL ) {
		FreeLibrary(handle);
	}

	/* Check for stdio redirect settings and do the redirection */
	if ((env_str = SDL_getenv("SDL_STDIO_REDIRECT"))) {
		if (SDL_atoi(env_str)) {
			redirect_output();
		}
	}
#ifndef NO_STDIO_REDIRECT
	else {
		redirect_output();
	}
#endif

	/* Grab the command line */
	bufp = GetCommandLine();
	nLen = SDL_strlen(bufp)+1;
	cmdline = SDL_stack_alloc(char, nLen);
	if ( cmdline == NULL ) {
		return OutOfMemory();
	}
	SDL_strlcpy(cmdline, bufp, nLen);

	/* Parse it into argv and argc */
	argc = ParseCommandLine(cmdline, NULL);
	argv = SDL_stack_alloc(char*, argc+1);
	if ( argv == NULL ) {
		return OutOfMemory();
	}
	ParseCommandLine(cmdline, argv);

	/* Run the main program (after a little SDL initialization) */
	console_main(argc, argv);

	/* Hush little compiler, don't you cry... */
	return 0;
}
