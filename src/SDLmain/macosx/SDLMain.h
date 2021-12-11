/*   SDLMain.m - main entry point for our Cocoa-ized SDL app
       Initial Version: Darrell Walisser <dwaliss1@purdue.edu>
       Non-NIB-Code & other changes: Max Horn <max@quendi.de>

    Feel free to customize this file to suit your needs
*/

#ifndef _SDLMain_h_
#define _SDLMain_h_

#import <Cocoa/Cocoa.h>

/* Note that the following defines have not been changed in a long time and
   it's likely going to need fixes if you try it. */

/* Use this flag to determine whether we use SDLMain.nib or not */
#define		SDL_USE_NIB_FILE	0

/* Use this flag to determine whether we use CPS (docking) or not */
#define		SDL_USE_CPS		1

@interface SDLMain : NSObject<NSApplicationDelegate>
- (NSApplicationTerminateReply) applicationShouldTerminate:(NSApplication *)sender;
- (void) setupWorkingDirectory:(BOOL)shouldChdir;
- (BOOL) application:(NSApplication *)theApplication openFile:(NSString *)filename;
- (void) applicationDidFinishLaunching: (NSNotification *) note;
#if SDL_USE_NIB_FILE
- (void)fixMenu:(NSMenu *)aMenu withAppName:(NSString *)appName;
#endif
@end

#endif /* _SDLMain_h_ */
