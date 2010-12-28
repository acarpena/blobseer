#include <boost/python.hpp>
#include <boost/python/args.hpp>
#include <boost/bind.hpp>
#include <boost/mpl/vector.hpp>

// include the BlobSeer client classes
#include "NamespaceClientWrapper.hpp"
#include "FileHandlerWrapper.hpp"

using namespace boost::python;

BOOST_PYTHON_MODULE(fblobseer)
{
	class_<NamespaceClientWrapper, boost::noncopyable>("NamespaceClient", init<std::string>())
        .def("mkdir", &NamespaceClientWrapper::mkdir)
        .def("exists", &NamespaceClientWrapper::exists)
        .def("isFile", &NamespaceClientWrapper::isFile)
        .def("rename", &NamespaceClientWrapper::rename)
        .def("status", &NamespaceClientWrapper::status)
        .def("deleteFile", &NamespaceClientWrapper::deleteFile)
        .def("listDir", &NamespaceClientWrapper::listDir)
        .def("status", &NamespaceClientWrapper::status)

        .def("createFile", &NamespaceClientWrapper::createFile, return_value_policy<manage_new_object>())
        .def("getFileHandler", &NamespaceClientWrapper::getFileHandler, return_value_policy<manage_new_object>())
        .def("destroyFileHandler", &NamespaceClientWrapper::destroyFileHandler)
        ;

	class_<FileHandlerWrapper, boost::noncopyable>("FileHandler", no_init)
        .def("getLatestVersion", &FileHandlerWrapper::getLatestVersion)
        .def("read", &FileHandlerWrapper::read, (arg("offset"), arg("size"), arg("version") = 0))
        .def("write", &FileHandlerWrapper::write, (arg("offset"), arg("size"), arg("buffer")))
        .def("append", &FileHandlerWrapper::append, (arg("size"),arg("buffer")))
        .def("getSize", &FileHandlerWrapper::get_size, (arg("version") = 0))
        .def("getObjcount", &FileHandlerWrapper::get_objcount)
        .def("getPageSize", &FileHandlerWrapper::get_page_size)
        ;
}
