If you want to create your own "injector", just follow these steps below

1. Inject the compiled hosting dll into the process
2. External: CreateRemoteThread on export 'RunExecutable' from hosted dll with appropriate function arguments
3. Internal: Just call export 'RunExecutable' directly

If you want to do it the easy way
1. Download ExtremeInjector 3.7.2
2. Add the hosting dll to the list, check if (if its not checked)
3. Press advanced options on the hosting dll in the dll list
4. For the export routine, pick the one with the mangled name called something like RunExecutable
5. Calling convention is StdCall
6. And here setup the correct parameters (you can see info about this at the end of the readme)

Example "appropriate" function arguments
*    runtime_version: the runtime version you want to use, i've only tried with "5.0.4" on my pc
*    net_file_directory: path do directory where your .net dll is, excluding the name of the .net dll
*    net_file_name: the name of the .net dll
*    net_namespace_name: full namespace of name where your entrypoint is
*    net_class_name: class name where your entrypoint is
*    net_method_name: function name of your entrypoint

*    (OPTIONAL) unlink_after_execution: placebo, will unlink the loaded .net module from peb (or atleast try to), value should be 1 or 0

Here is an example video from an older revision of this
https://vimeo.com/562119936