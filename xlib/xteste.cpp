// g++ xteste.cpp -lX11
#include <iostream>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

int xWindowProcedure(const XEvent& event);

int main()
{
	std::cout << "> Starting XLib Application" << std::endl;

	Display* display = XOpenDisplay(nullptr);

	if(display == nullptr)
	{
		std::cout << "(!) ERROR - Failed to connect to X server: " << XDisplayName(nullptr) << std::endl;
		return -1;
	}

	std::cout << "> Connected to display name: " << XDisplayName(nullptr) << std::endl;

	int screenNumber = DefaultScreen(display);
	Screen* screen = DefaultScreenOfDisplay(display);

	std::cout << "> Screen number: " << screenNumber << std::endl;
	std::cout << "> Screen count: " << ScreenCount(display) << std::endl;

	// XGetGeometry()
	// XGetWindowAttributes()
	std::cout << "> Display width: " << DisplayWidth(display, screenNumber) << std::endl;
	std::cout << "> Display height: " << DisplayHeight(display, screenNumber) << std::endl;

	unsigned int mask =
		StructureNotifyMask |
		VisibilityChangeMask |
		ExposureMask |
		FocusChangeMask |
		KeyPressMask | KeyReleaseMask |
		ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

	XSetWindowAttributes attributes;
	attributes.event_mask = mask;

	Window hWindow = XCreateWindow(
		display,
		DefaultRootWindow(display),
		0, 0,
		640, 480,
		0, // border width
		CopyFromParent, // depth
		InputOutput, // class
		CopyFromParent, // visual
		CWEventMask, // valueMask
		&attributes//attributes
	);

	XStoreName(display, hWindow, "Xlib window test");

	Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, hWindow, &wm_delete, 1);

	// XSelectInput(
	// 	display,
	// 	hWindow,
	// 	mask);

	XMapWindow(display, hWindow);

	bool loop = true;
	XEvent xEvent;

	while(loop)
	{
		//int ret = XNextEvent(display, &xEvent);
		if(XCheckTypedWindowEvent(display, hWindow, ClientMessage, &xEvent))
		{
			if(!strcmp(XGetAtomName(display, xEvent.xclient.message_type), "WM_PROTOCOLS"))
				XDestroyWindow(display, hWindow);
		}

		if(XCheckWindowEvent(display, hWindow, mask, &xEvent))
		{
			if(!xWindowProcedure(xEvent))
				loop = false;
		}
	}

	XCloseDisplay(display);

	std::cout << "> End XLib Application" << std::endl;
	return 0;
}

int xWindowProcedure(const XEvent& event)
{
	switch(event.type)
	{
	/*
		Keyboard events
	*/
	case KeyPress:
		std::cout << " > EVENT (KeyPressMask): KeyPress: " << event.xkey.keycode << " | state: " << event.xkey.state << std::endl;
		break;

	case KeyRelease:
		std::cout << " > EVENT (KeyReleaseMask): KeyRelease: " << event.xkey.keycode << " | state: " << event.xkey.state << std::endl;
		break;

	/*
		Pointer events
	*/
	case ButtonPress:
		std::cout << " > EVENT (ButtonPressMask): ButtonPress: " << event.xbutton.button << " | state: " << event.xbutton.state << std::endl;
		std::cout << "\t - x: " << event.xbutton.x << std::endl;
		std::cout << "\t - y: " << event.xbutton.y << std::endl;
		break;

	case ButtonRelease:
		std::cout << " > EVENT (ButtonReleaseMask): ButtonRelease" << std::endl;
		std::cout << "\t - x: " << event.xbutton.x << std::endl;
		std::cout << "\t - y: " << event.xbutton.y << std::endl;
		break;

	case MotionNotify:
		std::cout << " > EVENT (PointerMotionMask): MotionNotify" << std::endl;
		std::cout << "\t - x: " << event.xmotion.x << std::endl;
		std::cout << "\t - y: " << event.xmotion.y << std::endl;
		break;

	/*
		Window satate notification events
	*/

	case CirculateNotify:
		std::cout << "> EVENT (StructureNotifyMask): CirculateNotify" << std::endl;
		break;

	case ConfigureNotify:
		std::cout << "> EVENT (StructureNotifyMask): ConfigureNotify" << std::endl;
		std::cout << "\t - x: " << event.xconfigure.x << std::endl;
		std::cout << "\t - y: " << event.xconfigure.y << std::endl;
		std::cout << "\t - width: " << event.xconfigure.width << std::endl;
		std::cout << "\t - height: " << event.xconfigure.height << std::endl;
		break;

	case CreateNotify:
		std::cout << "> EVENT (StructureNotifyMask): CreateNotify" << std::endl;
		break;

	case DestroyNotify:
		std::cout << "> EVENT (StructureNotifyMask): DestroyNotify" << std::endl;
		return 0;

	case GravityNotify:
		std::cout << "> EVENT (StructureNotifyMask): GravityNotify" << std::endl;
		break;

	case MapNotify:
		std::cout << "> EVENT (StructureNotifyMask): MapNotify" << std::endl;
		break;

	case MappingNotify:
		std::cout << "> EVENT (StructureNotifyMask): MappingNotify" << std::endl;
		break;

	case ReparentNotify:
		std::cout << "> EVENT (StructureNotifyMask): ReparentNotify" << std::endl;
		break;

	case UnmapNotify:
		std::cout << "> EVENT (StructureNotifyMask): UnmapNotify" << std::endl;
		break;

	case VisibilityNotify:
		std::cout << "> EVENT (VisibilityChangeMask): VisibilityNotify" << std::endl;
		break;

	/*
		Exposure events
	*/

	case Expose:
		std::cout << "> EVENT (ExposureMask): Expose" << std::endl;
		break;

	case GraphicsExpose:
		std::cout << "> EVENT (ExposureMask): GraphicsExpose" << std::endl;
		break;

	case NoExpose:
		std::cout << "> EVENT (ExposureMask): NoExpose" << std::endl;
		break;

	/*
		Input Focus Events
	*/
	case FocusIn:
		std::cout << "> EVENT (FocusChangeMask): FocusIn" << std::endl;
		break;

	case FocusOut:
		std::cout << "> EVENT (FocusChangeMask): FocusOut" << std::endl;
		break;

	/*
		ICCM
	*/
	case ClientMessage:
		std::cout << "> EVENT (ICCM): ClientMessage" << std::endl;
		break;

	
	default:
		std::cout << "> EVENT (default): type: " << event.type << std::endl;
	}

	return 1;
}