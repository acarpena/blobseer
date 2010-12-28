require 'gserver'

class HTTP_Server < GServer
  def initialize(port=8080, *args)
    super(port, *args)
    @logmsg = "[HTTP-INFO]:"
  end

  def serve(io)
    query = io.gets.chomp
    respond(query, io)
  end
	
  def print_log(msg)
    print @logmsg + msg + "\n"
  end

  def respond(query,session)
    session.print "HTTP/1.1 200/OK\r\nContent-type: text/html\r\n\r\n"
    session.print "<html><head><title>BlobSeer FileSystem View</title></head><body><h1>BlobSeer FileSystem</h1>"
    session.print $filesystem.print_html
    session.print "</body></html>\r\n"
  end
end
