require 'gserver'

class BSFS_Server < GServer
  def initialize(port=9000, *args)
    super(port, *args)
    @logmsg = "[BSFS-INFO]:"
  end

  def serve(io)
    query = io.gets.chomp
    respond(query, io)
  end
	
  def print_log(msg)
    print @logmsg + msg + "\n"
  end
  
  def respond(query,session)
    words = query.split(":")
    begin      
      case words[0]
      when "EXISTS"
        if words.size != 2
          session.print "ERROR\n"
          print_log("[#{Time.now}] RESPONSE : (exists) ERROR (argument error)")
          return
        end
        r = $filesystem.exists(words[1].chomp)
        if r
          session.print "EXISTS:TRUE\n"
          print_log("[#{Time.now}] RESPONSE : (exists) TRUE")
        else
          session.print "EXISTS:FALSE\n"
          print_log("[#{Time.now}] RESPONSE : (exists) FALSE")
        end
      when "MKDIRS"
        if words.size != 2
          session.print "ERROR\n"
          print_log("[#{Time.now}] RESPONSE : (mkdirs) ERROR (argument error)")
          return
        end
        r = $filesystem.mkdirs(words[1].chomp)
        if r
          session.print "MKDIRS:TRUE\n"
          print_log("[#{Time.now}] RESPONSE : (mkdirs) TRUE")
          $filesystem.save_yaml($backup)
        else
          session.print "MKDIRS:FALSE\n"
          print_log("[#{Time.now}] RESPONSE : (mkdirs) FALSE")
        end
      when "BLOBID"
        if words.size != 2
          session.print "ERROR\n"
          print_log("[#{Time.now}] RESPONSE : (blobid) ERROR (argument error)")
          return
        end
        r = $filesystem.blobid(words[1].chomp)
        if r != -1
          session.print "BLOBID:#{r}\n"
          print_log("[#{Time.now}] RESPONSE : (blobid) blobid = #{r}")
        else
          session.print "BLOBID:ANY\n"
          print_log("[#{Time.now}] RESPONSE : (blobid) ERROR (no blob id)")
        end
      when "DELETE"
        if words.size != 2
          session.print "ERROR\n"
          print_log("[#{Time.now}] RESPONSE : (delete) ERROR (argument error)")
          return
        end
        r = $filesystem.delete(words[1].chomp)
        if r
          session.print "DELETE:TRUE\n"
          print_log("[#{Time.now}] RESPONSE : (delete) TRUE")
          $filesystem.save_yaml($backup)
        else
          session.print "DELETE:FALSE\n"
          print_log("[#{Time.now}] RESPONSE : (delete) FALSE")
        end
      when "SETSIZE"
        if words.size != 3
          session.print "ERROR\n"
          print_log("[#{Time.now}] RESPONSE : (setsize) ERROR (argument error)")
          return
        end
        r = $filesystem.setsize(words[1],words[2].chomp.to_i)
        if r
          session.print "SETSIZE:TRUE\n"
          print_log("[#{Time.now}] RESPONSE : (setsize) TRUE")
          $filesystem.save_yaml($backup)
        else
          session.print "SETSIZE:FALSE\n"
          print_log("[#{Time.now}] RESPONSE : (setsize) FALSE")
        end
      when "CREATE"
        if words.size != 4
          session.print "ERROR\n"
          print_log("[#{Time.now}] RESPONSE : (create) ERROR (argument error)")
          return
        end
        r = $filesystem.create(words[1],words[2].to_i,words[3].to_i)
        if r
          session.print "CREATE:TRUE\n"
          print_log("[#{Time.now}] RESPONSE : (create) TRUE")
          $filesystem.save_yaml($backup)
        else
          session.print "CREATE:FALSE\n"
          print_log("[#{Time.now}] RESPONSE : (create) FALSE")
        end
      when "ISFILE"
        if words.size != 2
          session.print "ERROR\n"
          print_log("[#{Time.now}] RESPONSE : (isfile) ERROR (argument error)")
          return
        end
        r = $filesystem.isfile(words[1].chomp)
        if r
          session.print "ISFILE:TRUE\n"
          print_log("[#{Time.now}] RESPONSE : (isfile) TRUE (argument error)")
        else
          session.print "ISFILE:FALSE\n"
          print_log("[#{Time.now}] RESPONSE : (isfile) FALSE (argument error)")
        end
      when "RENAME"
        if words.size != 3
          session.print "ERROR\n"
          print_log("[#{Time.now}] RESPONSE : (rename) ERROR (argument error)")
          return
        end
        r = $filesystem.rename(words[1],words[2].chomp)
        if r
          session.print "RENAME:TRUE\n"
          print_log("[#{Time.now}] RESPONSE : (rename) TRUE")
          $filesystem.save_yaml($backup)
        else
          session.print "RENAME:FALSE\n"
          print_log("[#{Time.now}] RESPONSE : (rename) FALSE")
        end
      when "STATUS"
        if words.size != 2
          session.print "ERROR\n"
          print_log("[#{Time.now}] RESPONSE : (status) ERROR (argument error)")
          return
        end
        r = $filesystem.status(words[1].chomp)
        if r
          session.print "STATUS:"+r+"\n"
          print_log("[#{Time.now}] RESPONSE : (status) #{r}")
        else
          session.print "STATUS:ERROR\n"
          print_log("[#{Time.now}] RESPONSE : (status) ERROR (unknown error)")
        end
      when "LISTDIR"
        if words.size != 2
          session.print "ERROR\n"
          print_log("[#{Time.now}] RESPONSE : (listdir) ERROR (argument error)")
          return
        end
        r = $filesystem.listdir(words[1].chomp)
        if r
          session.print "LISTDIR:"+r+"\n"
          print_log("[#{Time.now}] RESPONSE : (listdir) #{r}")
        else
          session.print "ERROR\n"
          print_log("[#{Time.now}] RESPONSE : (listdir) ERROR (unknown error)")
        end
      else
        session.print "ERROR\n"
        print_log("[#{Time.now}] RESPONSE : (command) ERROR (unknown command error)")
      end
    rescue
      print_log("[#{Time.now}] RUBY ERROR TRACE:")
      print "#{$@} #{$!}"
      print "\n"
      print_log("[#{Time.now}] END OF RUBY ERROR TRACE")
    ensure
      
    end
  end
end
