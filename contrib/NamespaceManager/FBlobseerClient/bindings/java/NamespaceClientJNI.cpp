#include "NamespaceClientJNI.h"
#include "FBlobseerClient/NamespaceClient.hpp"

#include "common/debug.hpp"

void __attribute__ ((constructor)) ns_handlerJNI_init() {
}
void __attribute__ ((destructor)) ns_handlerJNI_fini() {
}

JNIEXPORT jlong JNICALL Java_fblobseer_NamespaceClient_ns_1handler_1init(JNIEnv *env, jclass c, jstring jcfg) {
    const char *cfg = env->GetStringUTFChars(jcfg, NULL);
    std::string cfg_file(cfg);
    env->ReleaseStringUTFChars(jcfg, cfg);

    NamespaceClient* nclient = new NamespaceClient(cfg_file);
    return (jlong)nclient;
}

JNIEXPORT jboolean JNICALL Java_fblobseer_NamespaceClient_ns_1handler_1finalize(JNIEnv *env, jclass c, jlong ns) {
	if (ns == 0)
		return false;
	delete (NamespaceClient*)ns;
	return true;
}

JNIEXPORT jint JNICALL Java_fblobseer_NamespaceClient_ns_1handler_1mkdir(JNIEnv *env, jclass c, jlong ns, jstring path) {
    const char *str = env->GetStringUTFChars(path, NULL);
    std::string p(str);
    env->ReleaseStringUTFChars(path, str);

    NamespaceClient* nclient = (NamespaceClient*)ns;
    return nclient->mkdir(p);
}

JNIEXPORT jobject JNICALL Java_fblobseer_NamespaceClient_ns_1handler_1exists(JNIEnv *env, jclass c, jlong ns, jstring path) {
    const char *str = env->GetStringUTFChars(path, NULL);
    std::string p(str);
    bool res;
    int errorCode = 0;
    jobject result;
    env->ReleaseStringUTFChars(path, str);

    // create a JAVA object that contains the status
    // search for the ResultValues java class
    jclass classResultValues = env->FindClass("fblobseer/ResultValues");
    if (classResultValues == NULL) {
    	ERROR("Cannot find class: 'fblobseer/ResultValues'");
    	return false;
    }
    
    // search for the ResultValues method in the ResultValues class
    jmethodID resConstructor = env->GetMethodID(classResultValues, "<init>", "(ZI)V");
    if (resConstructor == NULL) {
    	ERROR("Cannot load method: 'fblobseer.ResultValues.ResultValues', signature: '(ZI)V'");
    	return false;
    }
    
    NamespaceClient* nclient = (NamespaceClient*)ns;
    errorCode = nclient->exists(p, res);
    
    /* Construct a java.lang.String object */
    result = env->NewObject(classResultValues, resConstructor, res, errorCode);    
    
    return result;
}

JNIEXPORT jobject JNICALL Java_fblobseer_NamespaceClient_ns_1handler_1isfile(JNIEnv *env, jclass c, jlong ns, jstring path) {
    const char *str = env->GetStringUTFChars(path, NULL);
    std::string p(str);
    bool res;
    int errorCode = 0;
    jobject result;
    env->ReleaseStringUTFChars(path, str);

    // create a JAVA object that contains the status
    // search for the ResultValues java class
    jclass classResultValues = env->FindClass("fblobseer/ResultValues");
    if (classResultValues == NULL) {
    	ERROR("Cannot find class: 'fblobseer/ResultValues'");
    	return false;
    }
    
    // search for the ResultValues method in the ResultValues class
    jmethodID resConstructor = env->GetMethodID(classResultValues, "<init>", "(ZI)V");
    if (resConstructor == NULL) {
    	ERROR("Cannot load method: 'fblobseer.ResultValues.ResultValues', signature: '(ZI)V'");
    	return false;
    }
    
    NamespaceClient* nclient = (NamespaceClient*)ns;
    errorCode = nclient->isFile(p, res);
    
    /* Construct a java.lang.String object */
    result = env->NewObject(classResultValues, resConstructor, res, errorCode);    
    
    return result;
    

}

JNIEXPORT jint JNICALL Java_fblobseer_NamespaceClient_ns_1handler_1rename(JNIEnv *env, jclass c, jlong ns, jstring path, jstring newPath) {
    const char *str1 = env->GetStringUTFChars(path, NULL);
    std::string p1(str1);
    env->ReleaseStringUTFChars(path, str1);

    const char *str2 = env->GetStringUTFChars(newPath, NULL);
    std::string p2(str2);
    env->ReleaseStringUTFChars(newPath, str2);

    NamespaceClient* nclient = (NamespaceClient*)ns;
    return nclient->rename(p1,p2);
}

JNIEXPORT jint JNICALL Java_fblobseer_NamespaceClient_ns_1handler_1delete(JNIEnv *env, jclass c, jlong ns, jstring path) {
    const char *str = env->GetStringUTFChars(path, NULL);
    std::string p(str);
    env->ReleaseStringUTFChars(path, str);

    NamespaceClient* nclient = (NamespaceClient*)ns;
    return nclient->deleteFile(p);
}


JNIEXPORT jboolean JNICALL Java_fblobseer_NamespaceClient_ns_1handler_1status(JNIEnv *env, jclass c, jlong ns, jstring path, jobject fileMetadata) {
    const char *str = env->GetStringUTFChars(path, NULL);
    std::string p(str);
    env->ReleaseStringUTFChars(path, str);

    FileMetadata fm;

    // get the status of the file
    NamespaceClient* nclient = (NamespaceClient*)ns;
    nclient->status(p, fm);

    // create a JAVA object that contains the status
    // search for the FileMetadata java class
    jclass classFileMetadata = env->FindClass("fblobseer/FileMetadata");
    if (classFileMetadata == NULL) {
    	ERROR("Cannot find class: 'fblobseer/FileMetadata'");
    	return false;
    }

    // search for the setPath method in the FileMetadata class
    jmethodID fmSetPath = env->GetMethodID(classFileMetadata, "setPath", "(Ljava/lang/String;)V");
    if (fmSetPath == NULL) {
    	ERROR("Cannot load method: 'fblobseer.FileMetadata.setPath', signature: '(Ljava/lang/String;)V'");
    	return false;
    }

    // search for the setBlobId method in the FileMetadata class
    jmethodID fmSetBlobId = env->GetMethodID(classFileMetadata, "setBlobId", "(J)V");
    if (fmSetBlobId == NULL) {
    	ERROR("Cannot load method: 'fblobseer.FileMetadata.setBlobId', signature: '(J)V'");
    	return false;
    }

    // search for the setType method in the FileMetadata class
    jmethodID fmSetType = env->GetMethodID(classFileMetadata, "setType", "(I)V");
    if (fmSetType == NULL) {
    	ERROR("Cannot load method: 'fblobseer.FileMetadata.setType', signature: '(I)V'");
    	return false;
    }

    env->CallBooleanMethod(fileMetadata, fmSetPath, env->NewStringUTF(fm.getPath().c_str()));
    env->CallBooleanMethod(fileMetadata, fmSetBlobId, fm.getBlobId());
    env->CallBooleanMethod(fileMetadata, fmSetType, fm.getType());

    return true;
}

JNIEXPORT jboolean JNICALL Java_fblobseer_NamespaceClient_ns_1handler_1listdir(JNIEnv *env, jclass c, jlong ns, jstring path, jobject fmList) {
	const char *str = env->GetStringUTFChars(path, NULL);
	std::string p(str);
	env->ReleaseStringUTFChars(path, str);

	std::vector<FileMetadata> list;

	// call the list directory method
	NamespaceClient* nclient = (NamespaceClient*)ns;
	nclient->listDir(p, list);

	// search for the FileMetadata java class
	jclass classFileMetadata = env->FindClass("fblobseer/FileMetadata");
	if (classFileMetadata == NULL) {
		ERROR("Cannot find class: 'fblobseer/FileMetadata'");
		return false;
	}

	// search for the constructor in the FileMetadata class
	jmethodID fmConstructor = env->GetMethodID(classFileMetadata, "<init>", "(Ljava/lang/String;IJ)V");
	if (fmConstructor == NULL) {
		ERROR("Cannot load constructor: 'fblobseer.FileMetadata.FileMetadata', signature: '(Ljava/lang/String;IJ)V'");
		return false;
	}

	// search for the JAVA Vector class, its constructor and its add method
    jclass classVec = env->FindClass("java/util/Vector");
    if (classVec == NULL) {
    	ERROR("Cannot find class: 'java.util.Vector'");
    	return false;
    }

    jmethodID vecAdd = env->GetMethodID(classVec, "add", "(Ljava/lang/Object;)Z");
    if (vecAdd == NULL) {
    	ERROR("Cannot load method: 'java.util.Vector:add', signature: '(Ljava/lang/Object;)Z'");
    	return false;
    }

    // create the list of FileMetadata (the result of the listDirectory operation)
    for (unsigned int i = 0; i < list.size(); i++)
    {
    	FileMetadata fm = list[i];
    	env->CallBooleanMethod(fmList, vecAdd,
    			env->NewObject(classFileMetadata, fmConstructor, env->NewStringUTF(fm.getPath().c_str()), fm.getType(), fm.getBlobId()));
    }

	return true;

}

JNIEXPORT jlong JNICALL Java_fblobseer_NamespaceClient_ns_1handler_1createFile(JNIEnv *env, jclass c, jlong ns, jstring path, jlong page_size, jint replica_count) {
	const char *str = env->GetStringUTFChars(path, NULL);
	std::string p(str);
	env->ReleaseStringUTFChars(path, str);

	FileHandler* handler;

	// create a new file and its FileHandler
	NamespaceClient* nclient = (NamespaceClient*)ns;
	handler = nclient->createFile(p, page_size, replica_count);

	return (jlong)handler;
}

JNIEXPORT jlong JNICALL Java_fblobseer_NamespaceClient_ns_1handler_1getFileHandler(JNIEnv *env, jclass c, jlong ns, jstring path) {
	const char *str = env->GetStringUTFChars(path, NULL);
	std::string p(str);
	env->ReleaseStringUTFChars(path, str);

	FileHandler* handler;

	// create a new file and its FileHandler
	NamespaceClient* nclient = (NamespaceClient*)ns;
	handler = nclient->getFileHandler(p);

	return (jlong)handler;
}

JNIEXPORT void JNICALL Java_fblobseer_NamespaceClient_ns_1handler_1finalizeFileHandler(JNIEnv *env, jclass c, jlong fblob) {

	FileHandler* handler = (FileHandler*)fblob;

	if (handler == 0)
		return;
	delete handler;
}






