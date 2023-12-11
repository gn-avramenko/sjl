#include "wincommon.h"

using namespace std;

void show_last_error();

HMODULE hJVM = NULL;
JavaVMInitArgs g_sJavaVMInitArgs;
typedef jint (JNICALL CreateJavaVM_t)(JavaVM **pvm, void **env, void *args);
JavaVM* jvm = NULL;
JavaVM* g_pJavaVM = NULL;
JNIEnv* g_pJNIEnv = NULL;
HWND hWnd;
DWORD dwExitCode = 0;
jclass obj;

void LoadJVMLibrary();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPSTR lpCmdLine, int nCmdShow) {
	if (!prepare(lpCmdLine)) {
		show_last_error();
		return 1;
	}
	debug(L"Can launch app");
	try {
		hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, "STATIC", "",
		WS_POPUP | SS_BITMAP, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL,
				hInstance, NULL);
//		do {
//
//		} while (dwExitCode == RESTART_EXIT_CODE);
		LoadJVMLibrary();

		close_log_file_handle();
		return 0;
	} catch (...) {
		debug(L"error occurred");
		close_log_file_handle();
		show_last_error();
		return 1;
	}

}

void show_last_error() {
	wstring msg = get_error_message();
	MessageBoxW(NULL, msg.empty() ? L"Unknown error" : msg.c_str(),
			L"Error occurred", MB_OK);
}

void LoadJVMLibrary()
{

//  wstring binDir = get_jvm_path() + L"\\bin";
	wstring binDir = L"c:\\IdeaProjects\\sjl\\examples\\win-gui\\dist\\l4j\\jre1.8.0_201\\bin";
//  wstring dllName = binDir + L"\\server\\jvm.dll";
//  wstring dllName = get_jvm_path() + L"\\lib\\jvm.lib";
   wstring dllName = L"c:\\IdeaProjects\\sjl\\examples\\win-gui\\dist\\l4j\\jre1.8.0_201\\bin\\client\\jvm.dll";
  // Sometimes the parent process may call SetDllDirectory to change its own context, and this will be inherited by the
  // launcher. In that case, we won't be able to load the libraries from the current directory that is set below. So, to
  // fix such cases, we have to reset the DllDirectory to restore the default DLL loading order.
  SetDllDirectoryW(nullptr);

  // Call SetCurrentDirectory to allow jvm.dll to load the corresponding runtime libraries.
  SetCurrentDirectoryW(binDir.c_str());



  hJVM = LoadLibraryW(dllName.c_str());
    if (hJVM)
    {
    	CreateJavaVM_t	*pfnCreateJavaVM;
    	pfnCreateJavaVM = (CreateJavaVM_t *)GetProcAddress(hJVM, "JNI_CreateJavaVM");

    		if (pfnCreateJavaVM == NULL)
    		{
    			return;
    		}

    		JavaVM *jvm;       /* denotes a Java VM */
    		    JNIEnv *env;       /* pointer to native method interface */
    		    JavaVMInitArgs vm_args; /* JDK/JRE 6 VM initialization arguments */
    		    JavaVMOption* options = new JavaVMOption[1];
    		    options[0].optionString = "-Djava.class.path=/usr/lib/java";
    		    vm_args.version = JNI_VERSION_1_6;
    		    vm_args.nOptions = 1;
    		    vm_args.options = options;
    		    vm_args.ignoreUnrecognized = false;
    		/* Start the VM */
    		int iRetVal = pfnCreateJavaVM(&jvm, (void **)&env, &vm_args);

    		if (iRetVal != 0)
    		{
    			return ;
    		}


//    	FARPROC proc = GetProcAddress(hJVM, "JNI_CreateJavaVM");
//    	pCreateJavaVM = reinterpret_cast<JNI_createJavaVM>(proc);
//    	if(pCreateJavaVM){
//    		debug(L"got address 2");
//    		JavaVM *jvm;       /* denotes a Java VM */
//
//    		    	    JavaVMInitArgs initArgs;
//    		    	     initArgs.version = JNI_VERSION_1_2;
//    		    	     initArgs.options = NULL;
//    		    	     initArgs.nOptions = 0;
//    		    	     initArgs.ignoreUnrecognized = JNI_FALSE;
//
//    		    	     JNIEnv* jenv = NULL;
//    		    	     int result = pCreateJavaVM(&jvm, &jenv, &initArgs);
//
//    		    	    /* load and initialize a Java VM, return a JNI interface
//    		    	     * pointer in env */
//    		    	    debug(L"created");
//    		    	    if(result != JNI_OK){
//    		    	    	debug(L"not OK");
//    		    	    }
//    		    	    /* invoke the Main.test method using the JNI */
//    		//    	    jclass cls = env->FindClass("Main");
//    		//    	    jmethodID mid = env->GetStaticMethodID(cls, "test", "(I)V");
//    		//    	    env->CallStaticVoidMethod(cls, mid, 100);
//    		    	    /* We are done. */
//    		    	    jvm->DestroyJavaVM();
//    		    	    debug(L"destroyed");
//    	}
    	//JNI_CreateJavaVM(NULL, NULL, NULL)
//    	JavaVM *jvm;       /* denotes a Java VM */
//    	    JNIEnv *env;       /* pointer to native method interface */
//    	    JavaVMInitArgs vm_args; /* JDK/JRE 6 VM initialization arguments */
//    	    JavaVMOption* options = new JavaVMOption[1];
//    	    vm_args.version = JNI_VERSION_10;
//    	    vm_args.nOptions = 1;
//    	    vm_args.options = options;
//    	    vm_args.ignoreUnrecognized = false;
//    	    /* load and initialize a Java VM, return a JNI interface
//    	     * pointer in env */
//    	    int res  = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
//    	    debug(L"created");
//    	    if(res != JNI_OK){
//    	    	debug(L"not OK");
//    	    }
//    	    delete options;
//    	    /* invoke the Main.test method using the JNI */
////    	    jclass cls = env->FindClass("Main");
////    	    jmethodID mid = env->GetStaticMethodID(cls, "test", "(I)V");
////    	    env->CallStaticVoidMethod(cls, mid, 100);
//    	    /* We are done. */
//    	    jvm->DestroyJavaVM();
//    	    debug(L"destroyed");
    } else {
    	throw_exception(L"unable to create jvm");
    }

}
