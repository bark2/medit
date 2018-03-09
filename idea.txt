first program the core api
parse a source file into a data structure, keep in mind for later that the editor should opt for a cpp/c file. other files should be treated raw.
next core implementing the rendering. currently thinking of using glfw to create the window and handle io, and opengl\freetype to render text.
implement all of that generecly so it can easily be translated to console. (think of it as a study project to learn opengl and linux :>, it doesnt have to be portable for now ..)
follow emacs philosophy, seperate to buffers. every buffer is attached to a file.
plugin which enables multiple clients connect and edit files together

remove from the emacs config, it is slower than it should be 
