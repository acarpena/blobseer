import os
import sys
from pycb.cbException import cbException
import pycb
import stat
import urllib
import glob
import errno
import logging
import threading
import tempfile
import hashlib
import traceback
import time
import StringIO
import array

# Import the BlobSeer bindings
import fblobseer
from fblobseer import *

class cbBlobseerBackend(object):

    def __init__(self, config_file):
        
        pycb.log(logging.INFO, "Starting the Blobseer back end...")
        try:
            self.ns = NamespaceClient(config_file)
            pycb.log(logging.INFO, "done")
        except:
            pycb.log(logging.WARNING, "Configuration file error: %s" % (config_file))


    # create and return a DataObject for writing
    # bucketName and objectName will likely just be used for seeding
    # internal names, the plug should not need these values
    #  In this operation we return a data object for writing to the cb
    #  store.  The controlling code will handle moving buffers to it
    # 
    # returns <return code>,<error msg>,<data object | None>
    # 0 indicates success
    # 
    def put_object(self, bucketName, objectName):
        print "in put", bucketName, objectName
        obj = cbDataObject(self.ns, bucketName, objectName)
        return obj


    # copy an object, not yet implemented
    def copy_object(self, srcObjectName, dstObjectName, moveOrCopy, httpHeaders):
        ex = cbException('NotImplemented')
        raise ex


    # find and return a dataobject with the given key for reading
    def get_object(self, data_key):
        obj = cbDataObject(self.ns, data_key, None)
        return obj
    
    # delete the data in the given datakey
    def delete_object(self, data_key):
        self.ns.deleteFile(data_key)

    # return the size of the dataset associated with the given key
    def get_size(self, data_key):
        obj = cbDataObject(self.ns, data_key, None)
        size = obj.get_size()
        return size

    # get the modification time
    def get_mod_time(self, data_key):
        pass

    # get the md5 sum of the file
    def get_md5(self, data_key):
        obj = cbDataObject(self.ns, data_key, None)
        hash = obj.get_md5()
        obj.close()
        return hash


class cbDataObject(object):

    def __init__(self, ns, key, objectName = None):
    #       file like stuff
        #self.closed = True
        #self.encoding = 
        #self.errors
        #self.mode
        #self.name
        #self.newlines
        #self.softspace
        self.data_key = ""
        
        # key is a data_key
        if objectName == None:
            self.data_key = key
        else:
        # key is a bucket name
            bucket = key
            self.data_key = "/" + bucket+ "/" + objectName 
        self.data_key = self.data_key.strip()
        self.ns = ns

        self.metafname = self.data_key + ".meta"
        self.blockSize = pycb.config.block_size
        self.replicas = 1
        
        self.hashValue = None
        self.delete_on_close = False
        self.md5er = hashlib.md5()

        self.offset = 0
        self.file = None
        #if not openIt:
        #    return
        try:
            self.file = self.ns.getFileHandler(self.data_key)
            self.metafile = self.ns.getFileHandler(self.metafname)
        except:
            if objectName == None:
                pycb.log(logging.ERROR, "[Open file] No such file: %s" % (self.data_key))
                raise cbException('NoSuchKey')
        if objectName != None and self.file == None:
            try:
                self.file = self.ns.createFile(self.data_key, self.blockSize, self.replicas)
            except:
                pycb.log(logging.ERROR, "[Create file] Could not create file: %s" % (self.data_key))
                
            try:
                self.metafile = self.ns.createFile(self.metafname, self.blockSize, 1)
            except:
                pycb.log(logging.ERROR, "[Create file] Could not create metafile: %s" % (self.metafname))
                
        try:
            self.hashValue = self.metafile.read(0, self.metafile.getSize())
        except:
            pass


    def get_md5(self):
        if self.hashValue == None:
            v = str(self.md5er.hexdigest()).strip()
            return v
        return self.hashValue


    def get_size(self):
        return self.file.getSize()

    def get_data_key(self):
        return self.data_key
    
    def set_md5(self, hash):
        self.hashValue = hash
    
    # this is part of the work around for twisted
    def set_delete_on_close(self, delete_on_close):
        self.delete_on_close = delete_on_close

    # implement file-like methods
    def close(self):
        hashValue = self.get_md5()
        if hashValue != None:
            try:
                self.metafile.write(0, len(hashvalue) , hashvalue)
            except:
                pass
        try:
            pycb.log(logging.INFO, "Closing file")
            self.ns.destroyFileHandler(self.file)
            self.ns.destroyFileHandler(self.metafile)
        except:
            pycb.log(logging.WARNING, "error closing data object %s " % (self.data_key))

        #if self.delete_on_close:
            #self.ns.deleteFile(self.data_key)
        print "File Closed"
        
        


    def flush(self):
        pass

    #def fileno(self):
    #def isatty(self):

    def next(self):
        pass

    def read(self, size=None):
        maxSize = self.get_size()
        if self.file == None:
            raise cbException('File Closed')
        
        ret = False
        if size == None:
            size = self.blockSize
        if self.offset + size > maxSize:
            size = maxSize - self.offset
        
        if size > 0:
            block = self.file.read(self.offset, size)
            
            if block != None:
                self.offset = self.offset + size
                   
                if len(block)<size:
                    block = block + (size-len(block))*chr(0)
                
        else:
            block = ""
        
        result = str(block)
        return result

    def seek(self, offset, whence=None):
         self.offset = offset

#    def tell(self):
#    def truncate(self, size=None):

    def write(self, st):
        if self.file == None:
            raise cbException('File Closed')
        self.file.write(self.offset, len(st) , st)
        self.offset = self.offset + len(st)
        self.md5er.update(st)    

    def writelines(self, seq):
        if self.file == None:
            raise cbException('File Closed')
        for s in seq:
            self.write(s)
