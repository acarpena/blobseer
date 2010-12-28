/*
 * FileHandlerJNI.cpp
 *
 *  Created on: Apr 14, 2010
 *      Author: acarpena
 */

#include "FileHandlerJNI.h"
#include "FBlobseerClient/FileHandler.hpp"

#include "common/debug.hpp"

void __attribute__ ((constructor)) FileHandlerJNI_init() {
}
void __attribute__ ((destructor)) FileHandlerJNI_fini() {
}


JNIEXPORT jint JNICALL Java_fblobseer_FileHandler_fblob_1get_1latest(JNIEnv *env, jclass c, jlong fblob) {
    return ((FileHandler*)fblob)->getLatestVersion();
}

JNIEXPORT jlong JNICALL Java_fblobseer_FileHandler_fblob_1get_1size__JI(JNIEnv *env, jclass c, jlong fblob, jint version) {
    return ((FileHandler*)fblob)->get_size(version);
}

JNIEXPORT jlong JNICALL Java_fblobseer_FileHandler_fblob_1get_1size__J(JNIEnv *env, jclass c, jlong fblob) {
    return ((FileHandler*)fblob)->get_size();
}

JNIEXPORT jint JNICALL Java_fblobseer_FileHandler_fblob_1get_1page_1size(JNIEnv *env, jclass c, jlong fblob) {
    return ((FileHandler*)fblob)->get_page_size();
}

JNIEXPORT jboolean JNICALL Java_fblobseer_FileHandler_fblob_1read(JNIEnv *env, jclass c, jlong fblob, jlong offset, jlong size, jobject jbuffer, jint version) {
    char* buffer = (char *)env->GetDirectBufferAddress(jbuffer);
    bool result = false;

    if (buffer == NULL) {
    	ERROR("could not get buffer address for reading (" << offset << ", " << size << ") from blob " << fblob);
    	return false;
	}

    try {
    	result = ((FileHandler*)fblob)->read(offset, size, buffer, version);
    } catch (std::runtime_error &e) {
    	ERROR(e.what());
    	return false;
    }
    return result;
}


JNIEXPORT jboolean JNICALL Java_fblobseer_FileHandler_fblob_1append(JNIEnv *env, jclass c, jlong fblob, jlong size, jobject jbuffer) {
    char* buffer = (char *)env->GetDirectBufferAddress(jbuffer);
    bool result = false;

    if (buffer == NULL) {
    	ERROR("could not get buffer address for appending  " << size << "bytes to blob " << fblob);
    	return false;
    }

    try {
    	result = ((FileHandler*)fblob)->append(size, buffer);
    } catch (std::runtime_error &e) {
    	ERROR(e.what());
    	return false;
    }
    return result;
}

JNIEXPORT jboolean JNICALL Java_fblobseer_FileHandler_fblob_1write(JNIEnv *env, jclass c, jlong fblob, jlong offset, jlong size, jobject jbuffer) {
    char* buffer = (char *)env->GetDirectBufferAddress(jbuffer);
    bool result = false;

    if (buffer == NULL) {
    	ERROR("could not get buffer address for writing (" << offset << ", " << size << ") to blob " << fblob);
    	return false;
    }

    try {
    	result = ((FileHandler*)fblob)->write(offset, size, buffer);
    } catch (std::runtime_error &e) {
    	ERROR(e.what());
    	return false;
    }

    return result;
}

