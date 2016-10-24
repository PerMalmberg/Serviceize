# Serviceize
A small framework for creating a Windows service with debugging possibilities via alternate call chain as a console application.

#Why
Like (Daemonize)[https://github.com/PerMalmberg/Daemonize], I wrote this library because I was interested in how a Windows service works. 
I also wanted a framework that makes it possible to run an application both as a Windows service and a console application (for debugging purposes) without having
to write two separate implementations.

#What
Serviceize is a framework consisting of a main class, ```Application``` and a two helpers; ```ServiceController``` and ```Process```. 

Using this framework you get:
* Ready-to-use Windows service handing (install, uninstall, start, stop), able to be compiled both with unicode and multi-byte support.
* Ability to run your application as a console application which makes it easier to debug (no need to attach to process). All you loose is the pause/continue ability a service can do.
* Signal handling (CTRL-C and CTRL-Break) for console mode.

#How
As shown in the tests, you inherit from ```Application``` and implement the methods ```RunAsService()``` and ```RunAsConsole()```, is desired. 

When started from Windows Service Control Manager (SCM), in your ```main()``` you do:

    TestApp app( argc, argv );
	TestApp::RunService( app ) ? 0 : 1;

or when you want to run as a console application:
	
    TestApp app( argc, argv );
    result = TestApp::RunConsole( app, argc, argv );

You'll have to decide how your application determines if it is running as a service or a console application, for example install the service with a command line argument indicating the mode.
	
You might also want to add commands to install/uninstall commands to your application, the class ```ServiceControler``` will help you with that.

This framework does not provide the tools to handle threading for your service routine so you'll have to handle that on your own.