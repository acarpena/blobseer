/*
 * FileHandler.cpp
 *
 *  Created on: Sep 10, 2010
 *      Author: acarpena
 */
#include "FileHandlerWrapper.hpp"

using namespace boost::python;

FileHandlerWrapper::FileHandlerWrapper(FileHandler* file) {

	fileHandler = file;
}


FileHandlerWrapper::~FileHandlerWrapper() {

	delete fileHandler;
}

/* New function to read from blobseer so that the binding handles python StringIO buffers */
PyObject* FileHandlerWrapper::read(boost::uint64_t offset, boost::uint64_t size, boost::uint32_t version) {
	/*PyObject* buffer_class = PyObject_Str(PyObject_GetAttr(py_buffer, PyString_FromString("__class__")));
	PyObject* stringio_str = PyString_FromString("array");
	long find = PyInt_AsLong(PyObject_CallMethodObjArgs(buffer_class, PyString_FromString("find"), stringio_str, NULL));

	if (find == -1) {
		PyObject_Print(PyString_FromString("3rd argument of fblobseer.FileHandler.read must be a StringIO!\n"),stderr,Py_PRINT_RAW);
		return false;
	}

	char* buffer = new char[size];
	if (fileHandler->read(offset,size,buffer,version)) {
		//buffer[size]='\0';
		PyObject_CallMethodObjArgs(py_buffer, PyString_FromString("fromstring"), PyString_FromString(buffer),NULL);
		return true;
	} else
		return false;*/
	INFO("trying to read");
	char* buffer = new char[size];
		if (fileHandler->read(offset,size,buffer,version)) {
			PyObject* py_buffer = PyBuffer_FromReadWriteMemory(buffer, size);
			INFO("got a py_buffer"<<PyObject_Size(py_buffer)<<"   end");
			//handle<> h(py_buffer);
			//return object(h);
			return py_buffer;
		} else
			return 0;
}

boost::int32_t FileHandlerWrapper::get_objcount(){
	return fileHandler->get_objcount();
}

boost::uint64_t FileHandlerWrapper::get_size(boost::uint32_t version) {
	return fileHandler->get_size(version);
}

boost::uint64_t FileHandlerWrapper::getLatestVersion(void) {
	return fileHandler->getLatestVersion();
}

boost::uint64_t FileHandlerWrapper::get_page_size(void) {
	return fileHandler->get_page_size();
}


bool FileHandlerWrapper::append(boost::uint64_t size, char *buffer){

	return fileHandler->append(size, buffer);
}


bool FileHandlerWrapper::write(boost::uint64_t offset, boost::uint64_t size, char *buffer){

	return fileHandler->write(offset, size,buffer);
}
