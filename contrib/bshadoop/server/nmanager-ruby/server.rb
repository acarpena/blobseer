if ARGV.size < 3
	print "Usage : ruby server.rb <bsfs port> <http port> <blobseer config>[optional : <backup file>]\n"
	exit
end
# requiring basic libraries
require "thread"
require "socket"
require "yaml"
require "blobseer"
require "filesystem"
require "bsfs"
require "http"

$bsfs_port = ARGV[0].to_i # the bsfs port
$http_port = ARGV[1].to_i # the http port
$bs_config = ARGV[2]  # the blobseer configuration file
$filesystem = BSFileSystem.new # the filesystem object
$object_handler = ObjectHandler.new($bs_config) # the blobseer object handler

# setting the backup file
$backup = "backup.yml" 
if ARGV[3] != nil
	$backup = ARGV[3]
	$filesystem.restore_yaml($backup)
end

# opening http server and bsfs server
$http_server = HTTP_Server.new($http_port, "0.0.0.0", 1)
$bsfs_server = BSFS_Server.new($bsfs_port, "0.0.0.0", 128)

# staring the servers
$bsfs_server.start
$http_server.start

# joining with the servers
$bsfs_server.join
$http_server.join
