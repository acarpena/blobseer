class BSFileSystem
	# class BSFile
	# stores informations about files
	class BSFile
		
		attr_accessor :length
		attr_accessor :bloblength
		attr_accessor :path
		attr_accessor :modiftime
		attr_accessor :blobid
		attr_accessor :replica
		attr_accessor :psize
		
		def initialize(path)
			@bloblength = 0
			@length = 0
			@path = path
			@modiftime = Time.now
			@blobid = -1
			@replica = 1
			@psize = 0
		end
		# get the status of the file
		def status
			if @blobid != -1
				$object_handler.get_latest(@blobid)
				@bloblength = $object_handler.get_size()
			end
			return "FILE:#{(modiftime.to_f*1000).to_i}:#{@length}:#{@replica}:#{@psize}"
		end
		
		def listed_status
			if @blobid != -1
				$object_handler.get_latest(@blobid)
				@bloblength = $object_handler.get_size()
			end
			return "#{@path};FILE;#{(@modiftime.to_f*1000).to_i};#{@length};#{@replica};#{@psize}"
		end
		# create a blob and link it with the file
		def create_blob(psize,rep)
			begin
				$object_handler.create_blob(psize, rep)
				@blobid = $object_handler.get_id
				return true
			rescue
				return false
			end
		end
	end
	# class BSDir
	# stores informations about directories
	class BSDir
		
		attr_accessor :path
		attr_accessor :modiftime
		
		def initialize(path)
			@path = path
			@modiftime = Time.now
		end
		# get the status of the directory
		def status
			return "DIR:#{(modiftime.to_f*1000).to_i}"
		end
		
		def listed_status
			return "#{@path};DIR;#{(modiftime.to_f*1000).to_i}"
		end
	end
	# constructor
	def initialize
		@mutex = Mutex.new
		@paths = Hash.new
		@paths[""] = BSDir.new("") # root directory
	end
	# exists
	def exists(path)
		return true if path == "/" 
		return false if not is_absolute(path)
		@mutex.synchronize do
			return @paths[path] != nil
		end
	end
	# mkdirs
	def mkdirs(path)
		return false if path == "/"
		return false if not is_absolute(path)
		@mutex.synchronize do
			if @paths[path] != nil
				return false
			else
				subdir = []
				tokens = path.split("/")
				for i in 0...tokens.size
					subdir << tokens[0..i].join("/")
					if @paths[subdir[-1]].is_a?(BSFile)
						return false
					end
				end
				subdir.each do |dir|
					if not @paths[dir].is_a?(BSDir)
						@paths[dir] = BSDir.new(dir)
					end
				end
				return true
			end
		end
	end
	# blobid
	def blobid(path)
		return -1 if path == "/"
		return -1 if not is_absolute(path)
		@mutex.synchronize do
			entry = @paths[path]
			if entry.is_a?(BSFile)
				return entry.blobid
			else
				return -1
			end
		end
	end
	# delete
	def delete(path)
		return false if path == "/"
		return false if not is_absolute(path)
		@mutex.synchronize do
			entry = @paths[path]
			if entry.is_a?(BSFile)
				@paths.delete(path)
				return true
			elsif entry.is_a?(BSDir)
				@paths.delete(path)
				@paths.each do |key,val|
					if key.index(path) == 0
						@paths.delete(key)
					end
				end
				return true
			else
				return false
			end
		end
	end
	# setsize
	def setsize(path,size)
		return false if path == "/"
		return false if not is_absolute(path)
		@mutex.synchronize do
			entry = @paths[path]
			if entry.is_a?(BSFile)
				@paths[path].length = size
				return true
			else
				return false
			end
		end
	end
	# create
	def create(path,rep,psize)
		return false if path == "/"
		return false if not is_absolute(path)
		@mutex.synchronize do
			entry = @paths[path]
			if entry != nil
				return false
			else
				@paths[path] = BSFile.new(path)
				@paths[path].replica = rep
				@paths[path].psize = psize
				return @paths[path].create_blob(psize,rep)
			end
		end
	end
	# isfile
	def isfile(path)
		return false if path == "/"
		return false if not is_absolute(path)
		@mutex.synchronize do
			return @paths[path].is_a?(BSFile)
		end
	end
	# rename
	def rename(oldpath,newpath)
		return false if (oldpath == "/" or newpath == "/")
		return false if not (is_absolute(oldpath) and is_absolute(newpath))
		@mutex.synchronize do
			entry = @paths[oldpath]
			if entry == nil
				return false
			elsif entry.is_a?(BSFile)
				if @paths[newpath] != nil
					return false
				else
					@paths.delete(oldpath)
					@paths[newpath] = entry
				end
			else
				if @paths[newpath] != nil
					return false
				else
					@paths.each do |key,val|
						if key.index(oldpath) == 0
							@paths.delete(key)
							newkey = key.gsub(oldpath,newpath)
							@paths[newkey] = val
							@paths[newkey].path = newkey
						end
					end
					return true
				end
			end
		end
	end
	# status
	def status(path)
		if path == "/"
			@mutex.synchronize do
				return @paths[""].status
			end
		end
		return false if not is_absolute(path)
		@mutex.synchronize do
			entry = @paths[path]
			if entry != nil
				return entry.status
			else
				return false
			end
		end
	end
	# listdir
	def listdir(path)
		if path == "/"
			path = ""
		else
			return false if not is_absolute(path)
		end
		@mutex.synchronize do
			entry = @paths[path]
			if entry == nil or entry.is_a?(BSFile)
				return false
			else
				subdir = []
				@paths.each do |key,val|
					rel_key = key[path.size..-1]
					if key.index(path) == 0 and key != path and rel_key.count("/") == 1
						subdir<< key
					end
				end
				resp = ["#{subdir.size}"]
				subdir.each do |p|
					resp << @paths[p].listed_status
				end
				return resp.join(":")
			end
		end
	end
	
	def print_html
		str = ""
		tab = []
		@mutex.synchronize do
			tab = @paths.sort
		end
		tab.each do |pair|
			key = pair[0]
			offset = "&nbsp;"*(key.count("/")*5)
			value = pair[1]
			if value.is_a?(BSDir)
				str << "<i>dir : #{offset} #{key.split("/")[-1]}/</i><br>"
			else
				str << "<i>file : #{offset} #{key.split("/")[-1]}</i><br>"
			end
		end
		return str
	end
	
	# save the filesystem in a yaml file
	def save_yaml(filename)
	#	File.open(filename,"w+") do |file|
	#		file << @paths.to_yaml
	#	end
	end
	
	# load the filesystem from a yaml file
	def restore_yaml(filename)
	#	if not FileTest.exists?(filename)
	#		@paths = Hash.new
	#		@paths[""] = BSDir.new("") # root directory
	#		return
	#	end
	#	File.open(filename,"r") do |file|
	#		@paths = YAML.load(file)
	#	end
	end
	
	# check if a path is absolute
	def is_absolute(path)
		return path[0] == "/"[0]
	end
end
